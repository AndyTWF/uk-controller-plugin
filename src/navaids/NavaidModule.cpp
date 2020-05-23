#include "pch/stdafx.h"
#include "navaids/NavaidModule.h"
#include "navaids/NavaidCollection.h"

using UKControllerPlugin::Bootstrap::PersistenceContainer;
using UKControllerPlugin::Dependency::DependencyLoaderInterface;

namespace UKControllerPlugin {
    namespace Navaids {

        void BootstrapPlugin(
            PersistenceContainer& container,
            DependencyLoaderInterface& dependencies
        ) {
            container.navaids = std::make_unique<NavaidCollection>();

            nlohmann::json navaidData = dependencies.LoadDependency("DEPENDENCY_NAVAIDS", "{}"_json);

            if (!navaidData.is_array()) {
                LogWarning("Navaid data is invalid");
                return;
            }

            for (
                nlohmann::json::const_iterator it = navaidData.cbegin();
                it != navaidData.cend();
                ++it
            ) {
                if (!NavaidValid(*it)) {
                    LogWarning("Invalid navaid detected " + it->dump());
                    continue;
                }

                EuroScopePlugIn::CPosition position;
                position.LoadFromStrings(
                    it->at("longitude").get<std::string>().c_str(),
                    it->at("latitude").get<std::string>().c_str()
                );

                container.navaids->AddNavaid(
                    { 
                        it->at("id").get<int>(),
                        it->at("identifier").get<std::string>(),
                        position
                    }
                );
            }

            LogInfo("Loaded " + std::to_string(container.navaids->Count()) + " navaids");
        }

        bool NavaidValid(const nlohmann::json& navaid)
        {
            return navaid.is_object() &&
                navaid.contains("id") &&
                navaid.at("id").is_number_integer() &&
                navaid.contains("latitude") &&
                navaid.at("latitude").is_string() &&
                navaid.contains("longitude") &&
                navaid.at("longitude").is_string() &&
                (EuroScopePlugIn::CPosition()).LoadFromStrings(
                    navaid.at("longitude").get<std::string>().c_str(),
                    navaid.at("latitude").get<std::string>().c_str()
                ) &&
                navaid.contains("identifier") && 
                navaid.at("identifier").is_string();
        }
    }  // namespace Navaids
}  // namespace UKControllerPlugin
