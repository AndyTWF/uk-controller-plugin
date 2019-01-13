#pragma once

namespace UKControllerPlugin {
    namespace Controller {
        class ControllerPositionCollection;
        class ControllerPositionHierarchy;
    }  // namespace Controller
}  // namespace UKControllerPlugin

namespace UKControllerPlugin {
    namespace Controller {

        /*
            Responsible for creating Hierarchies of controllers.
        */
        class ControllerPositionHierarchyFactory
        {
            public:
                ControllerPositionHierarchyFactory(
                    const UKControllerPlugin::Controller::ControllerPositionCollection & controllers
                );
                std::unique_ptr<UKControllerPlugin::Controller::ControllerPositionHierarchy> CreateFromJson(
                    const nlohmann::json & json
                ) const;

            private:
                // All the controller positions known to the plugin
                const UKControllerPlugin::Controller::ControllerPositionCollection & controllers;
        };
    }  // namespace Controller
}  // namespace UKControllerPlugin
