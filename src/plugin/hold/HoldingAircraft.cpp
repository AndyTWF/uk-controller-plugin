#include "pch/pch.h"
#include "hold/HoldingAircraft.h"

namespace UKControllerPlugin {
    namespace Hold {

        HoldingAircraft::HoldingAircraft(std::string callsign, std::string assignedHold)
            : callsign(callsign), assignedHold(assignedHold), entryTime(std::chrono::system_clock::now())
        {
        }

        HoldingAircraft::HoldingAircraft(std::string callsign, std::set<std::string> proximityHolds)
            : callsign(callsign), proximityHolds(proximityHolds)
        {
        }

        void HoldingAircraft::AddProximityHold(std::string hold)
        {
            this->proximityHolds.insert(hold);
        }

        std::string HoldingAircraft::GetAssignedHold(void) const
        {
            return this->assignedHold;
        }

        const std::chrono::system_clock::time_point& HoldingAircraft::GetAssignedHoldEntryTime(void) const
        {
            return this->entryTime;
        }

        std::string HoldingAircraft::GetCallsign(void) const
        {
            return this->callsign;
        }

        const std::set<std::string> HoldingAircraft::GetProximityHolds(void) const
        {
            return this->proximityHolds;
        }

        bool HoldingAircraft::IsInAnyHold(void) const
        {
            return this->proximityHolds.size() != 0 || this->assignedHold != this->noHoldAssigned;
        }

        bool HoldingAircraft::IsInHold(std::string hold) const
        {
            return this->IsInHoldProximity(hold) || this->assignedHold == hold;
        }

        bool HoldingAircraft::IsInHoldProximity(std::string hold) const
        {
            return this->proximityHolds.find(hold) != this->proximityHolds.cend();
        }

        void HoldingAircraft::SetAssignedHold(std::string hold)
        {
            this->assignedHold = hold;
            this->entryTime = std::chrono::system_clock::now();
        }

        void HoldingAircraft::RemoveAssignedHold(void)
        {
            this->assignedHold = this->noHoldAssigned;
        }

        void HoldingAircraft::RemoveProximityHold(std::string hold)
        {
            auto proximity = this->proximityHolds.find(hold);
            if (proximity == this->proximityHolds.cend()) {
                return;
            }

            this->proximityHolds.erase(proximity);
        }
    }  // namespace Hold
}  // namespace UKControllerPlugin
