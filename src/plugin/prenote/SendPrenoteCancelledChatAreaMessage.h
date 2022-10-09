#pragma once
#include "PrenoteMessageEventHandlerInterface.h"

namespace UKControllerPlugin {
    namespace Coordination {
        class CoordinationChatAreaMessage;
    } // namespace Coordination
    namespace Euroscope {
        class UserSetting;
    } // namespace Euroscope
} // namespace UKControllerPlugin

namespace UKControllerPlugin::Prenote {
    class PrenoteUserRelevanceChecker;

    class SendPrenoteCancelledChatAreaMessage : public PrenoteMessageEventHandlerInterface
    {
        public:
        SendPrenoteCancelledChatAreaMessage(
            std::shared_ptr<PrenoteUserRelevanceChecker> prenoteRelevance,
            Euroscope::UserSetting& userSettings,
            std::shared_ptr<Coordination::CoordinationChatAreaMessage> coordinationChatArea);
        void MessageCancelled(const PrenoteMessage& message) override;

        private:
        [[nodiscard]] auto UserWantsChatAreaMessages() const -> bool;
        // Checks relevance of prenotes
        const std::shared_ptr<PrenoteUserRelevanceChecker> prenoteRelevance;

        // User settings
        Euroscope::UserSetting& userSettings;

        // For sending messages to the chat area
        std::shared_ptr<Coordination::CoordinationChatAreaMessage> coordinationChatArea;
    };
} // namespace UKControllerPlugin::Prenote
