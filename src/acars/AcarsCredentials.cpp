#pragma once
#include "pch/stdafx.h"
#include "acars/AcarsCredentials.h"

namespace UKControllerPlugin {
    namespace Acars {

        const std::string AcarsCredentials::NO_DATA_PROVIDED = "";


        AcarsCredentials::AcarsCredentials(std::string callsign, std::string secret)
            : callsign(callsign), secret(secret)
        {
        }

        bool AcarsCredentials::IsValid() const
        {
            return this->callsign != AcarsCredentials::NO_DATA_PROVIDED &&
                this->secret != AcarsCredentials::NO_DATA_PROVIDED;
        }

    }  // namespace Acars
}  // namespace UKControllerPlugin
