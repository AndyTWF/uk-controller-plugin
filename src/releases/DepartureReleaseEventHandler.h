#pragma once
#include "websocket/WebsocketEventProcessorInterface.h"
#include "timedevent/AbstractTimedEvent.h"
#include "tag/TagItemInterface.h"

namespace UKControllerPlugin {
    namespace Controller {
        class ControllerPositionCollection;
        class ActiveCallsignCollection;
    } // namespace Controller
    namespace Dialog {
        class DialogManager;
    } // namespace Dialog
    namespace Euroscope {
        class EuroscopePluginLoopbackInterface;
        class EuroScopeCFlightPlanInterface;
        class EuroScopeCRadarTargetInterface;
    } // namespace Euroscope
    namespace TaskManager {
        class TaskRunnerInterface;
    } // namespace TaskManager
    namespace Api {
        class ApiInterface;
    } // namespace Api

    namespace Releases {

        class DepartureReleaseRequest;

        /*
            Handles events relating to departure releases.
        */
        class DepartureReleaseEventHandler : public Websocket::WebsocketEventProcessorInterface,
                                             public TimedEvent::AbstractTimedEvent,
                                             public Tag::TagItemInterface
        {
            public:
                explicit DepartureReleaseEventHandler(
                    const Api::ApiInterface& api,
                    TaskManager::TaskRunnerInterface& taskRunner,
                    Euroscope::EuroscopePluginLoopbackInterface& plugin,
                    const Controller::ControllerPositionCollection& controllers,
                    const Controller::ActiveCallsignCollection& activeCallsigns,
                    const Dialog::DialogManager& dialogManager,
                    int triggerRequestDialogFunctionId,
                    int triggerDecisionMenuFunctionId,
                    int releaseDecisionCallbackId
                );
                void ProcessWebsocketMessage(const Websocket::WebsocketMessage& message) override;
                std::set<Websocket::WebsocketSubscription> GetSubscriptions() const override;
                void AddReleaseRequest(std::shared_ptr<DepartureReleaseRequest> request);
                std::shared_ptr<DepartureReleaseRequest> GetReleaseRequest(int id);
                void TimedEventTrigger() override;
                void OpenRequestDialog(
                    Euroscope::EuroScopeCFlightPlanInterface& flightplan,
                    Euroscope::EuroScopeCRadarTargetInterface& radarTarget,
                    std::string context,
                    const POINT& mousePos
                );
                void OpenDecisionMenu(
                    Euroscope::EuroScopeCFlightPlanInterface& flightplan,
                    Euroscope::EuroScopeCRadarTargetInterface& radarTarget,
                    std::string context,
                    const POINT& mousePos
                );
                void ReleaseDecisionMade(int functionId, std::string context, RECT);
                void RequestRelease(std::string callsign, int targetControllerId);
                void ApproveRelease(
                    int releaseId,
                    std::chrono::system_clock::time_point releasedAt,
                    int expiresInSeconds
                );
                std::string GetTagItemDescription(int tagItemId) const override;
                void SetTagItemData(Tag::TagData& tagData) override;

            private:

                void ProcessDepartureReleaseRequestedMessage(const nlohmann::json& data);
                void ProcessRequestAcknowledgedMessage(const nlohmann::json& data);
                void ProcessRequestRejectedMessage(const nlohmann::json& data);
                void ProcessRequestApprovedMessage(const nlohmann::json& data);
                void ProcessRequestCancelledMessage(const nlohmann::json& data);

                bool DepartureReleaseRequestedMessageValid(const nlohmann::json& data) const;
                bool DepartureReleaseAcknowledgedMessageValid(const nlohmann::json& data) const;
                bool DepartureReleaseRejectedMessageValid(const nlohmann::json& data) const;
                bool DepartureReleaseApprovedMessageValid(const nlohmann::json& data) const;
                bool DepartureReleaseCancelMessageValid(const nlohmann::json& data) const;
                bool ReleaseShouldBeRemoved(const std::shared_ptr<DepartureReleaseRequest>& releaseRequest);
                bool ControllerCanMakeReleaseDecision(
                    const std::shared_ptr<DepartureReleaseRequest>& releaseRequest
                ) const;
                const std::shared_ptr<DepartureReleaseRequest> FindReleaseRequiringDecisionForCallsign(
                    std::string callsign
                ) const;
                void SetReleaseStatusIndicatorTagData(Tag::TagData& tagData) const;
                void SetReleaseCountdownTagData(Tag::TagData& tagData) const;

                // A guard on the map to allow async operations
                std::mutex releaseMapGuard;

                // TAG function that triggers the request dialog
                const int triggerRequestDialogFunctionId;

                // TAG function that triggers the approve menu
                const int triggerDecisionMenuFunctionId;

                // Callback for when a release decision is made
                const int releaseDecisionCallbackId;

                // Release requests in progress
                std::map<int, std::shared_ptr<DepartureReleaseRequest>> releaseRequests;

                // Controller positions
                const Controller::ControllerPositionCollection& controllers;

                // Plugin
                Euroscope::EuroscopePluginLoopbackInterface& plugin;

                // For loading dialogs
                const Dialog::DialogManager& dialogManager;

                // For making API calls
                const Api::ApiInterface& api;

                // For running tasks asynchronously
                TaskManager::TaskRunnerInterface& taskRunner;

                // All the active controllers
                const Controller::ActiveCallsignCollection& activeCallsigns;
        };
    }  // namespace Releases
}  // namespace UKControllerPlugin