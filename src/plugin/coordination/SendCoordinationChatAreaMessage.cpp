#include "SendCoordinationChatAreaMessage.h"
#include "euroscope/EuroscopePluginLoopbackInterface.h"

namespace UKControllerPlugin::Coordination {

    SendCoordinationChatAreaMessage::SendCoordinationChatAreaMessage(
        Euroscope::EuroscopePluginLoopbackInterface& plugin)
        : plugin(plugin)
    {
    }

    void SendCoordinationChatAreaMessage::SendMessage(const std::string& message)
    {
        plugin.ChatAreaMessage("UKCP_COORDINATION", "UKCP", message, true, true, true, true, true);
    }
} // namespace UKControllerPlugin::Coordination
