#pragma once
#include "push/PushEventProcessorInterface.h"

namespace UKControllerPlugin::Controller {
    class ControllerPositionCollection;
} // namespace UKControllerPlugin::Controller

namespace UKControllerPlugin::Releases {
    class DepartureReleaseRequestCollection;
    class DepartureReleaseRequestEventHandlerCollection;

    class NewDepartureReleasePushEventHandler : public Push::PushEventProcessorInterface
    {
        public:
        NewDepartureReleasePushEventHandler(
            const std::shared_ptr<DepartureReleaseRequestCollection>& releases,
            const std::shared_ptr<DepartureReleaseRequestEventHandlerCollection>& eventHandlers,
            const Controller::ControllerPositionCollection& controllers);
        void ProcessPushEvent(const Push::PushEvent& message) override;
        [[nodiscard]] auto GetPushEventSubscriptions() const -> std::set<Push::PushEventSubscription> override;

        private:
        [[nodiscard]] auto MessageValid(const nlohmann::json& message) -> bool;

        // Collection of all the releases
        std::shared_ptr<DepartureReleaseRequestCollection> releases;

        // Handle release request events
        std::shared_ptr<DepartureReleaseRequestEventHandlerCollection> eventHandlers;

        // Collection of all the controllers
        const Controller::ControllerPositionCollection& controllers;
    };
} // namespace UKControllerPlugin::Releases
