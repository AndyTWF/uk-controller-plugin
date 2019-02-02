#pragma once
#include "pch/stdafx.h"

namespace UKControllerPlugin {
    namespace Hold {

        /*
            Data about a holding aircraft
        */
        typedef struct HoldingAircraft
        {
            const std::string callsign;
            const std::string holdIdentifier;
            mutable int reportedAltitude;
            mutable int clearedAltitude;
            const std::chrono::system_clock::time_point entryTime;
        } HoldingAircraft;

    }  // namespace Hold
}  // namespace UKControllerPlugin