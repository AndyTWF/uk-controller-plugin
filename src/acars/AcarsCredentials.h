#pragma once
#include "pch/stdafx.h"

namespace UKControllerPlugin {
    namespace Acars {

        struct AcarsCredentials
        {
            public:

                AcarsCredentials(std::string callsign = NO_DATA_PROVIDED, std::string secret = NO_DATA_PROVIDED);
                bool IsValid() const;

                // The callsign to log into ACARS with
                std::string callsign;

                // The per-user secret to log into ACARS with
                std::string secret;

                // Default value
                static const std::string NO_DATA_PROVIDED;

        };
    }  // namespace Acars
}  // namespace UKControllerPlugin
