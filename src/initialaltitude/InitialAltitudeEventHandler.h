#pragma once
#include "flightplan/FlightPlanEventHandlerInterface.h"
#include "airfield/NormaliseSid.h"
#include "timedevent/DeferredEventHandler.h"
#include "euroscope/UserSettingAwareInterface.h"
#include "controller/ActiveCallsignEventHandlerInterface.h"
#include "flightplan/StoredFlightplanCollection.h"

// Forward declarations

namespace UKControllerPlugin {
    namespace Flightplan {
        class FlightPlanEventHandlerInterface;
    }  // namespace Flightplan
    namespace Ownership {
        class AirfieldOwnershipManager;
    }  // namespace Ownership

    namespace Controller {
        class ActiveCallsignCollection;
        class Login;
    }  // namespace Controller
    namespace Sid {
        class SidCollection;
        class StandardInstrumentDeparture;
    } // namespace Sid
    namespace TimedEvent {
        class DeferredEventHandler;
    }  // namespace TimedEvent
    namespace Euroscope {
        class EuroscopePluginLoopbackInterface;
    }  // namespace Euroscope
}  // namespace UKControllerPlugin

namespace UKControllerPlugin {
    namespace InitialAltitude {

        /*
            Class that responds to events related to initial altitudes.
        */
        class InitialAltitudeEventHandler : public UKControllerPlugin::Flightplan::FlightPlanEventHandlerInterface,
            public UKControllerPlugin::Euroscope::UserSettingAwareInterface,
            public UKControllerPlugin::Controller::ActiveCallsignEventHandlerInterface
        {
            public:
                InitialAltitudeEventHandler(
                    const Sid::SidCollection& sids,
                    const UKControllerPlugin::Controller::ActiveCallsignCollection & activeCallsigns,
                    const UKControllerPlugin::Ownership::AirfieldOwnershipManager & airfieldOwnership,
                    const UKControllerPlugin::Controller::Login & login,
                    UKControllerPlugin::TimedEvent::DeferredEventHandler & deferredEvents,
                    UKControllerPlugin::Euroscope::EuroscopePluginLoopbackInterface & plugin,
                    const UKControllerPlugin::Flightplan::StoredFlightplanCollection& storedFlightplans
                );
                void FlightPlanEvent(
                    UKControllerPlugin::Euroscope::EuroScopeCFlightPlanInterface & flightPlan,
                    UKControllerPlugin::Euroscope::EuroScopeCRadarTargetInterface & radarTarget
                ) override;
                ~InitialAltitudeEventHandler(void);
                void FlightPlanDisconnectEvent(
                    UKControllerPlugin::Euroscope::EuroScopeCFlightPlanInterface & flightPlan
                ) override;
                void ControllerFlightPlanDataEvent(
                    UKControllerPlugin::Euroscope::EuroScopeCFlightPlanInterface & flightPlan,
                    int dataType
                ) override;
                void RecycleInitialAltitude(
                    UKControllerPlugin::Euroscope::EuroScopeCFlightPlanInterface & flightPlan,
                    UKControllerPlugin::Euroscope::EuroScopeCRadarTargetInterface & radarTarget,
                    std::string context,
                    const POINT & mousePos
                );
                bool UserAutomaticAssignmentsAllowed(void) const;
                void UserSettingsUpdated(UKControllerPlugin::Euroscope::UserSetting & userSettings) override;

                // Inherited via ActiveCallsignEventHandlerInterface
                void ActiveCallsignAdded(
                    const UKControllerPlugin::Controller::ActiveCallsign& callsign,
                    bool userCallsign
                ) override;
                void ActiveCallsignRemoved(
                    const UKControllerPlugin::Controller::ActiveCallsign& callsign,
                    bool userCallsign
                ) override;
                void CallsignsFlushed(void) override;

                // The maximum distance from the airfield that an aircraft can be untracked
                // to be considered for an altitude update.
                const double assignmentMaxDistanceFromOrigin = 3.0;

                // Then maximum speed that for assigning initial altitudes.
                const int assignmentMaxSpeed = 40;

                // Then current altitude assigning initial altitudes.
                const int assignmentMaxAltitude = 1000;

                // How long we should wait after logging in before assigning
                const std::chrono::seconds minimumLoginTimeBeforeAssignment;

            private:
                bool MeetsAssignmentConditions(
                    Euroscope::EuroScopeCFlightPlanInterface& flightPlan,
                    Euroscope::EuroScopeCRadarTargetInterface& radarTarget
                ) const;

                static bool MeetsForceAssignmentConditions(
                    UKControllerPlugin::Euroscope::EuroScopeCFlightPlanInterface & flightplan,
                    UKControllerPlugin::Euroscope::EuroScopeCRadarTargetInterface & radarTarget
                );

                std::shared_ptr<Sid::StandardInstrumentDeparture> GetSidForFlight(
                    Euroscope::EuroScopeCFlightPlanInterface& flightplan
                );

                // Used to generate initial altitudes.
                const Sid::SidCollection& sids;

                // Used to find out the users callsign.
                const UKControllerPlugin::Controller::ActiveCallsignCollection & activeCallsigns;

                // Used to find out if the user owns a particular airfield.
                const UKControllerPlugin::Ownership::AirfieldOwnershipManager & airfieldOwnership;

                // So we can defer loading IAs on first login
                UKControllerPlugin::TimedEvent::DeferredEventHandler & deferredEvents;

                // For checking how long we've been logged in
                const UKControllerPlugin::Controller::Login & login;

                // Class for parsing SIDs and removing deprecation warnings.
                const UKControllerPlugin::Airfield::NormaliseSid normalise;

                // Stored flightplans
                const UKControllerPlugin::Flightplan::StoredFlightplanCollection& storedFlightplans;

                // So we can get flightplans after deferred events
                UKControllerPlugin::Euroscope::EuroscopePluginLoopbackInterface & plugin;

                // Has the user enabled automatic assignments
                bool userAutomaticAssignmentsAllowed = true;

                // Map of callsigns vs which SID their altitude has been set for
                std::map<std::string, std::string> alreadySetMap;
        };
    }  // namespace InitialAltitude
}  // namespace UKControllerPlugin
