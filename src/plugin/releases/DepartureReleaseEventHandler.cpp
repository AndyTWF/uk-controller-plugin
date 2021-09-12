#include "DepartureReleaseColours.h"
#include "DepartureReleaseCountdownColours.h"
#include "DepartureReleaseEventHandler.h"
#include "DepartureReleaseRequest.h"
#include "DepartureReleaseRequestView.h"
#include "api/ApiException.h"
#include "api/ApiInterface.h"
#include "controller/ActiveCallsignCollection.h"
#include "controller/ControllerPosition.h"
#include "controller/ControllerPositionCollection.h"
#include "dialog/DialogManager.h"
#include "euroscope/EuroScopeCFlightPlanInterface.h"
#include "euroscope/EuroscopePluginLoopbackInterface.h"
#include "tag/TagData.h"
#include "task/TaskRunnerInterface.h"
#include "time/ParseTimeStrings.h"
#include "time/SystemClock.h"
#include "timer/TimerDisplay.h"
#include "windows/WinApiInterface.h"

namespace UKControllerPlugin::Releases {

    DepartureReleaseEventHandler::DepartureReleaseEventHandler(
        const Api::ApiInterface& api,
        TaskManager::TaskRunnerInterface& taskRunner,
        Euroscope::EuroscopePluginLoopbackInterface& plugin,
        const Controller::ControllerPositionCollection& controllers,
        const Controller::ActiveCallsignCollection& activeCallsigns,
        const Dialog::DialogManager& dialogManager,
        Windows::WinApiInterface& windows,
        const int releaseDecisionCallbackId,
        int releaseCancellationCallbackId)
        : releaseDecisionCallbackId(releaseDecisionCallbackId),
          releaseCancellationCallbackId(releaseCancellationCallbackId), controllers(controllers), plugin(plugin),
          dialogManager(dialogManager), api(api), taskRunner(taskRunner), activeCallsigns(activeCallsigns),
          windows(windows)
    {
    }

    void DepartureReleaseEventHandler::ProcessPushEvent(const Push::PushEvent& message)
    {
        if (message.event == "departure_release.requested") {
            this->ProcessDepartureReleaseRequestedMessage(message.data);
        } else if (message.event == "departure_release.approved") {
            this->ProcessRequestApprovedMessage(message.data);
        } else if (message.event == "departure_release.rejected") {
            this->ProcessRequestRejectedMessage(message.data);
        } else if (message.event == "departure_release.acknowledged") {
            this->ProcessRequestAcknowledgedMessage(message.data);
        } else if (message.event == "departure_release.request_cancelled") {
            this->ProcessRequestCancelledMessage(message.data);
        }
    }

    auto DepartureReleaseEventHandler::GetPushEventSubscriptions() const -> std::set<Push::PushEventSubscription>
    {
        return {{Push::PushEventSubscription::SUB_TYPE_CHANNEL, "private-departure-releases"}};
    }

    void DepartureReleaseEventHandler::AddReleaseRequest(const std::shared_ptr<DepartureReleaseRequest>& request)
    {
        std::lock_guard queueLock(this->releaseMapGuard);
        this->releaseRequests[request->Id()] = request;
    }

    auto DepartureReleaseEventHandler::GetReleaseRequest(int id) -> std::shared_ptr<DepartureReleaseRequest>
    {
        std::lock_guard queueLock(this->releaseMapGuard);
        auto request = this->releaseRequests.find(id);
        return request == this->releaseRequests.cend() ? nullptr : request->second;
    }

    auto DepartureReleaseEventHandler::DepartureReleaseRequestedMessageValid(const nlohmann::json& data) const -> bool
    {
        return data.is_object() && data.contains("id") && data.at("id").is_number_integer() &&
               data.contains("callsign") && data.at("callsign").is_string() && data.contains("requesting_controller") &&
               data.at("requesting_controller").is_number_integer() &&
               this->controllers.FetchPositionById(data.at("requesting_controller").get<int>()) != nullptr &&
               data.contains("target_controller") && data.at("target_controller").is_number_integer() &&
               this->controllers.FetchPositionById(data.at("target_controller").get<int>()) != nullptr &&
               data.contains("expires_at") && data.at("expires_at").is_string() &&
               Time::ParseTimeString(data.at("expires_at").get<std::string>()) != Time::invalidTime;
    }

    /*
     * An acknowledged message needs an id that we know about.
     */
    auto DepartureReleaseEventHandler::DepartureReleaseAcknowledgedMessageValid(const nlohmann::json& data) const
        -> bool
    {
        return data.is_object() && data.contains("id") && data.at("id").is_number_integer() &&
               this->releaseRequests.find(data.at("id").get<int>()) != this->releaseRequests.cend();
    }

    /*
     * For now, rejecting a release only needs a valid id, so borrow the acknowledge method.
     */
    auto DepartureReleaseEventHandler::DepartureReleaseRejectedMessageValid(const nlohmann::json& data) const -> bool
    {
        return this->DepartureReleaseAcknowledgedMessageValid(data);
    }

    auto DepartureReleaseEventHandler::DepartureReleaseApprovedMessageValid(const nlohmann::json& data) const -> bool
    {
        return data.is_object() && data.contains("id") && data.at("id").is_number_integer() &&
               this->releaseRequests.find(data.at("id").get<int>()) != this->releaseRequests.cend() &&
               data.contains("expires_at") &&
               (data.at("expires_at").is_null() ||
                (data.at("expires_at").is_string() &&
                 Time::ParseTimeString(data.at("expires_at").get<std::string>()) != Time::invalidTime)) &&
               data.contains("released_at") && data.at("released_at").is_string() &&
               Time::ParseTimeString(data.at("released_at").get<std::string>()) != Time::invalidTime;
    }

    /*
     * For now, cancelling a release request only needs a valid id, so borrow the acknowledge method.
     */
    auto DepartureReleaseEventHandler::DepartureReleaseCancelMessageValid(const nlohmann::json& data) const -> bool
    {
        return this->DepartureReleaseAcknowledgedMessageValid(data);
    }

    /*
     * Whether releases should be removed from the lists.
     */
    auto
    DepartureReleaseEventHandler::ReleaseShouldBeRemoved(const std::shared_ptr<DepartureReleaseRequest>& releaseRequest)
        -> bool
    {
        if (releaseRequest->Approved()) {
            return releaseRequest->ReleaseExpiryTime() +
                       std::chrono::seconds(RELEASE_DECISION_MADE_DELETE_AFTER_SECONDS) <
                   std::chrono::system_clock::now();
        }

        if (releaseRequest->Rejected()) {
            return releaseRequest->RejectedAtTime() + std::chrono::seconds(RELEASE_DECISION_MADE_DELETE_AFTER_SECONDS) <
                   std::chrono::system_clock::now();
        }

        return releaseRequest->RequestExpiryTime() < std::chrono::system_clock::now();
    }

    auto DepartureReleaseEventHandler::ControllerCanMakeReleaseDecision(
        const std::shared_ptr<DepartureReleaseRequest>& release) const -> bool
    {
        if (!release) {
            LogError("Cannot approve release, release not found");
            return false;
        }

        if (!this->activeCallsigns.UserHasCallsign() ||
            this->activeCallsigns.GetUserCallsign().GetNormalisedPosition().GetId() != release->TargetController()) {
            LogError("Cannot approve release, user not authorised to approve");
            return false;
        }

        return true;
    }

    auto DepartureReleaseEventHandler::GetTagItemDescription(int tagItemId) const -> std::string
    {
        switch (tagItemId) {
        case STATUS_INDICATOR_TAG_ITEM_ID:
            return "Departure Release Status Indicator";
        case COUNTDOWN_TAG_ITEM_ID:
            return "Departure Release Countdown";
        case REQUESTING_CONTROLLER_TAG_ITEM_ID:
            return "Departure Release Requesting Controller";
        default:
            return "";
        }
    }

    void DepartureReleaseEventHandler::SetTagItemData(Tag::TagData& tagData)
    {
        switch (tagData.GetItemCode()) {
        case STATUS_INDICATOR_TAG_ITEM_ID:
            this->SetReleaseStatusIndicatorTagData(tagData);
            break;
        case COUNTDOWN_TAG_ITEM_ID:
            this->SetReleaseCountdownTagData(tagData);
            break;
        case REQUESTING_CONTROLLER_TAG_ITEM_ID:
            this->SetRequestingControllerTagData(tagData);
            break;
        default:
            break;
        }
    }

    void DepartureReleaseEventHandler::ShowStatusDisplay(
        Euroscope::EuroScopeCFlightPlanInterface& flightplan,
        Euroscope::EuroScopeCRadarTargetInterface& radarTarget,
        const std::string& context,
        const POINT& mousePos)
    {
        std::string callsign = flightplan.GetCallsign();
        std::lock_guard queueLock(this->releaseMapGuard);
        std::set<std::shared_ptr<DepartureReleaseRequest>> releasesForCallsign;
        std::for_each(
            this->releaseRequests.cbegin(),
            this->releaseRequests.cend(),
            [callsign, &releasesForCallsign](const std::pair<int, std::shared_ptr<DepartureReleaseRequest>>& release) {
                if (release.second->Callsign() == callsign) {
                    releasesForCallsign.insert(release.second);
                }
            });

        // Dont continue if nothing to display
        if (releasesForCallsign.empty()) {
            return;
        }

        // Replace the display list
        this->releasesToDisplay = releasesForCallsign;

        // Setup the displays time and position
        DepartureReleaseRequestView::MoveAllInstances(mousePos);
        DepartureReleaseRequestView::DisplayFor(std::chrono::seconds(3));
    }

    auto DepartureReleaseEventHandler::GetReleasesToDisplay() const
        -> const std::set<std::shared_ptr<DepartureReleaseRequest>>&
    {
        return this->releasesToDisplay;
    }

    auto DepartureReleaseEventHandler::GetReleasesRequiringUsersDecision()
        -> std::set<std::shared_ptr<DepartureReleaseRequest>, CompareDepartureReleases>
    {
        if (!this->activeCallsigns.UserHasCallsign()) {
            return {};
        }

        std::set<std::shared_ptr<DepartureReleaseRequest>, CompareDepartureReleases> releases;
        auto controllerId = this->activeCallsigns.GetUserCallsign().GetNormalisedPosition().GetId();

        std::lock_guard queueLock(this->releaseMapGuard);
        for (const auto& release : this->releaseRequests) {
            if (release.second->TargetController() == controllerId && release.second->RequiresDecision()) {
                releases.insert(release.second);
            }
        }

        return releases;
    }

    /*
     * Opens a list of controllers that have pending releases for the given callsign
     * so that one can be selected for cancellation.
     */
    void DepartureReleaseEventHandler::SelectReleaseRequestToCancel(
        Euroscope::EuroScopeCFlightPlanInterface& flightplan,
        Euroscope::EuroScopeCRadarTargetInterface& radarTarget,
        const std::string& context,
        const POINT& mousePos)
    {
        if (!this->activeCallsigns.UserHasCallsign()) {
            return;
        }

        bool menuTriggered = false;
        int userControllerId = this->activeCallsigns.GetUserCallsign().GetNormalisedPosition().GetId();
        for (const auto& release : this->releaseRequests) {
            if (release.second->RequestingController() != userControllerId ||
                release.second->Callsign() != flightplan.GetCallsign()) {
                continue;
            }

            // Trigger menu if not yet triggered
            if (!menuTriggered) {
                RECT popupArea = {
                    mousePos.x,
                    mousePos.y,
                    mousePos.x + 400, // NOLINT
                    mousePos.y + 600  // NOLINT
                };

                this->plugin.TriggerPopupList(popupArea, "Cancel Departure Release", 1);
                menuTriggered = true;
            }

            // Add an item to the menu
            Plugin::PopupMenuItem menuItem;
            menuItem.firstValue =
                this->controllers.FetchPositionById(release.second->TargetController())->GetCallsign();
            menuItem.secondValue = "";
            menuItem.callbackFunctionId = this->releaseCancellationCallbackId;
            menuItem.checked = EuroScopePlugIn::POPUP_ELEMENT_NO_CHECKBOX;
            menuItem.disabled = false;
            menuItem.fixedPosition = false;
            this->plugin.AddItemToPopupList(menuItem);
        }
    }

    /*
     * EuroScope callback function where a release request has been cancelled
     * via the cancellation menu. Finds the release, then polls the API to cancel it.
     */
    void DepartureReleaseEventHandler::RequestCancelled(int functionId, const std::string& context, RECT position)
    {
        auto fp = this->plugin.GetSelectedFlightplan();
        if (!fp) {
            LogWarning("Cannot cancel release request, no flightplan selected");
            return;
        }

        auto releaseToCancel = std::find_if(
            this->releaseRequests.cbegin(),
            this->releaseRequests.cend(),
            [fp, this, context](auto releaseRequest) -> bool {
                return fp->GetCallsign() == releaseRequest.second->Callsign() &&
                       this->controllers.FetchPositionByCallsign(context).GetId() ==
                           releaseRequest.second->TargetController();
            });

        if (releaseToCancel == this->releaseRequests.cend()) {
            return;
        }

        auto release = releaseToCancel->second;
        this->taskRunner.QueueAsynchronousTask([release, this]() {
            try {
                this->api.CancelDepartureReleaseRequest(release->Id());
                this->releaseRequests.erase(release->Id());
            } catch (Api::ApiException& api) {
                LogError("ApiException whilst cancelling release request");
            }
        });
    }

    auto DepartureReleaseEventHandler::FindReleaseRequiringDecisionForCallsign(std::string callsign)
        -> std::shared_ptr<DepartureReleaseRequest>
    {
        if (!this->activeCallsigns.UserHasCallsign()) {
            return nullptr;
        }

        int userControllerId = this->activeCallsigns.GetUserCallsign().GetNormalisedPosition().GetId();
        std::lock_guard queueLock(this->releaseMapGuard);
        auto release = std::find_if(
            this->releaseRequests.cbegin(),
            this->releaseRequests.cend(),
            [&callsign,
             userControllerId](const std::pair<int, std::shared_ptr<DepartureReleaseRequest>>& release) -> bool {
                return release.second->Callsign() == callsign && release.second->RequiresDecision() &&
                       userControllerId == release.second->TargetController();
            });

        return release != this->releaseRequests.cend() ? release->second : nullptr;
    }

    void DepartureReleaseEventHandler::SetReleaseStatusIndicatorTagData(Tag::TagData& tagData)
    {
        std::lock_guard queueLock(this->releaseMapGuard);

        /*
         * Go through all the release requests and find the most recent one for each target controller.
         */
        int approvals = 0;
        int rejections = 0;
        int expiries = 0;
        int awaitingReleasedAtTime = 0;
        int relevant = 0;
        int acknowledgements = 0;
        for (auto releaseRequest = this->releaseRequests.rbegin(); releaseRequest != this->releaseRequests.rend();
             ++releaseRequest

        ) {
            if (releaseRequest->second->Callsign() != tagData.GetFlightplan().GetCallsign()) {
                continue;
            }

            if (releaseRequest->second->Approved()) {
                approvals++;
                if (releaseRequest->second->ApprovalExpired()) {
                    expiries++;
                }

                if (releaseRequest->second->AwaitingReleasedTime()) {
                    awaitingReleasedAtTime++;
                }
            } else if (releaseRequest->second->Rejected()) {
                rejections++;
            } else if (releaseRequest->second->RequestExpired()) {
                expiries++;
            } else if (releaseRequest->second->Acknowledged()) {
                acknowledgements++;
            }

            relevant++;
        }

        // No releases, nothing to do
        if (relevant == 0) {
            return;
        }

        /*
         * Set the indicator colour and text.
         *
         * If any release request is rejected: display rejected colour.
         * If all outstanding requests are approved:
         * - Show released colour if all releases have passed release time
         * - Show pending time colour if still waiting for release time to pass
         * - Show expired colour if release has expired
         * Otherwise: display pending
         */
        COLORREF indicatorColour = statusIndicatorReleasePending;
        if (rejections != 0) {
            indicatorColour = statusIndicatorReleaseRejected;
        } else if (expiries != 0) {
            indicatorColour = statusIndicatorReleaseExpired;
        } else if (acknowledgements == relevant) {
            indicatorColour = statusIndicatorReleaseAcknowledged;
        } else if (approvals == relevant) {
            indicatorColour =
                awaitingReleasedAtTime != 0 ? statusIndicatorReleasedAwaitingTime : statusIndicatorReleased;
        }
        tagData.SetItemString(std::to_string(approvals - expiries) + "/" + std::to_string(relevant));
        tagData.SetTagColour(indicatorColour);
    }

    /*
     * Get the release countdown timer.
     */
    void DepartureReleaseEventHandler::SetReleaseCountdownTagData(Tag::TagData& tagData)
    {
        std::lock_guard queueLock(this->releaseMapGuard);

        std::set<std::shared_ptr<DepartureReleaseRequest>> relevantReleases;
        int releasesPendingReleaseTime = 0;
        for (auto releaseRequest = this->releaseRequests.rbegin(); releaseRequest != this->releaseRequests.rend();
             ++releaseRequest) {
            if (releaseRequest->second->Callsign() != tagData.GetFlightplan().GetCallsign()) {
                continue;
            }

            // If we find a release that isn't approved or approval has expired, do nothing
            if (!releaseRequest->second->Approved() ||
                (releaseRequest->second->Approved() && releaseRequest->second->ApprovalExpired())) {
                return;
            }

            if (releaseRequest->second->AwaitingReleasedTime()) {
                releasesPendingReleaseTime++;
            }

            relevantReleases.insert(releaseRequest->second);
        }

        // No releases, nothing to do
        if (relevantReleases.empty()) {
            return;
        }

        std::chrono::system_clock::time_point countdownTime;
        if (releasesPendingReleaseTime != 0) {
            // Find the release that has a released at time that is furthest away
            std::chrono::system_clock::time_point furthestPendingRelease =
                (std::chrono::system_clock::time_point::min)();

            for (const auto& relevantRelease : relevantReleases) {
                if (relevantRelease->ReleasedAtTime() > furthestPendingRelease) {
                    furthestPendingRelease = relevantRelease->ReleasedAtTime();
                }
            }

            countdownTime = furthestPendingRelease;
            tagData.SetTagColour(PendingReleaseTimeColour());
        } else {
            // Find the release that has the closest expiry time
            std::chrono::system_clock::time_point closestReleaseExpiry = (std::chrono::system_clock::time_point::max)();

            for (const auto& relevantRelease : relevantReleases) {
                if (!relevantRelease->ApprovedWithNoExpiry() &&
                    relevantRelease->ReleaseExpiryTime() < closestReleaseExpiry) {
                    closestReleaseExpiry = relevantRelease->ReleaseExpiryTime();
                }
            }

            // If no releases have an expiry point, no countdown to display
            if (closestReleaseExpiry == (std::chrono::system_clock::time_point::max)()) {
                return;
            }

            countdownTime = closestReleaseExpiry;
            tagData.SetTagColour(TimeUntilExpiryColour(closestReleaseExpiry));
        }

        // Set the timer display
        tagData.SetItemString(Timer::GetTimerDisplay(countdownTime));
    }

    /*
     * Set the requesting controller tag data.
     */
    void DepartureReleaseEventHandler::SetRequestingControllerTagData(Tag::TagData& tagData)
    {
        auto release = this->FindReleaseRequiringDecisionForCallsign(tagData.GetFlightplan().GetCallsign());
        if (!release) {
            return;
        }

        std::string requestingControllerCallsign =
            this->controllers.FetchPositionById(release->RequestingController())->GetCallsign();

        tagData.SetItemString(
            this->activeCallsigns.PositionActive(requestingControllerCallsign)
                ? this->activeCallsigns.GetLeadCallsignForPosition(requestingControllerCallsign).GetCallsign()
                : requestingControllerCallsign);
    }

    auto
    DepartureReleaseEventHandler::UserRequestedRelease(const std::shared_ptr<DepartureReleaseRequest>& request) const
        -> bool
    {
        return this->activeCallsigns.UserHasCallsign() &&
               this->activeCallsigns.GetUserCallsign().GetNormalisedPosition().GetId() ==
                   request->RequestingController();
    }

    /**
     * Create a new departure release request.
     */
    void DepartureReleaseEventHandler::ProcessDepartureReleaseRequestedMessage(const nlohmann::json& data)
    {
        std::lock_guard queueLock(this->releaseMapGuard);
        if (!DepartureReleaseRequestedMessageValid(data)) {
            LogError("Invalid departure release requested message");
            return;
        }

        // Add the release
        int releaseRequestId = data.at("id").get<int>();
        int targetController = data.at("target_controller").get<int>();
        auto callsign = data.at("callsign").get<std::string>();
        this->releaseRequests[data.at("id").get<int>()] = std::make_shared<DepartureReleaseRequest>(
            releaseRequestId,
            callsign,
            data.at("requesting_controller").get<int>(),
            targetController,
            Time::ParseTimeString(data.at("expires_at").get<std::string>()));

        // Remove any others for the same callsign and controller
        for (auto releaseRequest = this->releaseRequests.begin(); releaseRequest != this->releaseRequests.end();) {
            if (releaseRequest->second->Callsign() == callsign &&
                releaseRequest->second->TargetController() == targetController &&
                releaseRequest->second->Id() != releaseRequestId) {
                releaseRequest = this->releaseRequests.erase(releaseRequest);
            } else {
                ++releaseRequest;
            }
        }

        // Play a sound to alert the controller if we are the target
        if (this->activeCallsigns.UserHasCallsign() &&
            this->activeCallsigns.GetUserCallsign().GetNormalisedPosition().GetId() == targetController) {
            this->windows.PlayWave(MAKEINTRESOURCE(WAVE_DEP_RLS_REQ)); // NOLINT
        }
    }

    /**
     * Acknowledge a departure release request.
     */
    void DepartureReleaseEventHandler::ProcessRequestAcknowledgedMessage(const nlohmann::json& data)
    {
        std::lock_guard queueLock(this->releaseMapGuard);
        if (!DepartureReleaseAcknowledgedMessageValid(data)) {
            LogError("Invalid departure release acknowledged message");
            return;
        }

        this->releaseRequests.find(data.at("id").get<int>())->second->Acknowledge();
    }

    /**
     * Reject a departure release request.
     */
    void DepartureReleaseEventHandler::ProcessRequestRejectedMessage(const nlohmann::json& data)
    {
        std::lock_guard queueLock(this->releaseMapGuard);
        if (!DepartureReleaseRejectedMessageValid(data)) {
            LogError("Invalid departure release rejected message");
            return;
        }

        auto release = this->releaseRequests.find(data.at("id").get<int>())->second;
        release->Reject();

        // Play a sound to alert the controller if we requested it
        if (this->UserRequestedRelease(release)) {
            this->windows.PlayWave(MAKEINTRESOURCE(WAVE_DEP_RLS_REJ)); // NOLINT
        }
    }

    /**
     * Approve a departure release request.
     */
    void DepartureReleaseEventHandler::ProcessRequestApprovedMessage(const nlohmann::json& data)
    {
        std::lock_guard queueLock(this->releaseMapGuard);
        if (!DepartureReleaseApprovedMessageValid(data)) {
            LogError("Invalid departure release approved message");
            return;
        }

        auto release = this->releaseRequests.find(data.at("id").get<int>())->second;
        if (data.at("expires_at").is_null()) {
            release->Approve(Time::ParseTimeString(data.at("released_at").get<std::string>()));
        } else {
            release->Approve(
                Time::ParseTimeString(data.at("released_at").get<std::string>()),
                Time::ParseTimeString(data.at("expires_at").get<std::string>()));
        }

        // Play a sound to alert the controller if we requested it
        if (this->UserRequestedRelease(release)) {
            this->windows.PlayWave(MAKEINTRESOURCE(WAVE_DEP_RLS_ACCEPT)); // NOLINT
        }
    }

    /**
     * Cancel a departure release request.
     */
    void DepartureReleaseEventHandler::ProcessRequestCancelledMessage(const nlohmann::json& data)
    {
        if (!DepartureReleaseCancelMessageValid(data)) {
            LogError("Invalid departure release cancelled message");
            return;
        }

        std::lock_guard queueLock(this->releaseMapGuard);
        this->releaseRequests.erase(data.at("id").get<int>());
    }

    /*
     * Remove releases that have expired and no longer need
     * to be displayed.
     */
    void DepartureReleaseEventHandler::TimedEventTrigger()
    {
        std::lock_guard queueLock(this->releaseMapGuard);
        for (auto release = this->releaseRequests.cbegin(); release != this->releaseRequests.cend();) {
            if (this->ReleaseShouldBeRemoved(release->second)) {
                release = this->releaseRequests.erase(release);
            } else {
                ++release;
            }
        }
    }

    /*
     * Open the departure release request dialog to start requesting a new
     * release.
     */
    void DepartureReleaseEventHandler::OpenRequestDialog(
        Euroscope::EuroScopeCFlightPlanInterface& flightplan,
        Euroscope::EuroScopeCRadarTargetInterface& radarTarget,
        const std::string& context,
        const POINT& mousePos)
    {
        if (!this->activeCallsigns.UserHasCallsign() ||
            !this->activeCallsigns.GetUserCallsign().GetNormalisedPosition().RequestsDepartureReleases()) {
            LogWarning("Not opening departure release request dialog, user cannot request releases");
            return;
        }

        std::string callsign = flightplan.GetCallsign();
        this->dialogManager.OpenDialog(
            IDD_DEPARTURE_RELEASE_REQUEST, reinterpret_cast<LPARAM>(callsign.c_str())); // NOLINT
    }

    /*
     * Open the decision popup menu for a particular departure release.
     */
    void DepartureReleaseEventHandler::OpenDecisionMenu(
        Euroscope::EuroScopeCFlightPlanInterface& flightplan,
        Euroscope::EuroScopeCRadarTargetInterface& radarTarget,
        const std::string& context,
        const POINT& mousePos)
    {
        auto release = this->FindReleaseRequiringDecisionForCallsign(flightplan.GetCallsign());
        if (!release) {
            return;
        }

        this->plugin.SetEuroscopeSelectedFlightplan(flightplan);

        // Create the list in place
        RECT popupArea = {
            mousePos.x,
            mousePos.y,
            mousePos.x + 400, // NOLINT
            mousePos.y + 600  // NOLINT
        };

        this->plugin.TriggerPopupList(popupArea, "Departure Release Decision", 1);

        Plugin::PopupMenuItem menuItem;
        menuItem.firstValue = "";
        menuItem.secondValue = "";
        menuItem.callbackFunctionId = this->releaseDecisionCallbackId;
        menuItem.checked = EuroScopePlugIn::POPUP_ELEMENT_NO_CHECKBOX;
        menuItem.disabled = false;
        menuItem.fixedPosition = false;

        menuItem.firstValue = "Approve";
        this->plugin.AddItemToPopupList(menuItem);
        menuItem.firstValue = "Reject";
        this->plugin.AddItemToPopupList(menuItem);
        menuItem.firstValue = "Acknowledge";
        this->plugin.AddItemToPopupList(menuItem);
    }

    /*
     * Departure release decision has been made, process that decision.
     */
    void DepartureReleaseEventHandler::ReleaseDecisionMade(int functionId, const std::string& context, RECT position)
    {
        // Check we can make this decision
        auto fp = this->plugin.GetSelectedFlightplan();
        if (!fp) {
            LogWarning("Cannot make release decision, no callsign selected");
            return;
        }

        auto release = this->FindReleaseRequiringDecisionForCallsign(fp->GetCallsign());
        if (!release) {
            return;
        }

        // Release has been approved
        if (context == "Approve") {
            this->dialogManager.OpenDialog(
                IDD_DEPARTURE_RELEASE_APPROVE,
                reinterpret_cast<LPARAM>(&release) // NOLINT
            );
            return;
        }

        // Release has been rejected
        if (context == "Reject") {
            this->taskRunner.QueueAsynchronousTask([this, release]() {
                try {
                    this->api.RejectDepartureReleaseRequest(release->Id(), release->TargetController());
                    release->Reject();
                    LogInfo("Rejected departure release id " + std::to_string(release->Id()));
                } catch (Api::ApiException& apiException) {
                    LogError("ApiException when rejecting departure release: " + std::string(apiException.what()));
                }
            });
            return;
        }

        // Release has been acknowledged
        this->taskRunner.QueueAsynchronousTask([this, release]() {
            try {
                this->api.AcknowledgeDepartureReleaseRequest(release->Id(), release->TargetController());
                release->Acknowledge();
                LogInfo("Acknowledged departure release id " + std::to_string(release->Id()));
            } catch (Api::ApiException& apiException) {
                LogError("ApiException when acknowledging departure release: " + std::string(apiException.what()));
            }
        });
    }

    /*
     * Given a callsign and a target controller, check that releases can be performed
     * and perform the request. Create the request locally once we have an id to
     * tide us over until the next event update.
     */
    void DepartureReleaseEventHandler::RequestRelease(const std::string& callsign, int targetControllerId)
    {
        this->taskRunner.QueueAsynchronousTask([this, callsign, targetControllerId]() {
            auto controller = this->controllers.FetchPositionById(targetControllerId);
            if (!controller || !controller->ReceivesDepartureReleases()) {
                LogError("Cannot request release, target controller is invalid");
                return;
            }

            if (!this->activeCallsigns.UserHasCallsign() ||
                !this->activeCallsigns.GetUserCallsign().GetNormalisedPosition().RequestsDepartureReleases()) {
                LogError("Cannot request release, controller position cannot request releases");
                return;
            }

            int userPositionId = this->activeCallsigns.GetUserCallsign().GetNormalisedPosition().GetId();

            try {
                nlohmann::json response = this->api.RequestDepartureRelease(
                    callsign,
                    this->activeCallsigns.GetUserCallsign().GetNormalisedPosition().GetId(),
                    targetControllerId,
                    RELEASE_EXPIRY_SECONDS);

                if (!response.contains("id") || !response.at("id").is_number_integer()) {
                    LogError("Api returned invalid departure release id");
                    return;
                }

                this->AddReleaseRequest(std::make_shared<DepartureReleaseRequest>(
                    response.at("id").get<int>(),
                    callsign,
                    userPositionId,
                    targetControllerId,
                    Time::TimeNow() + std::chrono::seconds(RELEASE_EXPIRY_SECONDS)));
                LogInfo(
                    "Requested departure release id " + std::to_string(response.at("id").get<int>()) + " for " +
                    callsign);
            } catch (Api::ApiException& apiException) {
                LogError("ApiException when requesting departure release");
            }
        });
    }

    /*
     * Given a release id, approve it.
     */
    void DepartureReleaseEventHandler::ApproveRelease(
        int releaseId, std::chrono::system_clock::time_point releasedAt, int expiresInSeconds)
    {
        auto release = this->GetReleaseRequest(releaseId);
        if (!this->ControllerCanMakeReleaseDecision(release)) {
            return;
        }

        this->taskRunner.QueueAsynchronousTask([this, release, releasedAt, expiresInSeconds]() {
            try {
                this->api.ApproveDepartureReleaseRequest(
                    release->Id(), release->TargetController(), releasedAt, expiresInSeconds);

                if (expiresInSeconds == -1) {
                    release->Approve(releasedAt);
                } else {
                    release->Approve(releasedAt, releasedAt + std::chrono::seconds(expiresInSeconds));
                }
                LogInfo("Approved departure release id " + std::to_string(release->Id()));
            } catch (Api::ApiException&) {
                LogError("ApiException approving departure release " + std::to_string(release->Id()));
            }
        });
    }
} // namespace UKControllerPlugin::Releases
