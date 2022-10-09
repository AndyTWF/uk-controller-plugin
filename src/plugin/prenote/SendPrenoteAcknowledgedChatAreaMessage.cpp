#include "PrenoteMessage.h"
#include "PrenoteUserRelevanceChecker.h"
#include "SendPrenoteAcknowledgedChatAreaMessage.h"
#include "controller/ControllerPosition.h"
#include "coordination/CoordinationChatAreaMessage.h"
#include "euroscope/EuroscopePluginLoopbackInterface.h"
#include "euroscope/GeneralSettingsEntries.h"
#include "euroscope/UserSetting.h"

namespace UKControllerPlugin::Prenote {

    SendPrenoteAcknowledgedChatAreaMessage::SendPrenoteAcknowledgedChatAreaMessage(
        std::shared_ptr<PrenoteUserRelevanceChecker> prenoteRelevance,
        Euroscope::UserSetting& userSettings,
        std::shared_ptr<Coordination::CoordinationChatAreaMessage> coordinationChatArea)
        : prenoteRelevance(prenoteRelevance), userSettings(userSettings), coordinationChatArea(coordinationChatArea)
    {
        assert(prenoteRelevance && "Prenote relevance is nullptr");
        assert(coordinationChatArea && "Coordination chat area is nullptr");
    }

    void SendPrenoteAcknowledgedChatAreaMessage::MessageAcknowledged(const PrenoteMessage& message)
    {
        if (!UserWantsChatAreaMessages() || !prenoteRelevance->IsRelevant(message)) {
            return;
        }

        coordinationChatArea->SendMessage(
            "Prenote message for " + message.GetCallsign() + " has been acknowledged by " +
            message.GetTargetController()->GetCallsign() + ".");
    }

    auto SendPrenoteAcknowledgedChatAreaMessage::UserWantsChatAreaMessages() const -> bool
    {
        return userSettings.GetBooleanEntry(Euroscope::GeneralSettingsEntries::prenoteChatAreaMessagesSettingsKey);
    }
} // namespace UKControllerPlugin::Prenote
