#include "pch/stdafx.h"
#include "hold/HoldEventHandler.h"
#include "euroscope/EuroscopePluginLoopbackInterface.h"
#include "euroscope/EuroScopeCFlightPlanInterface.h"
#include "euroscope//EuroScopeCRadarTargetInterface.h"
#include "hold/HoldManager.h"
#include "plugin/PopupMenuItem.h"
#include "hold/ManagedHold.h"

using UKControllerPlugin::Euroscope::EuroScopeCFlightPlanInterface;
using UKControllerPlugin::Euroscope::EuroScopeCRadarTargetInterface;
using UKControllerPlugin::Euroscope::EuroscopePluginLoopbackInterface;
using UKControllerPlugin::TimedEvent::AbstractTimedEvent;
using UKControllerPlugin::Hold::HoldManager;
using UKControllerPlugin::Plugin::PopupMenuItem;
using UKControllerPlugin::Hold::ManagedHold;

namespace UKControllerPlugin {
    namespace Hold {

        HoldEventHandler::HoldEventHandler(
            HoldManager & holdManager,
            EuroscopePluginLoopbackInterface & plugin,
            const int popupMenuItemId
        )
            : holdManager(holdManager), plugin(plugin), popupMenuItemId(popupMenuItemId)
        {

        }

        void HoldEventHandler::FlightPlanEvent(
            EuroScopeCFlightPlanInterface & flightPlan,
            EuroScopeCRadarTargetInterface & radarTarget
        ) {
            // Nothing to do here
        }

        /*
            When a flightplan disconnects, remove that aircraft from the hold.
        */
        void HoldEventHandler::FlightPlanDisconnectEvent(EuroScopeCFlightPlanInterface & flightPlan)
        {
            this->holdManager.RemoveAircraftFromAnyHold(flightPlan.GetCallsign());
        }

        void HoldEventHandler::ControllerFlightPlanDataEvent(
            EuroScopeCFlightPlanInterface & flightPlan,
            int dataType
        ) {
            // Nothing to do here
        }

        /*
            When the timed event is called, we should update the holding aircrafts details
        */
        void HoldEventHandler::TimedEventTrigger(void)
        {
            this->holdManager.UpdateHoldingAircraft(this->plugin);
        }

        /*
            Return the description of the hold tag item
        */
        std::string HoldEventHandler::GetTagItemDescription(void) const
        {
            return "Selected Hold";
        }

        /*
            Return the value of the hold tag item
        */
        std::string HoldEventHandler::GetTagItemData(
            EuroScopeCFlightPlanInterface & flightPlan,
            EuroScopeCRadarTargetInterface & radarTarget
        ) {
            ManagedHold * const hold = this->holdManager.GetAircraftHold(flightPlan.GetCallsign());
            if (!hold) {
                return this->noHold;
            }

            return "H" + hold->GetHoldParameters().fix;
        }
    }  // namespace Hold
}  // namespace UKControllerPlugin
