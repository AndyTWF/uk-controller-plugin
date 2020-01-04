#pragma once
#include "pch/stdafx.h"
#include "acars/AcarsMessageService.h"
#include "curl/CurlRequest.h"
#include "curl/CurlEscaper.h"

using UKControllerPlugin::Curl::CurlInterface;
using UKControllerPlugin::Curl::CurlRequest;
using UKControllerPlugin::Curl::CurlResponse;
using UKControllerPlugin::Curl::CreateEscapedStringFromMap;
using UKControllerPlugin::TaskManager::TaskRunnerInterface;

namespace UKControllerPlugin {
    namespace Acars {

        AcarsMessageService::AcarsMessageService(
            CurlInterface& curl,
            TaskRunnerInterface & taskRunner
        ) 
            :curl(curl), taskRunner(taskRunner)
        {

        }

        void AcarsMessageService::Login(UKControllerPlugin::Acars::AcarsCredentials credentials)
        {
            this->isLoggedIn = false;
            if (!credentials.IsValid()) {
                LogWarning("Invalid ACARS credentials provided");
                return;
            }

            this->loginCredentials = std::move(credentials);

            this->taskRunner.QueueAsynchronousTask([this]() {
                CurlRequest request(this->acarsUrl, CurlRequest::METHOD_POST);

                request.AddHeader("Content-Type", "application/x-www-form-urlencoded");
                request.SetBody(
                    CreateEscapedStringFromMap(
                        {
                            {
                                "logon", this->loginCredentials.secret
                            },
                            {
                                "from", this->loginCredentials.callsign
                            },
                            {
                                "to", "SERVER"
                            },
                            {
                                "type", "PING"
                            },
                        }
                    )
                );

                CurlResponse response = this->curl.MakeCurlRequest(request);
                if (response.IsCurlError() || !response.StatusOk() || response.GetResponse().substr(0, 2) != "ok") {
                    LogError("Connection to ACARS failed, server returned error");
                    return;
                }

                LogInfo("Successfully logged into ACARS as " + this->loginCredentials.callsign);
                this->isLoggedIn = true;
            });
        }

        bool AcarsMessageService::IsLoggedIn(void) const
        {
            return this->isLoggedIn;
        }

    }  // namespace Acars
}  // namespace UKControllerPlugin
