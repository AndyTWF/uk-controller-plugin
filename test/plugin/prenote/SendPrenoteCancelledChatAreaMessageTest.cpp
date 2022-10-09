#include "controller/ControllerPosition.h"
#include "euroscope/UserSetting.h"
#include "prenote/PrenoteMessage.h"
#include "prenote/SendPrenoteCancelledChatAreaMessage.h"

using UKControllerPlugin::Controller::ControllerPosition;
using UKControllerPlugin::Euroscope::UserSetting;
using UKControllerPlugin::Prenote::PrenoteMessage;
using UKControllerPlugin::Prenote::SendPrenoteCancelledChatAreaMessage;

namespace UKControllerPluginTest::Prenote {
    class SendPrenoteCancelledChatAreaMessageTest : public testing::Test
    {
        public:
        SendPrenoteCancelledChatAreaMessageTest()
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
        SendPrenoteCancelledChatAreaMessage sendMessage;
    };

    TEST_F(SendPrenoteCancelledChatAreaMessageTest, ItSendsChatAreaMessageOnPrenoteMessageCancelled)
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
        EXPECT_CALL(*mockCoordination, SendMessage("Prenote message for BAW123 from EGKK_TWR has been cancelled."))
            .Times(1);
        EXPECT_CALL(*mockPrenoteRelevance, IsRelevant(testing::Ref(message))).Times(1).WillOnce(testing::Return(true));
        sendMessage.MessageCancelled(message);
    }

    TEST_F(SendPrenoteCancelledChatAreaMessageTest, ItDoesntSendMessageIfNotUserDoesntWantMessages)
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
        sendMessage.MessageCancelled(message);
    }

    TEST_F(SendPrenoteCancelledChatAreaMessageTest, ItDoesntSendMessageIfNotRelevantToController)
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
        sendMessage.MessageCancelled(message);
    }
} // namespace UKControllerPluginTest::Prenote
