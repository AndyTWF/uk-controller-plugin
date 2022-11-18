#include "DepartureReleaseRequest.h"
#include "DepartureReleaseRequestCollection.h"
#include "DepartureReleaseRequestEventHandlerCollection.h"
#include "NewDepartureReleasePushEventHandler.h"
#include "controller/ControllerPositionCollection.h"
#include "push/PushEventSubscription.h"
#include "time/ParseTimeStrings.h"

using UKControllerPlugin::Push::PushEventSubscription;

namespace UKControllerPlugin::Releases {

    NewDepartureReleasePushEventHandler::NewDepartureReleasePushEventHandler(
        const std::shared_ptr<DepartureReleaseRequestCollection>& releases,
        const std::shared_ptr<DepartureReleaseRequestEventHandlerCollection>& eventHandlers,
        const Controller::ControllerPositionCollection& controllers)
        : releases(releases), eventHandlers(eventHandlers), controllers(controllers)
    {
        assert(releases && "departure release collection is nullptr");
        assert(eventHandlers && "event handler collection is nullptr");
    }

    void NewDepartureReleasePushEventHandler::ProcessPushEvent(const Push::PushEvent& message)
    {
        const auto& data = message.data;
        if (!MessageValid(data)) {
            LogError("Invalid new release request message");
            return;
        }

        // Add the release#
        auto newRelease = std::make_shared<DepartureReleaseRequest>(
            data.at("id").get<int>(),
            data.at("callsign").get<std::string>(),
            controllers.FetchPositionById(data.at("requesting_controller").get<int>()),
            controllers.FetchPositionById(data.at("target_controller").get<int>()),
            Time::ParseTimeString(data.at("expires_at").get<std::string>()));
        releases->Add(newRelease);

        releases->RemoveWhere([&newRelease](const DepartureReleaseRequest& releaseRequest) -> bool {
            return releaseRequest.Callsign() == newRelease->Callsign() &&
                   releaseRequest.TargetControllerId() == newRelease->TargetControllerId() &&
                   releaseRequest.Id() != newRelease->Id();
        });

        // Trigger event
        eventHandlers->NewRelease(*newRelease);
    }

    auto NewDepartureReleasePushEventHandler::GetPushEventSubscriptions() const -> std::set<PushEventSubscription>
    {
        return {{PushEventSubscription::SUB_TYPE_EVENT, "departure_release.requested"}};
    }

    auto NewDepartureReleasePushEventHandler::MessageValid(const nlohmann::json& message) -> bool
    {
        return message.is_object() && message.contains("id") && message.at("id").is_number_integer() &&
               message.contains("callsign") && message.at("callsign").is_string() &&
               message.contains("requesting_controller") && message.at("requesting_controller").is_number_integer() &&
               controllers.FetchPositionById(message.at("requesting_controller").get<int>()) != nullptr &&
               message.contains("target_controller") && message.at("target_controller").is_number_integer() &&
               controllers.FetchPositionById(message.at("target_controller").get<int>()) != nullptr &&
               message.contains("expires_at") && message.at("expires_at").is_string() &&
               Time::ParseTimeString(message.at("expires_at").get<std::string>()) != Time::invalidTime;
    }
} // namespace UKControllerPlugin::Releases
