#pragma once
#include "pch/stdafx.h"

namespace UKControllerPlugin {
    namespace Airfield {

        /*
            Repesents a given runway.
        */
        typedef struct Runway
        {
            const std::string identifier;
            const unsigned int heading;
            bool activeDeparture;
            bool activeArrival;
        } Runway;

    }  // namespace Airfield
}  // namespace UKControllerPlugin
