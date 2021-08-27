#pragma once

namespace UKControllerPlugin {
    namespace Bootstrap {
        struct PersistenceContainer;
    } // namespace Bootstrap
    namespace Dependency {
        class DependencyLoaderInterface;
    } // namespace Dependency
} // namespace UKControllerPlugin

namespace UKControllerPlugin::Wake {

    void BootstrapPlugin(
        const UKControllerPlugin::Bootstrap::PersistenceContainer& container,
        UKControllerPlugin::Dependency::DependencyLoaderInterface& dependencies);

    [[nodiscard]] auto GetWakeDependencyKey() -> std::string;
    [[nodiscard]] auto GetRecatDependencyKey() -> std::string;
} // namespace UKControllerPlugin::Wake
