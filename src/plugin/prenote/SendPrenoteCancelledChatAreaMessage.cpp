#include "PrenoteMessage.h"
#include "PrenoteUserRelevanceChecker.h"
#include "SendPrenoteCancelledChatAreaMessage.h"
#include "controller/ControllerPosition.h"
#include "coordination/CoordinationChatAreaMessage.h"
#include "euroscope/GeneralSettingsEntries.h"
#include "euroscope/UserSetting.h"

namespace UKControllerPlugin::Prenote {

    SendPrenoteCancelledChatAreaMessage::SendPrenoteCancelledChatAreaMessage(
        std::shared_ptr<PrenoteUserRelevanceChecker> prenoteRelevance,
        Euroscope::UserSetting& userSettings,
        std::shared_ptr<Coordination::CoordinationChatAreaMessage> coordinationChatArea)
        : prenoteRelevance(prenoteRelevance), userSettings(userSettings), coordinationChatArea(coordinationChatArea)
    {
        assert(prenoteRelevance && "Prenote relevance is nullptr");
        assert(coordinationChatArea && "Coordination chat area is nullptr");
    }

    void SendPrenoteCancelledChatAreaMessage::MessageCancelled(const PrenoteMessage& message)
    {
        if (!UserWantsChatAreaMessages() || !prenoteRelevance->IsRelevant(message)) {
            return;
        }

        coordinationChatArea->SendMessage(
            "Prenote message for " + message.GetCallsign() + " from " + message.GetSendingController()->GetCallsign() +
            " has been cancelled.");
    }

    auto SendPrenoteCancelledChatAreaMessage::UserWantsChatAreaMessages() const -> bool
    {
        return userSettings.GetBooleanEntry(Euroscope::GeneralSettingsEntries::prenoteChatAreaMessagesSettingsKey);
    }
} // namespace UKControllerPlugin::Prenote
