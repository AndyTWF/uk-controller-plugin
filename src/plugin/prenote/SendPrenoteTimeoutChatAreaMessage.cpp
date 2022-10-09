#include "PrenoteMessage.h"
#include "PrenoteUserRelevanceChecker.h"
#include "SendPrenoteTimeoutChatAreaMessage.h"
#include "controller/ControllerPosition.h"
#include "coordination/CoordinationChatAreaMessage.h"
#include "euroscope/GeneralSettingsEntries.h"
#include "euroscope/UserSetting.h"

namespace UKControllerPlugin::Prenote {

    SendPrenoteTimeoutChatAreaMessage::SendPrenoteTimeoutChatAreaMessage(
        std::shared_ptr<PrenoteUserRelevanceChecker> prenoteRelevance,
        Euroscope::UserSetting& userSettings,
        std::shared_ptr<Coordination::CoordinationChatAreaMessage> coordinationChatArea)
        : prenoteRelevance(prenoteRelevance), userSettings(userSettings), coordinationChatArea(coordinationChatArea)
    {
        assert(prenoteRelevance && "Prenote relevance is nullptr");
        assert(coordinationChatArea && "Coordination chat area is nullptr");
    }

    void SendPrenoteTimeoutChatAreaMessage::MessageTimeout(const PrenoteMessage& message)
    {
        if (message.IsAcknowledged() || !UserWantsChatAreaMessages() || !prenoteRelevance->IsRelevant(message)) {
            return;
        }

        coordinationChatArea->SendMessage(
            "Prenote message to " + message.GetTargetController()->GetCallsign() + " for " + message.GetCallsign() +
            " has timed out without acknowledgement.");
    }

    auto SendPrenoteTimeoutChatAreaMessage::UserWantsChatAreaMessages() const -> bool
    {
        return userSettings.GetBooleanEntry(Euroscope::GeneralSettingsEntries::prenoteChatAreaMessagesSettingsKey);
    }
} // namespace UKControllerPlugin::Prenote
