#include "pch/pch.h"
#include "initialaltitude/InitialAltitudeEventHandler.h"
#include "mock/MockEuroScopeCFlightplanInterface.h"
#include "mock/MockEuroScopeCRadarTargetInterface.h"
#include "ownership/AirfieldOwnershipManager.h"
#include "controller/ActiveCallsignCollection.h"
#include "airfield/AirfieldCollection.h"
#include "controller/ActiveCallsign.h"
#include "controller/ControllerPosition.h"
#include "airfield/AirfieldModel.h"
#include "login/Login.h"
#include "mock/MockEuroscopePluginLoopbackInterface.h"
#include "controller/ControllerStatusEventHandlerCollection.h"
#include "mock/MockUserSettingProviderInterface.h"
#include "euroscope/UserSetting.h"
#include "euroscope/GeneralSettingsEntries.h"
#include "sid/SidCollection.h"
#include "sid/StandardInstrumentDeparture.h"

using UKControllerPlugin::InitialAltitude::InitialAltitudeEventHandler;
using UKControllerPlugin::Sid::SidCollection;
using UKControllerPlugin::Sid::StandardInstrumentDeparture;
using UKControllerPluginTest::Euroscope::MockEuroScopeCFlightPlanInterface;
using UKControllerPluginTest::Euroscope::MockEuroScopeCRadarTargetInterface;
using UKControllerPlugin::Ownership::AirfieldOwnershipManager;
using UKControllerPlugin::Controller::ActiveCallsignCollection;
using UKControllerPlugin::Controller::ActiveCallsign;
using UKControllerPlugin::Airfield::AirfieldCollection;
using UKControllerPlugin::Controller::ControllerPosition;
using UKControllerPlugin::Airfield::AirfieldModel;
using UKControllerPluginTest::Euroscope::MockEuroscopePluginLoopbackInterface;
using UKControllerPlugin::Controller::Login;
using UKControllerPlugin::Controller::ControllerStatusEventHandlerCollection;
using UKControllerPlugin::Euroscope::UserSetting;
using UKControllerPluginTest::Euroscope::MockUserSettingProviderInterface;
using UKControllerPlugin::Euroscope::GeneralSettingsEntries;

using ::testing::Test;
using ::testing::StrictMock;
using ::testing::Return;
using ::testing::NiceMock;
using ::testing::Throw;
using ::testing::_;

namespace UKControllerPluginTest {
    namespace InitialAltitude {

        class InitialAltitudeEventHandlerTest : public Test {
            public:
                InitialAltitudeEventHandlerTest()
                    :  owners(airfields, callsigns),
                    login(plugin, ControllerStatusEventHandlerCollection()),
                    handler(sids, callsigns, owners, login, plugin),
                    controller(1, "LON_S_CTR", 129.420, {"EGKK"}, true, false),
                    userCallsign("LON_S_CTR", "Test", controller)
                {

                }

                virtual void SetUp() {
                    // Pretend we've been logged in a while
                    login.SetLoginStatus(EuroScopePlugIn::CONNECTION_TYPE_DIRECT);
                    login.SetLoginTime(std::chrono::system_clock::now() - std::chrono::minutes(15));
                    sids.AddSid(std::make_shared<StandardInstrumentDeparture>("EGKK", "ADMAG2X", 6000, 0));
                    sids.AddSid(std::make_shared<StandardInstrumentDeparture>("EGKK", "CLN3X", 5000, 0));

                    this->mockFlightplanPointer.reset(new NiceMock<MockEuroScopeCFlightPlanInterface>);
                    this->mockRadarTargetPointer.reset(new NiceMock<MockEuroScopeCRadarTargetInterface>);
                    ON_CALL(mockFlightPlan, GetCallsign())
                        .WillByDefault(Return("BAW123"));

                    ON_CALL(*mockFlightplanPointer, GetCallsign())
                        .WillByDefault(Return("BAW123"));
                }

                ControllerPosition controller;
                ActiveCallsign userCallsign;
                AirfieldCollection airfields;
                std::shared_ptr<NiceMock<MockEuroScopeCFlightPlanInterface>> mockFlightplanPointer;
                std::shared_ptr<NiceMock<MockEuroScopeCRadarTargetInterface>> mockRadarTargetPointer;
                NiceMock<MockEuroScopeCFlightPlanInterface> mockFlightPlan;
                NiceMock<MockEuroScopeCRadarTargetInterface> mockRadarTarget;
                NiceMock<MockEuroscopePluginLoopbackInterface> plugin;
                Login login;
                ActiveCallsignCollection callsigns;
                AirfieldOwnershipManager owners;
                SidCollection sids;
                InitialAltitudeEventHandler handler;
        };

        TEST_F(InitialAltitudeEventHandlerTest, TestItDefaultsUserSettingToEnabled)
        {
            EXPECT_TRUE(this->handler.UserAutomaticAssignmentsAllowed());
        }

        TEST_F(InitialAltitudeEventHandlerTest, TestItSetsEnabledFromUserSettings)
        {
            NiceMock<MockUserSettingProviderInterface> mockUserSettings;
            UserSetting userSetting(mockUserSettings);

            ON_CALL(mockUserSettings, GetKey(GeneralSettingsEntries::initialAltitudeToggleSettingsKey))
                .WillByDefault(Return("0"));

            this->handler.UserSettingsUpdated(userSetting);
            EXPECT_FALSE(this->handler.UserAutomaticAssignmentsAllowed());
        }

        TEST_F(InitialAltitudeEventHandlerTest, TestItDefaultsToEnabledIfNoSettingPresent)
        {
            NiceMock<MockUserSettingProviderInterface> mockUserSettings;
            UserSetting userSetting(mockUserSettings);

            ON_CALL(mockUserSettings, GetKey(GeneralSettingsEntries::initialAltitudeToggleSettingsKey))
                .WillByDefault(Return("0"));

            NiceMock<MockUserSettingProviderInterface> mockUserSettingsNoSetting;
            UserSetting userSettingNoSetting(mockUserSettingsNoSetting);

            ON_CALL(mockUserSettingsNoSetting, GetKey(GeneralSettingsEntries::initialAltitudeToggleSettingsKey))
                .WillByDefault(Return(""));

            this->handler.UserSettingsUpdated(userSetting);
            EXPECT_FALSE(this->handler.UserAutomaticAssignmentsAllowed());
            this->handler.UserSettingsUpdated(userSettingNoSetting);
            EXPECT_TRUE(this->handler.UserAutomaticAssignmentsAllowed());
        }

        TEST_F(InitialAltitudeEventHandlerTest, FlightPlanEventDoesNothingIfUserSettingDisabled)
        {
            NiceMock<MockUserSettingProviderInterface> mockUserSettings;
            UserSetting userSetting(mockUserSettings);

            ON_CALL(mockUserSettings, GetKey(GeneralSettingsEntries::initialAltitudeToggleSettingsKey))
                .WillByDefault(Return("0"));

            this->handler.UserSettingsUpdated(userSetting);

            StrictMock<MockEuroScopeCFlightPlanInterface> mockFp;
            StrictMock<MockEuroScopeCRadarTargetInterface> mockRt;
            EXPECT_NO_THROW(this->handler.FlightPlanEvent(mockFp, mockRt));
        }

        TEST_F(InitialAltitudeEventHandlerTest, FlightPlanEventDoesNothingOnProxies)
        {
            this->login.SetLoginStatus(EuroScopePlugIn::CONNECTION_TYPE_VIA_PROXY);

            StrictMock<MockEuroScopeCFlightPlanInterface> mockFp;
            StrictMock<MockEuroScopeCRadarTargetInterface> mockRt;
            EXPECT_NO_THROW(this->handler.FlightPlanEvent(mockFp, mockRt));
        }

        TEST_F(InitialAltitudeEventHandlerTest, FlightPlanEventDefersIfNotLoggedInLongEnough)
        {
            EXPECT_CALL(this->mockFlightPlan, GetCallsign)
                .Times(1)
                .WillOnce(Return("BAW123"));

            EXPECT_CALL(this->mockRadarTarget, GetFlightLevel)
                .Times(0);

            login.SetLoginTime(std::chrono::system_clock::now() + std::chrono::minutes(15));
            handler.FlightPlanEvent(mockFlightPlan, mockRadarTarget);
        }

        TEST_F(InitialAltitudeEventHandlerTest, FlightPlanEventDoesNotAssignIfTooHigh)
        {
            EXPECT_CALL(mockRadarTarget, GetFlightLevel())
                .Times(1)
                .WillOnce(Return(handler.assignmentMaxAltitude + 1));

            handler.FlightPlanEvent(mockFlightPlan, mockRadarTarget);
        }

        TEST_F(InitialAltitudeEventHandlerTest, FlightPlanEventDoesNotAssignIfExactlyAtSeaLevel)
        {
            EXPECT_CALL(mockRadarTarget, GetFlightLevel())
                .Times(1)
                .WillOnce(Return(0));

            handler.FlightPlanEvent(mockFlightPlan, mockRadarTarget);
        }

        TEST_F(InitialAltitudeEventHandlerTest, FlightPlanEventDoesNotAssignIfTooFarFromOrigin)
        {
            EXPECT_CALL(mockRadarTarget, GetFlightLevel())
                .Times(2)
                .WillRepeatedly(Return(handler.assignmentMaxAltitude));

            EXPECT_CALL(mockFlightPlan, GetDistanceFromOrigin())
                .Times(2)
                .WillRepeatedly(Return(handler.assignmentMaxDistanceFromOrigin + 1));

            handler.FlightPlanEvent(mockFlightPlan, mockRadarTarget);
        }

        TEST_F(InitialAltitudeEventHandlerTest, FlightPlanEventDoesNotAssignIfExactlyOnOrigin)
        {
            EXPECT_CALL(mockRadarTarget, GetFlightLevel())
                .Times(1)
                .WillOnce(Return(handler.assignmentMaxAltitude));

            EXPECT_CALL(mockFlightPlan, GetDistanceFromOrigin())
                .Times(1)
                .WillOnce(Return(0));

            handler.FlightPlanEvent(mockFlightPlan, mockRadarTarget);
        }

        TEST_F(InitialAltitudeEventHandlerTest, FlightPlanEventDoesNotAssignIfTravellingTooFast)
        {
            EXPECT_CALL(mockFlightPlan, GetDistanceFromOrigin())
                .Times(2)
                .WillRepeatedly(Return(handler.assignmentMaxDistanceFromOrigin));

            EXPECT_CALL(mockRadarTarget, GetFlightLevel())
                .Times(2)
                .WillRepeatedly(Return(handler.assignmentMaxAltitude));

            EXPECT_CALL(mockRadarTarget, GetGroundSpeed())
                .Times(1)
                .WillOnce(Return(handler.assignmentMaxSpeed + 1));

            handler.FlightPlanEvent(mockFlightPlan, mockRadarTarget);
        }

        TEST_F(InitialAltitudeEventHandlerTest, FlightPlanEventDoesNotAssignIfAlreadyHasAClearedAltitude)
        {
            EXPECT_CALL(mockFlightPlan, GetDistanceFromOrigin())
                .Times(2)
                .WillOnce(Return(handler.assignmentMaxDistanceFromOrigin));

            EXPECT_CALL(mockRadarTarget, GetFlightLevel())
                .Times(2)
                .WillOnce(Return(handler.assignmentMaxAltitude));

            EXPECT_CALL(mockFlightPlan, HasControllerClearedAltitude())
                .Times(1)
                .WillOnce(Return(true));

            EXPECT_CALL(mockRadarTarget, GetGroundSpeed())
                .Times(1)
                .WillOnce(Return(handler.assignmentMaxSpeed));

            handler.FlightPlanEvent(mockFlightPlan, mockRadarTarget);
        }

        TEST_F(InitialAltitudeEventHandlerTest, FlightPlanEventDoesNotAssignIfTracked)
        {
            EXPECT_CALL(mockFlightPlan, GetDistanceFromOrigin())
                .Times(2)
                .WillRepeatedly(Return(handler.assignmentMaxDistanceFromOrigin));

            EXPECT_CALL(mockRadarTarget, GetFlightLevel())
                .Times(2)
                .WillRepeatedly(Return(handler.assignmentMaxAltitude));

            EXPECT_CALL(mockFlightPlan, HasControllerClearedAltitude())
                .Times(1)
                .WillOnce(Return(false));

            EXPECT_CALL(mockFlightPlan, IsTracked())
                .Times(1)
                .WillOnce(Return(true));

            EXPECT_CALL(mockRadarTarget, GetGroundSpeed())
                .Times(1)
                .WillOnce(Return(handler.assignmentMaxSpeed));

            handler.FlightPlanEvent(mockFlightPlan, mockRadarTarget);
        }

        TEST_F(InitialAltitudeEventHandlerTest, FlightPlanEventDoesNotAssignIfAircraftIsSimulated)
        {
            EXPECT_CALL(mockFlightPlan, GetDistanceFromOrigin())
                .Times(2)
                .WillOnce(Return(handler.assignmentMaxDistanceFromOrigin));

            EXPECT_CALL(mockRadarTarget, GetFlightLevel())
                .Times(2)
                .WillOnce(Return(handler.assignmentMaxAltitude));

            EXPECT_CALL(mockFlightPlan, HasControllerClearedAltitude())
                .Times(1)
                .WillOnce(Return(false));

            EXPECT_CALL(mockFlightPlan, IsTracked())
                .Times(1)
                .WillOnce(Return(false));

            EXPECT_CALL(mockFlightPlan, IsSimulated())
                .Times(1)
                .WillOnce(Return(true));

            EXPECT_CALL(mockRadarTarget, GetGroundSpeed())
                .Times(1)
                .WillOnce(Return(handler.assignmentMaxSpeed));

            handler.FlightPlanEvent(mockFlightPlan, mockRadarTarget);
        }

        TEST_F(InitialAltitudeEventHandlerTest, FlightPlanEventDoesNotAssignIfCruiseIsLessThanInitialAltitude)
        {
            ON_CALL(mockFlightPlan, GetOrigin())
                .WillByDefault(Return("EGKK"));

            ON_CALL(mockFlightPlan, GetCruiseLevel())
                .WillByDefault(Return(3000));

            ON_CALL(mockFlightPlan, GetSidName())
                .WillByDefault(Return("ADMAG2X"));

            EXPECT_CALL(mockFlightPlan, SetClearedAltitude(_))
                .Times(0);

            handler.FlightPlanEvent(mockFlightPlan, mockRadarTarget);
        }

        TEST_F(InitialAltitudeEventHandlerTest, FlightPlanEventDoesNotAssignIfUserCallsignIsNotActive)
        {
            EXPECT_CALL(mockFlightPlan, GetDistanceFromOrigin())
                .Times(2)
                .WillRepeatedly(Return(handler.assignmentMaxDistanceFromOrigin));

            EXPECT_CALL(mockRadarTarget, GetFlightLevel())
                .Times(2)
                .WillRepeatedly(Return(handler.assignmentMaxAltitude));

            EXPECT_CALL(mockFlightPlan, HasControllerClearedAltitude())
                .Times(1)
                .WillOnce(Return(false));

            EXPECT_CALL(mockFlightPlan, IsTracked())
                .Times(1)
                .WillOnce(Return(false));

            EXPECT_CALL(mockFlightPlan, IsSimulated())
                .Times(1)
                .WillOnce(Return(false));

            EXPECT_CALL(mockFlightPlan, GetOrigin())
                .Times(1)
                .WillOnce(Return("EGKK"));

            EXPECT_CALL(mockRadarTarget, GetGroundSpeed())
                .Times(1)
                .WillOnce(Return(handler.assignmentMaxSpeed));

            handler.FlightPlanEvent(mockFlightPlan, mockRadarTarget);
        }

        TEST_F(InitialAltitudeEventHandlerTest, FlightPlanEventDoesNotAssignIfAirfieldIsNotOwnedByUser)
        {
            callsigns.AddUserCallsign(userCallsign);

            EXPECT_CALL(mockFlightPlan, GetDistanceFromOrigin())
                .Times(2)
                .WillOnce(Return(handler.assignmentMaxDistanceFromOrigin));

            EXPECT_CALL(mockRadarTarget, GetFlightLevel())
                .Times(2)
                .WillOnce(Return(handler.assignmentMaxAltitude));

            EXPECT_CALL(mockFlightPlan, HasControllerClearedAltitude())
                .Times(1)
                .WillOnce(Return(false));

            EXPECT_CALL(mockFlightPlan, IsTracked())
                .Times(1)
                .WillOnce(Return(false));

            EXPECT_CALL(mockFlightPlan, IsSimulated())
                .Times(1)
                .WillOnce(Return(false));

            EXPECT_CALL(mockFlightPlan, GetOrigin())
                .Times(1)
                .WillOnce(Return("EGKK"));

            EXPECT_CALL(mockRadarTarget, GetGroundSpeed())
                .Times(1)
                .WillOnce(Return(handler.assignmentMaxSpeed));

            handler.FlightPlanEvent(mockFlightPlan, mockRadarTarget);
        }

        TEST_F(InitialAltitudeEventHandlerTest, FlightPlanEventDoesNotAssignIfSidNotFound)
        {
            callsigns.AddUserCallsign(userCallsign);

            airfields.AddAirfield(std::unique_ptr<AirfieldModel>(new AirfieldModel("EGKK", { "LON_S_CTR" })));
            owners.RefreshOwner("EGKK");

            EXPECT_CALL(mockFlightPlan, GetDistanceFromOrigin())
                .Times(2)
                .WillRepeatedly(Return(handler.assignmentMaxDistanceFromOrigin));

            EXPECT_CALL(mockRadarTarget, GetFlightLevel())
                .Times(2)
                .WillRepeatedly(Return(handler.assignmentMaxAltitude));

            EXPECT_CALL(mockFlightPlan, HasControllerClearedAltitude())
                .Times(1)
                .WillOnce(Return(false));

            EXPECT_CALL(mockFlightPlan, IsTracked())
                .Times(1)
                .WillOnce(Return(false));

            EXPECT_CALL(mockFlightPlan, IsSimulated())
                .Times(1)
                .WillOnce(Return(false));

            EXPECT_CALL(mockFlightPlan, GetSidName())
                .Times(1)
                .WillOnce(Return("ADMAG1X"));

            EXPECT_CALL(mockFlightPlan, GetOrigin())
                .Times(2)
                .WillRepeatedly(Return("EGKK"));

            EXPECT_CALL(mockRadarTarget, GetGroundSpeed())
                .Times(1)
                .WillOnce(Return(handler.assignmentMaxSpeed));

            handler.FlightPlanEvent(mockFlightPlan, mockRadarTarget);
        }

        TEST_F(InitialAltitudeEventHandlerTest, FlightPlanEventDoesNotAssignIfAlreadyAssignedOnSameSid)
        {
            callsigns.AddUserCallsign(userCallsign);

            airfields.AddAirfield(std::unique_ptr<AirfieldModel>(new AirfieldModel("EGKK", { "LON_S_CTR" })));
            owners.RefreshOwner("EGKK");

            EXPECT_CALL(mockFlightPlan, GetDistanceFromOrigin())
                .Times(2)
                .WillRepeatedly(Return(handler.assignmentMaxDistanceFromOrigin));

            EXPECT_CALL(mockRadarTarget, GetFlightLevel())
                .Times(2)
                .WillRepeatedly(Return(handler.assignmentMaxAltitude));

            EXPECT_CALL(mockFlightPlan, HasControllerClearedAltitude())
                .Times(1)
                .WillOnce(Return(false));

            EXPECT_CALL(mockFlightPlan, IsTracked())
                .Times(1)
                .WillOnce(Return(false));

            EXPECT_CALL(mockFlightPlan, IsSimulated())
                .Times(1)
                .WillOnce(Return(false));

            EXPECT_CALL(mockFlightPlan, GetSidName())
                .Times(3)
                .WillRepeatedly(Return("ADMAG2X"));

            EXPECT_CALL(mockFlightPlan, GetOrigin())
                .Times(3)
                .WillRepeatedly(Return("EGKK"));

            EXPECT_CALL(mockFlightPlan, GetCallsign())
                .Times(5)
                .WillRepeatedly(Return("BAW123"));

            EXPECT_CALL(mockFlightPlan, GetCruiseLevel())
                .Times(1)
                .WillOnce(Return(6000));

            EXPECT_CALL(mockFlightPlan, SetClearedAltitude(6000))
                .Times(1);

            EXPECT_CALL(mockRadarTarget, GetGroundSpeed())
                .Times(1)
                .WillOnce(Return(handler.assignmentMaxSpeed));

            handler.FlightPlanEvent(mockFlightPlan, mockRadarTarget);
            handler.FlightPlanEvent(mockFlightPlan, mockRadarTarget);
        }

        TEST_F(InitialAltitudeEventHandlerTest, FlightPlanEventAssignsIfSidFound)
        {
            callsigns.AddUserCallsign(userCallsign);

            airfields.AddAirfield(std::unique_ptr<AirfieldModel>(new AirfieldModel("EGKK", { "LON_S_CTR" })));
            owners.RefreshOwner("EGKK");

            EXPECT_CALL(mockFlightPlan, GetDistanceFromOrigin())
                .Times(2)
                .WillRepeatedly(Return(handler.assignmentMaxDistanceFromOrigin));

            EXPECT_CALL(mockRadarTarget, GetFlightLevel())
                .Times(2)
                .WillRepeatedly(Return(handler.assignmentMaxAltitude));

            EXPECT_CALL(mockFlightPlan, HasControllerClearedAltitude())
                .Times(1)
                .WillOnce(Return(false));

            EXPECT_CALL(mockFlightPlan, IsTracked())
                .Times(1)
                .WillOnce(Return(false));

            EXPECT_CALL(mockFlightPlan, IsSimulated())
                .Times(1)
                .WillOnce(Return(false));

            EXPECT_CALL(mockFlightPlan, GetSidName())
                .Times(2)
                .WillOnce(Return("ADMAG2X"));

            EXPECT_CALL(mockFlightPlan, GetOrigin())
                .Times(3)
                .WillRepeatedly(Return("EGKK"));

            EXPECT_CALL(mockFlightPlan, GetCallsign())
                .Times(3)
                .WillRepeatedly(Return("BAW123"));

            EXPECT_CALL(mockFlightPlan, GetCruiseLevel())
                .Times(1)
                .WillOnce(Return(6000));

            EXPECT_CALL(mockFlightPlan, SetClearedAltitude(6000))
                .Times(1);

            EXPECT_CALL(mockRadarTarget, GetGroundSpeed())
                .Times(1)
                .WillOnce(Return(handler.assignmentMaxSpeed));

            handler.FlightPlanEvent(mockFlightPlan, mockRadarTarget);
        }

        TEST_F(InitialAltitudeEventHandlerTest, FlightPlanEventAcceptsDeprecatedSids)
        {
            callsigns.AddUserCallsign(userCallsign);

            airfields.AddAirfield(std::unique_ptr<AirfieldModel>(new AirfieldModel("EGKK", { "LON_S_CTR" })));
            owners.RefreshOwner("EGKK");

            EXPECT_CALL(mockFlightPlan, GetDistanceFromOrigin())
                .Times(2)
                .WillRepeatedly(Return(handler.assignmentMaxDistanceFromOrigin));

            EXPECT_CALL(mockRadarTarget, GetFlightLevel())
                .Times(2)
                .WillRepeatedly(Return(handler.assignmentMaxAltitude));

            EXPECT_CALL(mockFlightPlan, HasControllerClearedAltitude())
                .Times(1)
                .WillOnce(Return(false));

            EXPECT_CALL(mockFlightPlan, IsTracked())
                .Times(1)
                .WillOnce(Return(false));

            EXPECT_CALL(mockFlightPlan, IsSimulated())
                .Times(1)
                .WillOnce(Return(false));

            EXPECT_CALL(mockFlightPlan, GetSidName())
                .Times(2)
                .WillRepeatedly(Return("#ADMAG2X"));

            EXPECT_CALL(mockFlightPlan, GetOrigin())
                .Times(3)
                .WillRepeatedly(Return("EGKK"));

            EXPECT_CALL(mockFlightPlan, GetCallsign())
                .Times(3)
                .WillRepeatedly(Return("BAW123"));

            EXPECT_CALL(mockFlightPlan, GetCruiseLevel())
                .Times(1)
                .WillOnce(Return(6000));

            EXPECT_CALL(mockFlightPlan, SetClearedAltitude(6000))
                .Times(1);

            EXPECT_CALL(mockRadarTarget, GetGroundSpeed())
                .Times(1)
                .WillOnce(Return(handler.assignmentMaxSpeed));

            handler.FlightPlanEvent(mockFlightPlan, mockRadarTarget);
        }

        TEST_F(InitialAltitudeEventHandlerTest, FlightPlanEventDoesAssignIfDifferentSid)
        {
            callsigns.AddUserCallsign(userCallsign);

            airfields.AddAirfield(std::unique_ptr<AirfieldModel>(new AirfieldModel("EGKK", { "LON_S_CTR" })));
            owners.RefreshOwner("EGKK");

            EXPECT_CALL(mockFlightPlan, GetDistanceFromOrigin())
                .Times(4)
                .WillRepeatedly(Return(handler.assignmentMaxDistanceFromOrigin));

            EXPECT_CALL(mockRadarTarget, GetFlightLevel())
                .Times(4)
                .WillRepeatedly(Return(handler.assignmentMaxAltitude));

            EXPECT_CALL(mockFlightPlan, HasControllerClearedAltitude())
                .Times(2)
                .WillRepeatedly(Return(false));

            EXPECT_CALL(mockFlightPlan, IsTracked())
                .Times(2)
                .WillRepeatedly(Return(false));

            EXPECT_CALL(mockFlightPlan, IsSimulated())
                .Times(2)
                .WillRepeatedly(Return(false));

            EXPECT_CALL(mockFlightPlan, GetSidName())
                .Times(5)
                .WillOnce(Return("ADMAG2X"))
                .WillOnce(Return("ADMAG2X"))
                .WillOnce(Return("CLN3X"))
                .WillOnce(Return("CLN3X"))
                .WillOnce(Return("CLN3X"));

            EXPECT_CALL(mockFlightPlan, GetOrigin())
                .Times(6)
                .WillRepeatedly(Return("EGKK"));

            EXPECT_CALL(mockFlightPlan, GetCallsign())
                .Times(7)
                .WillRepeatedly(Return("BAW123"));

            EXPECT_CALL(mockFlightPlan, GetCruiseLevel())
                .Times(2)
                .WillRepeatedly(Return(6000));

            EXPECT_CALL(mockFlightPlan, SetClearedAltitude(5000))
                .Times(1);

            EXPECT_CALL(mockFlightPlan, SetClearedAltitude(6000))
                .Times(1);

            EXPECT_CALL(mockRadarTarget, GetGroundSpeed())
                .Times(2)
                .WillRepeatedly(Return(handler.assignmentMaxSpeed));

            handler.FlightPlanEvent(mockFlightPlan, mockRadarTarget);
            handler.FlightPlanEvent(mockFlightPlan, mockRadarTarget);
        }

        TEST_F(InitialAltitudeEventHandlerTest, FlightPlanEventDoesAssignAfterDisconnect)
        {
            callsigns.AddUserCallsign(userCallsign);

            airfields.AddAirfield(std::unique_ptr<AirfieldModel>(new AirfieldModel("EGKK", { "LON_S_CTR" })));
            owners.RefreshOwner("EGKK");

            EXPECT_CALL(mockFlightPlan, GetDistanceFromOrigin())
                .Times(4)
                .WillRepeatedly(Return(handler.assignmentMaxDistanceFromOrigin));

            EXPECT_CALL(mockRadarTarget, GetFlightLevel())
                .Times(4)
                .WillRepeatedly(Return(handler.assignmentMaxAltitude));

            EXPECT_CALL(mockFlightPlan, HasControllerClearedAltitude())
                .Times(2)
                .WillRepeatedly(Return(false));

            EXPECT_CALL(mockFlightPlan, IsTracked())
                .Times(2)
                .WillRepeatedly(Return(false));

            EXPECT_CALL(mockFlightPlan, IsSimulated())
                .Times(2)
                .WillRepeatedly(Return(false));

            EXPECT_CALL(mockFlightPlan, GetSidName())
                .Times(4)
                .WillRepeatedly(Return("ADMAG2X"));

            EXPECT_CALL(mockFlightPlan, GetOrigin())
                .Times(6)
                .WillRepeatedly(Return("EGKK"));

            EXPECT_CALL(mockFlightPlan, GetCallsign())
                .Times(7)
                .WillRepeatedly(Return("BAW123"));

            EXPECT_CALL(mockFlightPlan, GetCruiseLevel())
                .Times(2)
                .WillRepeatedly(Return(6000));

            EXPECT_CALL(mockFlightPlan, SetClearedAltitude(6000))
                .Times(2);

            EXPECT_CALL(mockRadarTarget, GetGroundSpeed())
                .Times(2)
                .WillRepeatedly(Return(handler.assignmentMaxSpeed));

            handler.FlightPlanEvent(mockFlightPlan, mockRadarTarget);
            handler.FlightPlanDisconnectEvent(mockFlightPlan);
            handler.FlightPlanEvent(mockFlightPlan, mockRadarTarget);
        }

        TEST_F(InitialAltitudeEventHandlerTest, RecycleMarksAsAlreadyAssigned)
        {
            ON_CALL(this->mockFlightPlan, GetSidName())
                .WillByDefault(Return("ADMAG2X"));

            ON_CALL(this->mockFlightPlan, GetOrigin())
                .WillByDefault(Return("EGKK"));

            ON_CALL(this->mockFlightPlan, GetCallsign())
                .WillByDefault(Return("BAW123"));

            ON_CALL(this->mockFlightPlan, IsTracked())
                .WillByDefault(Return(false));

            EXPECT_CALL(this->mockFlightPlan, SetClearedAltitude(6000))
                .Times(1);

            EXPECT_CALL(mockFlightPlan, GetDistanceFromOrigin())
                .Times(0);

            EXPECT_CALL(mockFlightPlan, HasControllerClearedAltitude())
                .Times(0);

            EXPECT_CALL(mockFlightPlan, IsSimulated())
                .Times(0);

            EXPECT_CALL(mockFlightPlan, GetCruiseLevel())
                .Times(0);

            EXPECT_CALL(mockRadarTarget, GetGroundSpeed())
                .Times(0);

            handler.RecycleInitialAltitude(this->mockFlightPlan, this->mockRadarTarget, "", POINT());
            handler.FlightPlanEvent(mockFlightPlan, mockRadarTarget);
        }

        TEST_F(InitialAltitudeEventHandlerTest, RecycleSetsInitialAltitude)
        {
            ON_CALL(this->mockFlightPlan, GetSidName())
                .WillByDefault(Return("ADMAG2X"));

            ON_CALL(this->mockFlightPlan, GetOrigin())
                .WillByDefault(Return("EGKK"));

            ON_CALL(this->mockFlightPlan, GetCallsign())
                .WillByDefault(Return("BAW123"));

            ON_CALL(this->mockFlightPlan, IsTracked())
                .WillByDefault(Return(false));

            EXPECT_CALL(this->mockFlightPlan, SetClearedAltitude(6000))
                .Times(1);

            handler.RecycleInitialAltitude(this->mockFlightPlan, this->mockRadarTarget, "", POINT());
        }

        TEST_F(InitialAltitudeEventHandlerTest, RecycleSetsInitialAltitudeWhenTrackedByUser)
        {
            ON_CALL(this->mockFlightPlan, GetSidName())
                .WillByDefault(Return("ADMAG2X"));

            ON_CALL(this->mockFlightPlan, GetOrigin())
                .WillByDefault(Return("EGKK"));

            ON_CALL(this->mockFlightPlan, GetCallsign())
                .WillByDefault(Return("BAW123"));

            ON_CALL(this->mockFlightPlan, IsTracked())
                .WillByDefault(Return(true));

            ON_CALL(this->mockFlightPlan, IsTrackedByUser())
                .WillByDefault(Return(true));

            EXPECT_CALL(this->mockFlightPlan, SetClearedAltitude(6000))
                .Times(1);

            handler.RecycleInitialAltitude(this->mockFlightPlan, this->mockRadarTarget, "", POINT());
        }

        TEST_F(InitialAltitudeEventHandlerTest, RecycleDoesNothingIfAircraftTrackedByAnotherUser)
        {
            ON_CALL(this->mockFlightPlan, GetSidName())
                .WillByDefault(Return("ADMAG2X"));

            ON_CALL(this->mockFlightPlan, GetOrigin())
                .WillByDefault(Return("EGKK"));

            ON_CALL(this->mockFlightPlan, GetCallsign())
                .WillByDefault(Return("BAW123"));

            ON_CALL(this->mockFlightPlan, IsTracked())
                .WillByDefault(Return(true));

            ON_CALL(this->mockFlightPlan, IsTrackedByUser())
                .WillByDefault(Return(false));

            EXPECT_CALL(this->mockFlightPlan, SetClearedAltitude(_))
                .Times(0);

            handler.RecycleInitialAltitude(this->mockFlightPlan, this->mockRadarTarget, "", POINT());
        }

        TEST_F(InitialAltitudeEventHandlerTest, RecycleDoesNothingIfNoSidFound)
        {
            ON_CALL(this->mockFlightPlan, GetSidName())
                .WillByDefault(Return("ADMAG3X"));

            ON_CALL(this->mockFlightPlan, GetOrigin())
                .WillByDefault(Return("EGKK"));

            ON_CALL(this->mockFlightPlan, GetCallsign())
                .WillByDefault(Return("BAW123"));

            ON_CALL(this->mockFlightPlan, IsTracked())
                .WillByDefault(Return(false));

            EXPECT_CALL(this->mockFlightPlan, SetClearedAltitude(_))
                .Times(0);

            handler.RecycleInitialAltitude(this->mockFlightPlan, this->mockRadarTarget, "", POINT());
        }

        TEST_F(InitialAltitudeEventHandlerTest, NewActiveCallsignAssignsIfUserCallsign)
        {
            callsigns.AddUserCallsign(userCallsign);

            airfields.AddAirfield(std::unique_ptr<AirfieldModel>(new AirfieldModel("EGKK", { "LON_S_CTR" })));
            owners.RefreshOwner("EGKK");
            this->plugin.AddAllFlightplansItem({this->mockFlightplanPointer, this->mockRadarTargetPointer});

            ON_CALL(this->plugin, GetFlightplanForCallsign("BAW123"))
                .WillByDefault(Return(mockFlightplanPointer));

            ON_CALL(this->plugin, GetRadarTargetForCallsign("BAW123"))
                .WillByDefault(Return(mockRadarTargetPointer));

            EXPECT_CALL(*mockFlightplanPointer, GetDistanceFromOrigin())
                .Times(2)
                .WillRepeatedly(Return(handler.assignmentMaxDistanceFromOrigin));

            EXPECT_CALL(*mockFlightplanPointer, HasControllerClearedAltitude())
                .Times(1)
                .WillOnce(Return(false));

            EXPECT_CALL(*mockFlightplanPointer, IsTracked())
                .Times(1)
                .WillOnce(Return(false));

            EXPECT_CALL(*mockFlightplanPointer, IsSimulated())
                .Times(1)
                .WillOnce(Return(false));

            EXPECT_CALL(*mockFlightplanPointer, GetSidName())
                .Times(2)
                .WillRepeatedly(Return("ADMAG2X"));

            EXPECT_CALL(*mockFlightplanPointer, GetOrigin())
                .Times(3)
                .WillRepeatedly(Return("EGKK"));

            EXPECT_CALL(*mockFlightplanPointer, GetCallsign())
                .Times(3)
                .WillRepeatedly(Return("BAW123"));

            EXPECT_CALL(*mockFlightplanPointer, GetCruiseLevel())
                .Times(1)
                .WillOnce(Return(6000));

            EXPECT_CALL(*mockFlightplanPointer, SetClearedAltitude(6000))
                .Times(1);

            EXPECT_CALL(*mockRadarTargetPointer, GetGroundSpeed())
                .Times(1)
                .WillOnce(Return(handler.assignmentMaxSpeed));

            EXPECT_CALL(*mockRadarTargetPointer, GetFlightLevel())
                .Times(2)
                .WillRepeatedly(Return(handler.assignmentMaxSpeed));

            handler.ActiveCallsignAdded(userCallsign, true);
        }

        TEST_F(InitialAltitudeEventHandlerTest, NewActiveCallsignDoesNotAssignIfNotUserCallsign)
        {
            callsigns.AddCallsign(userCallsign);
            airfields.AddAirfield(std::unique_ptr<AirfieldModel>(new AirfieldModel("EGKK", { "LON_S_CTR" })));
            owners.RefreshOwner("EGKK");
            this->plugin.AddAllFlightplansItem({this->mockFlightplanPointer, this->mockRadarTargetPointer});
            this->plugin.ExpectNoFlightplanLoop();

            handler.ActiveCallsignAdded(userCallsign, false);
        }

        TEST_F(InitialAltitudeEventHandlerTest, TimedEventAssignsIfUserCallsign)
        {
            callsigns.AddUserCallsign(userCallsign);

            airfields.AddAirfield(std::make_unique<AirfieldModel>("EGKK", std::vector<std::string>{"LON_S_CTR"}));
            owners.RefreshOwner("EGKK");
            this->plugin.AddAllFlightplansItem({this->mockFlightplanPointer, this->mockRadarTargetPointer});

            ON_CALL(this->plugin, GetFlightplanForCallsign("BAW123"))
                .WillByDefault(Return(mockFlightplanPointer));

            ON_CALL(this->plugin, GetRadarTargetForCallsign("BAW123"))
                .WillByDefault(Return(mockRadarTargetPointer));

            EXPECT_CALL(*mockFlightplanPointer, GetDistanceFromOrigin())
                .Times(2)
                .WillRepeatedly(Return(handler.assignmentMaxDistanceFromOrigin));

            EXPECT_CALL(*mockFlightplanPointer, HasControllerClearedAltitude())
                .Times(1)
                .WillOnce(Return(false));

            EXPECT_CALL(*mockFlightplanPointer, IsTracked())
                .Times(1)
                .WillOnce(Return(false));

            EXPECT_CALL(*mockFlightplanPointer, IsSimulated())
                .Times(1)
                .WillOnce(Return(false));

            EXPECT_CALL(*mockFlightplanPointer, GetSidName())
                .Times(2)
                .WillRepeatedly(Return("ADMAG2X"));

            EXPECT_CALL(*mockFlightplanPointer, GetOrigin())
                .Times(3)
                .WillRepeatedly(Return("EGKK"));

            EXPECT_CALL(*mockFlightplanPointer, GetCallsign())
                .Times(3)
                .WillRepeatedly(Return("BAW123"));

            EXPECT_CALL(*mockFlightplanPointer, GetCruiseLevel())
                .Times(1)
                .WillOnce(Return(6000));

            EXPECT_CALL(*mockFlightplanPointer, SetClearedAltitude(6000))
                .Times(1);

            EXPECT_CALL(*mockRadarTargetPointer, GetGroundSpeed())
                .Times(1)
                .WillOnce(Return(handler.assignmentMaxSpeed));

            EXPECT_CALL(*mockRadarTargetPointer, GetFlightLevel())
                .Times(2)
                .WillRepeatedly(Return(handler.assignmentMaxSpeed));

            handler.TimedEventTrigger();
        }

        TEST_F(InitialAltitudeEventHandlerTest, TimedEventDoesNotAssignIfNoUserCallsign)
        {
            callsigns.AddCallsign(userCallsign);
            airfields.AddAirfield(std::make_unique<AirfieldModel>("EGKK", std::vector<std::string>{"LON_S_CTR"}));
            owners.RefreshOwner("EGKK");
            this->plugin.AddAllFlightplansItem({this->mockFlightplanPointer, this->mockRadarTargetPointer});
            this->plugin.ExpectNoFlightplanLoop();

            handler.TimedEventTrigger();
        }
    }  // namespace InitialAltitude
}  // namespace UKControllerPluginTest
