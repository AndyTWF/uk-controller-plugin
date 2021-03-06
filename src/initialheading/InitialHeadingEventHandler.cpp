#include "pch/stdafx.h"
#include "initialheading/InitialHeadingEventHandler.h"
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
    namespace InitialHeading {

        InitialHeadingEventHandler::InitialHeadingEventHandler(
            const SidCollection& sids,
            const ActiveCallsignCollection& activeCallsigns,
            const AirfieldOwnershipManager& airfieldOwnership,
            const Login& login,
            EuroscopePluginLoopbackInterface& plugin
        )
            : minimumLoginTimeBeforeAssignment(5), sids(sids), activeCallsigns(activeCallsigns),
              airfieldOwnership(airfieldOwnership), login(login), plugin(plugin)
        { }

        /*
            Handle events regarding when a flightplan changes.
        */
        void InitialHeadingEventHandler::FlightPlanEvent(
            EuroScopeCFlightPlanInterface& flightPlan,
            EuroScopeCRadarTargetInterface& radarTarget
        )
        {
            if (!this->userAutomaticAssignmentsAllowed || this->login.GetLoginStatus() != this->login.loggedIn) {
                return;
            }

            // If we've not been logged in for long, wait a bit
            if (this->login.GetSecondsLoggedIn() < this->minimumLoginTimeBeforeAssignment) {
                LogDebug(
                    "Deferring initial heading assignment for " + flightPlan.GetCallsign()
                    + " for now, as user only just logged in"
                );
                return;
            }

            // The aircraft has already had it's initial heading set once, so don't double set
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


            if (!matchedSid || matchedSid->InitialHeading() == 0) {
                return;
            }

            // Set the heading and record it
            LogInfo(
                "Set initial heading of " + std::to_string(matchedSid->InitialHeading()) + " for " +
                flightPlan.GetCallsign() + " (" + flightPlan.GetOrigin() + ", " + matchedSid->Identifier() + ")"
            );
            flightPlan.SetHeading(matchedSid->InitialHeading());
            this->alreadySetMap[flightPlan.GetCallsign()] = flightPlan.GetSidName();
        }

        /*
            Handle events regarding when a flightplan disconnects.
        */
        void InitialHeadingEventHandler::FlightPlanDisconnectEvent(EuroScopeCFlightPlanInterface& flightPlan)
        {
            this->alreadySetMap.erase(flightPlan.GetCallsign());
        }

        /*
            Handle events regarding controller data being changed (directs etc).
        */
        void InitialHeadingEventHandler::ControllerFlightPlanDataEvent(
            EuroScopeCFlightPlanInterface& flightPlan,
            int dataType
        )
        {
            // Nothing to do here
        }

        /*
            Reset the initial heading for the given callsign.
        */
        void InitialHeadingEventHandler::RecycleInitialHeading(
            EuroScopeCFlightPlanInterface& flightplan,
            EuroScopeCRadarTargetInterface& radarTarget,
            std::string context,
            const POINT& mousePos
        )
        {
            if (!MeetsForceAssignmentConditions(flightplan, radarTarget)) {
                return;
            }

            // Make sure the SID exists.
            std::shared_ptr<StandardInstrumentDeparture> matchedSid = this->GetSidForFlight(flightplan);

            if (!matchedSid || matchedSid->InitialHeading() == 0) {
                return;
            }

            flightplan.SetHeading(matchedSid->InitialHeading());
            LogInfo(
                "Recycled initial heading of " + std::to_string(matchedSid->InitialHeading()) + " for " +
                flightplan.GetCallsign() + " (" + flightplan.GetOrigin() + ", " + matchedSid->Identifier() + ")"
            );
            this->alreadySetMap[flightplan.GetCallsign()] = flightplan.GetSidName();
        }

        /*
            Returns whether or not the user has allowed automatic assignments.
        */
        bool InitialHeadingEventHandler::UserAutomaticAssignmentsAllowed(void) const
        {
            return this->userAutomaticAssignmentsAllowed;
        }

        void InitialHeadingEventHandler::CheckAllFlightplansForAssignment()
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
            Returns true if the aircraft meets the prerequisites for initial heading assignment.

            To return true, must:

            1. Must not have a current flight level or distance from origin of exactly zero.
               This is required because for some reason EuroScope picks up flightplan changes over the likes of
               Austria and decides its time to do an initial heading assignment.
            2. Be on the ground
            3. Be within a specified distance of the origin airport.
            4. Be under a certain speed.
            5. Not have an assigned heading set.
            6. Not be tracked by any controller.
            7. Not be "simulated" by ES.
            8. Have a user that has a recognised callsign.
            9. Have the airfield of origin owned by the controller.
        */
        bool InitialHeadingEventHandler::MeetsAssignmentConditions(
            EuroScopeCFlightPlanInterface& flightPlan,
            EuroScopeCRadarTargetInterface& radarTarget
        ) const
        {
            return radarTarget.GetFlightLevel() != 0 &&
                flightPlan.GetDistanceFromOrigin() != 0.0 &&
                radarTarget.GetFlightLevel() <= this->assignmentMaxAltitude &&
                flightPlan.GetDistanceFromOrigin() <= this->assignmentMaxDistanceFromOrigin &&
                radarTarget.GetGroundSpeed() <= this->assignmentMaxSpeed &&
                !flightPlan.HasControllerAssignedHeading() &&
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
        bool InitialHeadingEventHandler::MeetsForceAssignmentConditions(
            EuroScopeCFlightPlanInterface& flightplan,
            EuroScopeCRadarTargetInterface& radarTarget
        )
        {
            return !flightplan.IsTracked() || flightplan.IsTrackedByUser();
        }

        std::shared_ptr<StandardInstrumentDeparture> InitialHeadingEventHandler::GetSidForFlight(
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
        void InitialHeadingEventHandler::ActiveCallsignAdded(const ActiveCallsign& callsign, bool userCallsign)
        {
            if (!userCallsign) {
                return;
            }

            LogInfo("User now active, mass assigning initial headings");
            this->CheckAllFlightplansForAssignment();
        }

        /*
            Nothing to see here
        */
        void InitialHeadingEventHandler::ActiveCallsignRemoved(const ActiveCallsign& callsign, bool userCallsign)
        { }

        /*
            Nothing to see here
        */
        void InitialHeadingEventHandler::CallsignsFlushed(void)
        { }

        void InitialHeadingEventHandler::TimedEventTrigger()
        {
            if (!this->activeCallsigns.UserHasCallsign()) {
                return;
            }

            this->CheckAllFlightplansForAssignment();
        }

        /*
            Called when user settings get updated.
        */
        void InitialHeadingEventHandler::UserSettingsUpdated(
            Euroscope::UserSetting& userSettings
        )
        {
            this->userAutomaticAssignmentsAllowed = userSettings.GetBooleanEntry(
                GeneralSettingsEntries::initialHeadingToggleSettingsKey,
                true
            );
        }
    } // namespace InitialHeading
} // namespace UKControllerPlugin
