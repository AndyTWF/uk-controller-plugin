#include "pch/stdafx.h"
#include "initialaltitude/InitialAltitudeEventHandler.h"
#include "euroscope/EuroScopeCFlightPlanInterface.h"
#include "euroscope/EuroScopeCRadarTargetInterface.h"
#include "ownership/AirfieldOwnershipManager.h"
#include "controller/ActiveCallsignCollection.h"
#include "login/Login.h"
#include "euroscope/EuroscopePluginLoopbackInterface.h"
#include "euroscope/GeneralSettingsEntries.h"
#include "sid/SidCollection.h"
#include "sid/StandardInstrumentDeparture.h"

using UKControllerPlugin::Euroscope::EuroScopeCFlightPlanInterface;
using UKControllerPlugin::Euroscope::EuroScopeCRadarTargetInterface;
using UKControllerPlugin::Sid::SidCollection;
using UKControllerPlugin::Sid::StandardInstrumentDeparture;
using UKControllerPlugin::Ownership::AirfieldOwnershipManager;
using UKControllerPlugin::Controller::ActiveCallsignCollection;
using UKControllerPlugin::Controller::ActiveCallsign;
using UKControllerPlugin::Airfield::NormaliseSid;
using UKControllerPlugin::Controller::Login;
using UKControllerPlugin::Euroscope::EuroscopePluginLoopbackInterface;
using UKControllerPlugin::Euroscope::GeneralSettingsEntries;

namespace UKControllerPlugin {
    namespace InitialAltitude {

        InitialAltitudeEventHandler::InitialAltitudeEventHandler(
            const SidCollection& sids,
            const ActiveCallsignCollection & activeCallsigns,
            const AirfieldOwnershipManager & airfieldOwnership,
            const Login & login,
            EuroscopePluginLoopbackInterface& plugin
        )
            : minimumLoginTimeBeforeAssignment(5), sids(sids), activeCallsigns(activeCallsigns),
              airfieldOwnership(airfieldOwnership), login(login), plugin(plugin)
        {

        }

        /*
            Handle events regarding when a flightplan changes.
        */
        void InitialAltitudeEventHandler::FlightPlanEvent(
            EuroScopeCFlightPlanInterface & flightPlan,
            EuroScopeCRadarTargetInterface & radarTarget
        ) {
            if (!this->userAutomaticAssignmentsAllowed || this->login.GetLoginStatus() != this->login.loggedIn) {
                return;
            }

            // If we've not been logged in for long, wait a bit
            if (this->login.GetSecondsLoggedIn() < this->minimumLoginTimeBeforeAssignment) {
                LogDebug(
                    "Deferring initial altitude assignment for " + flightPlan.GetCallsign() +
                    " for now, user has only recently logged in"
                );
                return;
            }

            // The aircraft has already had it's initial altitude set once, so don't double set
            // it for the same SID
            if (
                this->alreadySetMap.count(flightPlan.GetCallsign()) &&
                this->alreadySetMap.at(flightPlan.GetCallsign()) == flightPlan.GetSidName()
            ) {
                return;
            }


            // Check necessary conditions for assignment are met.
            if (!this->MeetsAssignmentConditions(flightPlan, radarTarget)) {
                return;
            }

            // Make sure the SID exists.
            std::shared_ptr<StandardInstrumentDeparture> matchedSid = this->GetSidForFlight(flightPlan);

            if (!matchedSid) {
                return;
            }

            // Doesn't assign an init altitude if cruise level is less than the initial altitude of the SID.
            if (flightPlan.GetCruiseLevel() < matchedSid->InitialAltitude()) {
                return;
            }

            // Set the IA and record it
            LogInfo(
                "Set initial altitude of " + std::to_string(matchedSid->InitialAltitude()) + " for " +
                flightPlan.GetCallsign() + " (" + flightPlan.GetOrigin() + ", " + matchedSid->Identifier() + ")"
            );
            flightPlan.SetClearedAltitude(matchedSid->InitialAltitude());
            this->alreadySetMap[flightPlan.GetCallsign()] = flightPlan.GetSidName();
        }

        /*
            Handle events regarding when a flightplan disconnects.
        */
        void InitialAltitudeEventHandler::FlightPlanDisconnectEvent(EuroScopeCFlightPlanInterface & flightPlan)
        {
            this->alreadySetMap.erase(flightPlan.GetCallsign());
        }

        /*
            Handle events regarding controller data being changed (directs etc).
        */
        void InitialAltitudeEventHandler::ControllerFlightPlanDataEvent(
            EuroScopeCFlightPlanInterface & flightPlan,
            int dataType
        ) {
            // Nothing to do here
        }

        /*
            Reset the initial altitude for the given callsign.
        */
        void InitialAltitudeEventHandler::RecycleInitialAltitude(
            EuroScopeCFlightPlanInterface & flightplan,
            EuroScopeCRadarTargetInterface & radarTarget,
            std::string context,
            const POINT & mousePos
        ) {
            if (!MeetsForceAssignmentConditions(flightplan, radarTarget))
            {
                return;
            }

            // Make sure the SID exists.
            std::shared_ptr<StandardInstrumentDeparture> matchedSid = this->GetSidForFlight(flightplan);

            if (!matchedSid) {
                return;
            }

            flightplan.SetClearedAltitude(matchedSid->InitialAltitude());
            LogInfo(
                "Recycled initial altitude of " + std::to_string(matchedSid->InitialAltitude()) + " for " +
                flightplan.GetCallsign() + " (" + flightplan.GetOrigin() + ", " + matchedSid->Identifier() + ")"
            );
            this->alreadySetMap[flightplan.GetCallsign()] = flightplan.GetSidName();
        }

        /*
            Returns whether or not the user has allowed automatic assignments.
        */
        bool InitialAltitudeEventHandler::UserAutomaticAssignmentsAllowed(void) const
        {
            return this->userAutomaticAssignmentsAllowed;
        }

        /*
            Returns true if the aircraft meets the prerequisites for initial altitude assignment.

            To return true, must:

            1. Must not have a current flight level or distance from origin of exactly zero.
               This is required because for some reason EuroScope picks up flightplan changes over the likes of
               Austria and decides its time to do an initial altitude assignment.
            2. Be on the ground
            3. Be within a specified distance of the origin airport.
            4. Be under a certain speed.
            5. Not have a cleared altitude set.
            6. Not be tracked by any controller.
            7. Not be "simulated" by ES.
            8. Have a user that has a recognised callsign.
            9. Have the airfield of origin owned by the controller.
        */
        bool InitialAltitudeEventHandler::MeetsAssignmentConditions(
            EuroScopeCFlightPlanInterface & flightPlan,
            EuroScopeCRadarTargetInterface & radarTarget
        ) const {
            return radarTarget.GetFlightLevel() != 0 &&
                flightPlan.GetDistanceFromOrigin() != 0.0 &&
                radarTarget.GetFlightLevel() <= this->assignmentMaxAltitude &&
                flightPlan.GetDistanceFromOrigin() <= this->assignmentMaxDistanceFromOrigin &&
                radarTarget.GetGroundSpeed() <= this->assignmentMaxSpeed &&
                !flightPlan.HasControllerClearedAltitude() &&
                !flightPlan.IsTracked() &&
                !flightPlan.IsSimulated() &&
                this->airfieldOwnership.AirfieldOwnedByUser(flightPlan.GetOrigin());
        }

        /*
         * Force assignments are allowed if the aircraft is:
         *
         * 1. Not tracked
         * 2. Tracked by the current user
         */
        bool InitialAltitudeEventHandler::MeetsForceAssignmentConditions(
            EuroScopeCFlightPlanInterface& flightplan,
            EuroScopeCRadarTargetInterface& radarTarget
        ) {
            return !flightplan.IsTracked() || flightplan.IsTrackedByUser();
        }

        std::shared_ptr<StandardInstrumentDeparture> InitialAltitudeEventHandler::GetSidForFlight(
            EuroScopeCFlightPlanInterface& flightplan)
        {
            return this->sids.GetByAirfieldAndIdentifier(
                flightplan.GetOrigin(),
                normalise.StripSidDeprecation(flightplan.GetSidName())
            );
        }

        /*
            If its the user, do some updates
        */
        void InitialAltitudeEventHandler::ActiveCallsignAdded(const ActiveCallsign& callsign, bool userCallsign)
        {
            if (!userCallsign) {
                return;
            }


            LogInfo("User now active, mass assigning initial altitudes");
            this->CheckAllFlightplansForAssignment();
        }

        /*
            Nothing to see here
        */
        void InitialAltitudeEventHandler::ActiveCallsignRemoved(const ActiveCallsign& callsign, bool userCallsign)
        {

        }

        /*
            Nothing to see here
        */
        void InitialAltitudeEventHandler::CallsignsFlushed(void)
        {

        }

        /*
         * Periodically, check all flightplans to see if they need an initial altitude update.
         */
        void InitialAltitudeEventHandler::TimedEventTrigger()
        {
            if (!this->activeCallsigns.UserHasCallsign()) {
                return;
            }

            this->CheckAllFlightplansForAssignment();
        }

        /*
         * Loop all the flightplans and check if they need an assignment.
         */
        void InitialAltitudeEventHandler::CheckAllFlightplansForAssignment()
        {
            this->plugin.ApplyFunctionToAllFlightplans(
                [this]
            (
                std::shared_ptr<EuroScopeCFlightPlanInterface> fp,
                std::shared_ptr<EuroScopeCRadarTargetInterface> rt
            )
                {
                    this->FlightPlanEvent(*fp, *rt);
                }
            );
        }

        /*
            Called when user settings get updated.
        */
        void InitialAltitudeEventHandler::UserSettingsUpdated(Euroscope::UserSetting& userSettings)
        {
            this->userAutomaticAssignmentsAllowed = userSettings.GetBooleanEntry(
                GeneralSettingsEntries::initialAltitudeToggleSettingsKey,
                true
            );
        }
    }  // namespace InitialAltitude
}  // namespace UKControllerPlugin
