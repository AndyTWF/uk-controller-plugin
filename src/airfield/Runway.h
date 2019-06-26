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

            bool operator== (const Runway & compare) const {
                return this->identifier == compare.identifier;
            }
        } Runway;

    }  // namespace Airfield
}  // namespace UKControllerPlugin
