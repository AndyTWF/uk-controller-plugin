#include "pch/stdafx.h"
#include "websocket/WebsocketBootstrap.h"
#include "websocket/PusherWebsocketProtocolHandler.h"
#include "websocket/WebsocketProxyConnection.h"
#include "websocket/PollingWebsocketConnection.h"
#include "websocket/WebsocketProxyHandler.h"

using UKControllerPlugin::Bootstrap::PersistenceContainer;

namespace UKControllerPlugin {
    namespace Websocket {

        std::shared_ptr<PollingWebsocketConnection> pollingConnection;

        /*
            Bootstrap up the websocket.
        */
        void BootstrapPlugin(PersistenceContainer& container, bool duplicatePlugin)
        {
            // Connect to websocket
            std::string wsHost = container.settingsRepository->HasSetting("websocket_host")
                ? container.settingsRepository->GetSetting("websocket_host")
                : "ukcp.vatsim.uk";

            std::string wsPort = container.settingsRepository->HasSetting("websocket_port")
                ? container.settingsRepository->GetSetting("websocket_port")
                : "6001";

            // Set up handler collection
            container.websocketProcessors.reset(new WebsocketEventProcessorCollection);

            // Create a websocket connection depending on whether we're the main plugin
            if (duplicatePlugin) {
                container.websocket.reset(new WebsocketProxyConnection());
            } else {
                pollingConnection = std::make_shared<PollingWebsocketConnection>(*container.api, *container.taskRunner);
                container.websocket = pollingConnection;
                container.websocketProcessors->AddProcessor(
                    std::make_shared<WebsocketProxyHandler>()
                );
                container.timedHandler->RegisterEvent(pollingConnection, 1);
            }

            container.timedHandler->RegisterEvent(
                std::make_shared<PusherWebsocketProtocolHandler>(*container.websocket, *container.websocketProcessors),
                1
            );
        }
    }  // namespace Websocket
}  // namespace UKControllerPlugin
