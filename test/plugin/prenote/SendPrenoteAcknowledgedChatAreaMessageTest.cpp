#include "controller/ControllerPosition.h"
#include "euroscope/UserSetting.h"
#include "prenote/PrenoteMessage.h"
#include "prenote/SendPrenoteAcknowledgedChatAreaMessage.h"

using UKControllerPlugin::Controller::ControllerPosition;
using UKControllerPlugin::Euroscope::UserSetting;
using UKControllerPlugin::Prenote::PrenoteMessage;
using UKControllerPlugin::Prenote::SendPrenoteAcknowledgedChatAreaMessage;

namespace UKControllerPluginTest::Prenote {
    class SendPrenoteAcknowledgedChatAreaMessageTest : public testing::Test
    {
        public:
        SendPrenoteAcknowledgedChatAreaMessageTest()
            : userSettings(userSettingProvider),
              mockPrenoteRelevance(std::make_shared<testing::NiceMock<MockPrenoteUserRelevanceChecker>>()),
              mockCoordination(std::make_shared<testing::NiceMock<Coordination::MockCoordinationChatAreaMessage>>()),
              sendMessage(mockPrenoteRelevance, userSettings, mockCoordination)
        {
            sendingPosition = std::make_shared<ControllerPosition>(
                1, "EGKK_TWR", 124.225, std::vector<std::string>{"EGKK"}, true, false);
            receivingPosition = std::make_shared<ControllerPosition>(
                2, "EGKK_F_APP", 124.225, std::vector<std::string>{"EGKK"}, true, false);
        }

        testing::NiceMock<Euroscope::MockUserSettingProviderInterface> userSettingProvider;
        UserSetting userSettings;
        std::shared_ptr<ControllerPosition> sendingPosition;
        std::shared_ptr<ControllerPosition> receivingPosition;
        std::shared_ptr<testing::NiceMock<MockPrenoteUserRelevanceChecker>> mockPrenoteRelevance;
        std::shared_ptr<testing::NiceMock<Coordination::MockCoordinationChatAreaMessage>> mockCoordination;
        SendPrenoteAcknowledgedChatAreaMessage sendMessage;
    };

    TEST_F(SendPrenoteAcknowledgedChatAreaMessageTest, ItSendsChatAreaMessageOnPrenoteMessageAcknowledged)
    {
        ON_CALL(userSettingProvider, KeyExists("sendPrenotesToChat")).WillByDefault(testing::Return(true));
        ON_CALL(userSettingProvider, GetKey("sendPrenotesToChat")).WillByDefault(testing::Return("1"));
        const PrenoteMessage message(
            1,
            "BAW123",
            "EGKK",
            "TEST1A",
            "EGLL",
            sendingPosition,
            receivingPosition,
            std::chrono::system_clock::now());
        EXPECT_CALL(
            *mockCoordination,
            SendMessage(
                "Prenote message for BAW123 has been acknowledged by EGKK_F_APP."))
            .Times(1);
        EXPECT_CALL(*mockPrenoteRelevance, IsRelevant(testing::Ref(message))).Times(1).WillOnce(testing::Return(true));
        sendMessage.MessageAcknowledged(message);
    }

    TEST_F(SendPrenoteAcknowledgedChatAreaMessageTest, ItDoesntSendMessageIfNotUserDoesntWantMessages)
    {
        ON_CALL(userSettingProvider, KeyExists("sendPrenotesToChat")).WillByDefault(testing::Return(true));
        ON_CALL(userSettingProvider, GetKey("sendPrenotesToChat")).WillByDefault(testing::Return("0"));

        const PrenoteMessage message(
            1,
            "BAW123",
            "EGKK",
            "TEST1A",
            "EGLL",
            sendingPosition,
            receivingPosition,
            std::chrono::system_clock::now());
        EXPECT_CALL(*mockCoordination, SendMessage).Times(0);
        EXPECT_CALL(*mockPrenoteRelevance, IsRelevant(testing::Ref(message))).Times(0);
        sendMessage.MessageAcknowledged(message);
    }

    TEST_F(SendPrenoteAcknowledgedChatAreaMessageTest, ItDoesntSendMessageIfNotRelevantToController)
    {
        ON_CALL(userSettingProvider, KeyExists("sendPrenotesToChat")).WillByDefault(testing::Return(true));
        ON_CALL(userSettingProvider, GetKey("sendPrenotesToChat")).WillByDefault(testing::Return("1"));
        const PrenoteMessage message(
            1,
            "BAW123",
            "EGKK",
            "TEST1A",
            "EGLL",
            sendingPosition,
            receivingPosition,
            std::chrono::system_clock::now());
        EXPECT_CALL(*mockCoordination, SendMessage).Times(0);
        EXPECT_CALL(*mockPrenoteRelevance, IsRelevant(testing::Ref(message))).Times(1).WillOnce(testing::Return(false));
        sendMessage.MessageAcknowledged(message);
    }
} // namespace UKControllerPluginTest::Prenote
