#pragma once
#include "pch/pch.h"
#include "sectorfile/Runway.h"

namespace UKControllerPlugin {
    namespace SectorFile {

        Runway::Runway(
            std::string airfield,
            std::string identifier,
            unsigned int heading,
            bool activeForDepartures,
            bool activeForArrivals
        )
            : airfield(airfield), identifier(identifier), heading(heading),
            activeForDepartures(activeForDepartures), activeForArrivals(activeForArrivals)
        {

        }

        /*
            Returns whether or not the runway is active at all.
        */
        bool Runway::Active(void) const
        {
            return this->activeForArrivals || this->activeForDepartures;
        }

        bool Runway::ActiveForDepartures(void) const
        {
            return this->activeForDepartures;
        }

        bool Runway::ActiveForArrivals(void) const
        {
            return this->activeForArrivals;
        }

        void Runway::SetActiveForDepartures(bool active)
        {
            this->activeForDepartures = active;
        }

        void Runway::SetActiveForArrivals(bool active)
        {
            this->activeForArrivals = active;
        }

        bool Runway::operator==(const UKControllerPlugin::SectorFile::Runway & compare) const
        {
            return this->airfield + "." + this->identifier == compare.airfield + "." + compare.identifier;
            return false;
        }
    }  // namespace SectorFile
}  // namespace UKControllerPlugin
