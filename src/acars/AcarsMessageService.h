#pragma once
#include "curl/CurlInterface.h"
#include "acars/AcarsCredentials.h"
#include "task/TaskRunnerInterface.h"

namespace UKControllerPlugin {
    namespace Acars {

        class AcarsMessageService
        {
            public:
            
                AcarsMessageService(
                    UKControllerPlugin::Curl::CurlInterface& curl,
                    UKControllerPlugin::TaskManager::TaskRunnerInterface& taskRunner
                );
                void Login(UKControllerPlugin::Acars::AcarsCredentials credentials);
                bool IsLoggedIn(void) const;

                // The URL to hit
                const std::string acarsUrl = "http://www.hoppie.nl/acars/system/connect.html";

            private:
                
                // For running tasks asynchronously
                UKControllerPlugin::TaskManager::TaskRunnerInterface& taskRunner;

                // For making CURL requests
                UKControllerPlugin::Curl::CurlInterface& curl;

                // The login credentials
                UKControllerPlugin::Acars::AcarsCredentials loginCredentials;

                // Are we logged into the ACARS system
                bool isLoggedIn = false;
        };
    }  // namespace Acars
}  // namespace UKControllerPlugin
