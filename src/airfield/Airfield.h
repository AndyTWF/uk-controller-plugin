#pragma once
#include "airfield/Runway.h"

namespace UKControllerPlugin {
    namespace Airfield {

        /*
            A class representing a UK airfield. Contains details such as the top-down
            order, as well as who currently owns it.
        */
        class Airfield
        {
            public:
                Airfield(
                    std::string icao,
                    std::vector<std::string> ownership
                );

                bool AddRunway(std::unique_ptr<Runway> runway);
                std::string GetIcao(void) const;
                std::vector<std::string> GetOwnershipPresedence(void) const;
                UKControllerPlugin::Airfield::Runway * const GetRunway(std::string identifier) const;
                bool operator==(const Airfield & compare);

            private:
                // The airfield ICAO
                std::string icao;

                // The order of who owns this airfield.
                std::vector<std::string> ownershipPresedence;

                // The runways at the airfield
                std::set<std::unique_ptr<UKControllerPlugin::Airfield::Runway>> runways;
        };
    }  // namespace Airfield
}  // namespace UKControllerPlugin
