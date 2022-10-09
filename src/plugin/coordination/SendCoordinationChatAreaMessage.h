#pragma once
#include "CoordinationChatAreaMessage.h"

namespace UKControllerPlugin::Euroscope {
    class EuroscopePluginLoopbackInterface;
} // namespace UKControllerPlugin::Euroscope

namespace UKControllerPlugin::Coordination {
    class SendCoordinationChatAreaMessage : public CoordinationChatAreaMessage
    {
        public:
        SendCoordinationChatAreaMessage(Euroscope::EuroscopePluginLoopbackInterface& plugin);
        void SendMessage(const std::string& message) override;

        private:
        // The plugin has a method for sending chat area messages
        Euroscope::EuroscopePluginLoopbackInterface& plugin;
    };
} // namespace UKControllerPlugin::Coordination
