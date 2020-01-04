#pragma once

namespace UKControllerPlugin {
    namespace Acars {

        /*
            Represents an incoming ACARS message
        */
        struct OutgoingAcarsMessage
        {
            OutgoingAcarsMessage(std::string to, std::string type, std::string message);

            // Who the message was from
            const std::string to;

            // The type of the message
            const std::string type;

            // The message itself
            const std::string message;
        };
    }  // namespace Acars
}  // namespace UKControllerPlugin
