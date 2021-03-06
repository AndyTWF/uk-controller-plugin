#include "pch/stdafx.h"
#include "bootstrap/HelperBootstrap.h"
#include "bootstrap/PersistenceContainer.h"
#include "setting/SettingRepository.h"
#include "setting/SettingRepositoryFactory.h"
#include "task/TaskRunner.h"
#include "api/ApiHelper.h"
#include "api/LocateApiSettings.h"
#include "radarscreen/ConfigurableDisplayCollection.h"
#include "api/ApiConfigurationMenuItem.h"
#include "euroscope/CallbackFunction.h"

using UKControllerPlugin::Bootstrap::PersistenceContainer;
using UKControllerPlugin::Setting::SettingRepository;
using UKControllerPlugin::Setting::SettingRepositoryFactory;
using UKControllerPlugin::Api::ApiHelper;
using UKControllerPlugin::Api::ApiRequestBuilder;
using UKControllerPlugin::TaskManager::TaskRunner;
using UKControllerPlugin::RadarScreen::ConfigurableDisplayCollection;
using UKControllerPlugin::Api::ApiConfigurationMenuItem;
using UKControllerPlugin::Euroscope::CallbackFunction;

namespace UKControllerPlugin {
    namespace Bootstrap {

        /*
            Create the various helpers we use and put them in the persistence structure.
        */
        void HelperBootstrap::Bootstrap(PersistenceContainer & persistence)
        {
            persistence.settingsRepository = SettingRepositoryFactory::Create(*persistence.windows);

            // Prompt for a settings file, if one isn't there.
            Api::LocateApiSettings(*persistence.windows, *persistence.settingsRepository);

            ApiRequestBuilder requestBuilder(
                persistence.settingsRepository->GetSetting("api-url", "https://ukcp.vatsim.uk"),
                persistence.settingsRepository->GetSetting("api-key")
            );
            persistence.api.reset(
                new ApiHelper(*persistence.curl, requestBuilder)
            );

            persistence.taskRunner = std::make_unique<TaskRunner>(3);
        }

        /*
            Create the ApiConfigurationMenuItem and bootstrap it in.
        */
        void HelperBootstrap::BootstrapApiConfigurationItem(
            const PersistenceContainer & persistence,
            ConfigurableDisplayCollection & configurableDisplays
        ) {
            unsigned int callbackId = persistence.pluginFunctionHandlers->ReserveNextDynamicFunctionId();
            std::shared_ptr<ApiConfigurationMenuItem> menuItem = std::make_shared<ApiConfigurationMenuItem>(
                *persistence.windows,
                callbackId
            );

            CallbackFunction menuItemSelectedCallback(
                callbackId,
                "API Configuration Menu Item Selected",
                std::bind(
                    &ApiConfigurationMenuItem::Configure,
                    menuItem,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3
                )
            );

            persistence.pluginFunctionHandlers->RegisterFunctionCall(menuItemSelectedCallback);
            configurableDisplays.RegisterDisplay(menuItem);
        }
    }  // namespace Bootstrap
}  // namespace UKControllerPlugin
