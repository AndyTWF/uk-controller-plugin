#pragma once

namespace UKControllerPlugin {
    namespace Acars {

        /*
            Represents an incoming ACARS message
        */
        struct AcarsClearance
        {
            AcarsClearance(
                std::string to,
                int messageId,
                std::string destination,
                std::string runway,
                std::string departure,
                int initialAltitude,
                std::string squawk,
                std::string ctot = AcarsClearance::NO_DATA_PROVIDED,
                std::string tsat = AcarsClearance::NO_DATA_PROVIDED,
                std::string nextFrequency = AcarsClearance::NO_DATA_PROVIDED
            );

            bool operator=(const AcarsClearance& compare) const;

            // Who the clearance is for
            const std::string to;

            // The message type, this is always CPDLC
            const std::string type = "CPDLC";

            // The id of the message
            const int messageId;

            // The flightplan destination
            const std::string destination;

            // The runway for departure
            const std::string runway;

            // The SID in use
            const std::string departure;

            // The initial altitude
            const int initialAltitude;

            // The assigned squawk code
            const std::string squawk;

            // The CTOT
            const std::string ctot;

            // The TSAT
            const std::string tsat;

            // The next frequency
            const std::string nextFrequency;

            // Value to use if data not provided
            const static std::string NO_DATA_PROVIDED;
        };
    }  // namespace Acars
}  // namespace UKControllerPlugin
