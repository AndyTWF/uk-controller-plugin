#pragma once
#include "pch/pch.h"
#include "initialaltitude/InitialAltitudeEventHandler.h"
#include "initialaltitude/InitialAltitudeGenerator.h"
#include "controller/ActiveCallsignCollection.h"
#include "ownership/AirfieldOwnershipManager.h"
#include "euroscope/EuroScopeCFlightPlanInterface.h"
#include "euroscope/EuroScopeCRadarTargetInterface.h"
#include "euroscope/EuroscopePluginLoopbackInterface.h"
#include "login/Login.h"
#include "timedevent/DeferredEventRunnerInterface.h"
#include "flightplan/StoredFlightplanCollection.h"

namespace UKControllerPluginTest {
    namespace EventHandler {

        class MockInitialAltitudeEventHandler : public UKControllerPlugin::InitialAltitude::InitialAltitudeEventHandler
        {
            public:
                MockInitialAltitudeEventHandler() : InitialAltitudeEventHandler(
                    *generator,
                    *activeCallsigns,
                    *airfieldOwnership,
                    *login,
                    *deferredEvents,
                    *plugin,
                    *storedFlightplans
                ) {}
                MOCK_METHOD2(
                    FlightPlanEvent,
                    void(
                        UKControllerPlugin::Euroscope::EuroScopeCFlightPlanInterface & flightPlan,
                        UKControllerPlugin::Euroscope::EuroScopeCRadarTargetInterface & target)
                    );
                MOCK_METHOD1(
                    FlightPlanDisconnectEvent,
                    void(UKControllerPlugin::Euroscope::EuroScopeCFlightPlanInterface & flightPlan)
                );
                MOCK_METHOD1(
                    ControllerFlightPlanDataEvent,
                    void(UKControllerPlugin::Euroscope::EuroScopeCFlightPlanInterface & flightPlan)
                );

            private:

                // Some dummy things so we can build the mock.
                std::unique_ptr<UKControllerPlugin::InitialAltitude::InitialAltitudeGenerator> generator;
                std::unique_ptr<UKControllerPlugin::Controller::ActiveCallsignCollection> activeCallsigns;
                std::unique_ptr<UKControllerPlugin::Ownership::AirfieldOwnershipManager> airfieldOwnership;
                std::unique_ptr<UKControllerPlugin::Controller::Login> login;
                std::unique_ptr<UKControllerPlugin::TimedEvent::DeferredEventHandler> deferredEvents;
                std::unique_ptr<UKControllerPlugin::Euroscope::EuroscopePluginLoopbackInterface> plugin;
                std::unique_ptr<UKControllerPlugin::Flightplan::StoredFlightplanCollection> storedFlightplans;
        };
    }  // namespace EventHandler
}  // namespace UKControllerPluginTest
