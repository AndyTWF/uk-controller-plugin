#pragma once
#include "message/MessageSerializableInterface.h"
#include "airfield/AirfieldModel.h"

namespace UKControllerPlugin {
    namespace Ownership {

        /*
            A message returning which user owns a given airfield.
        */
        class AirfieldsOwnedQueryMessage : public UKControllerPlugin::Message::MessageSerializableInterface
        {
            public:
                AirfieldsOwnedQueryMessage(
                    std::vector<UKControllerPlugin::Airfield::AirfieldModel> airfields,
                    std::string userCallsign
                );
                std::string MessageHandler(void) const override;
                std::string MessageSender(void) const override;
                std::string MessageString(void) const override;
                bool MessageShowHandler(void) const override;
                bool MessageMarkUnread(void) const override;
                bool MessageOverrideBusy(void) const override;
                bool MessageFlashHandler(void) const override;
                bool MessageRequiresConfirm(void) const override;

            private:
                // The airfield involved
                const std::vector<UKControllerPlugin::Airfield::AirfieldModel> airfields;

                // The user callsign
                const std::string userCallsign;
        };
    }  // namespace Ownership
}  // namespace UKControllerPlugin
