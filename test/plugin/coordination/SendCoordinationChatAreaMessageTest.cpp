#include "coordination/SendCoordinationChatAreaMessage.h"

using UKControllerPlugin::Coordination::SendCoordinationChatAreaMessage;

namespace UKControllerPluginTest::Coordination {
    class SendCoordinationChatAreaMessageTest : public testing::Test
    {
        public:
        SendCoordinationChatAreaMessageTest() : sendMessage(plugin)
        {
        }

        testing::NiceMock<Euroscope::MockEuroscopePluginLoopbackInterface> plugin;
        SendCoordinationChatAreaMessage sendMessage;
    };

    TEST_F(SendCoordinationChatAreaMessageTest, ItSendsChatAreaMessage)
    {
        EXPECT_CALL(
            plugin,
            ChatAreaMessage("UKCP_COORDINATION", "UKCP", "This is a test message.", true, true, true, true, true))
            .Times(1);
        sendMessage.SendMessage("This is a test message.");
    }
} // namespace UKControllerPluginTest::Coordination
