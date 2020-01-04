#pragma once

namespace UKControllerPlugin {
    namespace Acars {

        /*
            Represents an incoming ACARS message
        */
        struct IncomingAcarsMessage
        {
                IncomingAcarsMessage(std::string from, std::string type, std::string message);

                // Who the message was from
                const std::string from;

                // The type of the message
                const std::string type;

                // The message itself
                const std::string message;
        };
    }  // namespace Acars
}  // namespace UKControllerPlugin
