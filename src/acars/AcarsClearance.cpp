#include "pch/stdafx.h"
#include "acars/AcarsClearance.h"

namespace UKControllerPlugin {
    namespace Acars {

        const std::string AcarsClearance::NO_DATA_PROVIDED = "NODATA";

        AcarsClearance::AcarsClearance(
            std::string to,
            int messageId,
            std::string destination,
            std::string runway,
            std::string departure,
            int initialAltitude,
            std::string squawk,
            std::string ctot,
            std::string tsat,
            std::string nextFrequency
        ) 
            : to(to), messageId(messageId), destination(destination), runway(runway), departure(departure),
            initialAltitude(initialAltitude), squawk(squawk), ctot(ctot), tsat(tsat), nextFrequency(nextFrequency)
        {

        }

        bool AcarsClearance::operator=(const AcarsClearance& compare) const
        {
            return this->to == compare.to &&
                this->messageId == compare.messageId &&
                this->destination == compare.destination &&
                this->runway == compare.runway &&
                this->departure == compare.departure &&
                this->initialAltitude == compare.initialAltitude &&
                this->squawk == compare.squawk &&
                this->ctot == compare.ctot &&
                this->tsat == compare.tsat &&
                this->nextFrequency == compare.nextFrequency;
        }
    }  // namespace Acars
}  // namespace UKControllerPlugin
