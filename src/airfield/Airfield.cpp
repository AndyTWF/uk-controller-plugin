#include "pch/stdafx.h"
#include "airfield/Airfield.h"

namespace UKControllerPlugin {
    namespace Airfield {

        Airfield::Airfield(
            std::string icao,
            std::vector<std::string> ownership
        ) {
            if (icao.size() > 4 || icao.substr(0, 2).compare("EG") != 0) {
                throw std::invalid_argument("Invalid UK code.");
            }

            this->icao = icao;
            this->ownershipPresedence = ownership;
        }

        /*
            Add the runway to the airfield
        */
        bool Airfield::AddRunway(std::unique_ptr<Runway> runway)
        {
            Runway * runwayPtr = runway.get();
            auto runway = std::find_if(
                this->runways.cbegin(),
                this->runways.cend(),
                [runwayPtr](const std::unique_ptr<Runway> & runway) -> bool {
                    return runway->identifier == runwayPtr->identifier;
                }
            );

            if (runway) {
                return false;
            }

            return this->runways.insert(std::move(runway)).second;
        }

        std::string Airfield::GetIcao(void) const
        {
            return this->icao;
        }

        /*
            Returns the ownership presedence for the airfield.
        */
        std::vector<std::string> Airfield::GetOwnershipPresedence(void) const
        {
            return this->ownershipPresedence;
        }

        /*
            Get a given runway
        */
        Runway * const Airfield::GetRunway(std::string identifier) const
        {
            auto runway = std::find_if(
                this->runways.cbegin(),
                this->runways.cend(),
                [identifier](const std::unique_ptr<Runway> & runway) -> bool {
                    return runway->identifier == identifier;
                }
            );

            return *runway ? runway->get() : nullptr;
        }

        /*
            Returns true if the two airfields are the same.
        */
        bool Airfield::operator==(const Airfield & compare)
        {
            return this->icao == compare.icao;
        }
    }  // namespace Airfield
}  // namespace UKControllerPlugin
