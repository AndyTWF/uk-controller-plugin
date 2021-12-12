#include "api/ApiException.h"
#include "controller/ControllerPosition.h"
#include "missedapproach/MissedApproach.h"
#include "missedapproach/MissedApproachAudioAlert.h"
#include "missedapproach/MissedApproachCollection.h"
#include "missedapproach/MissedApproachOptions.h"
#include "missedapproach/MissedApproachTriggeredMessage.h"
#include "missedapproach/TriggerMissedApproach.h"
#include "ownership/AirfieldServiceProviderCollection.h"
#include "ownership/ServiceProvision.h"
#include "time/ParseTimeStrings.h"
#include "time/SystemClock.h"

using ::testing::NiceMock;
using UKControllerPlugin::Api::ApiException;
using UKControllerPlugin::Controller::ActiveCallsign;
using UKControllerPlugin::Controller::ControllerPosition;
using UKControllerPlugin::MissedApproach::MissedApproach;
using UKControllerPlugin::MissedApproach::MissedApproachAudioAlert;
using UKControllerPlugin::MissedApproach::MissedApproachCollection;
using UKControllerPlugin::MissedApproach::MissedApproachOptions;
using UKControllerPlugin::MissedApproach::MissedApproachTriggeredMessage;
using UKControllerPlugin::MissedApproach::TriggerMissedApproach;
using UKControllerPlugin::Ownership::AirfieldServiceProviderCollection;
using UKControllerPlugin::Ownership::ServiceProvision;
using UKControllerPlugin::Ownership::ServiceType;
using UKControllerPlugin::Time::ParseTimeString;
using UKControllerPlugin::Time::SetTestNow;
using UKControllerPlugin::Time::TimeNow;
using UKControllerPluginTest::Api::MockApiInterface;
using UKControllerPluginTest::Euroscope::MockEuroscopePluginLoopbackInterface;
using UKControllerPluginTest::Windows::MockWinApi;

namespace UKControllerPluginTest::MissedApproach {
    class TriggerMissedApproachTest : public testing::Test
    {
        public:
        TriggerMissedApproachTest()
            : successFunction([this]() { successCalled = true; }),
              failFunction([this](std::vector<std::string>) { failCalled = true; }),
              kkTwr(2, "EGKK_TWR", 199.999, {"EGKK"}, true, false),
              kkApp(3, "EGKK_APP", 199.999, {"EGKK"}, true, false),
              userTowerCallsign(std::make_shared<ActiveCallsign>("EGKK_TWR", "Testy McTest", kkTwr, true)),
              notUserTowerCallsign(std::make_shared<ActiveCallsign>("EGKK_TWR", "Testy McTest", kkTwr, false)),
              userAppCallsign(std::make_shared<ActiveCallsign>("EGKK_TWR", "Testy McTest", kkApp, true)),
              missed1(
                  std::make_shared<class MissedApproach>(1, "BAW123", ParseTimeString("2021-08-23 13:56:00"), false)),
              missed2(
                  std::make_shared<class MissedApproach>(2, "BAW456", ParseTimeString("2021-08-23 13:56:00"), false)),
              missed3(
                  std::make_shared<class MissedApproach>(3, "BAW123", ParseTimeString("2021-08-23 13:54:00"), false)),
              collection(std::make_shared<MissedApproachCollection>()),
              options(std::make_shared<MissedApproachOptions>()),
              audioAlert(std::make_shared<MissedApproachAudioAlert>(options, plugin, serviceProviders, windows)),
              trigger(collection, windows, api, serviceProviders, audioAlert, mockIntegration)
        {
            SetTestNow(ParseTimeString("2021-08-23 13:55:00"));
            collection->Add(missed2);
            collection->Add(missed3);

            std::vector<std::shared_ptr<ServiceProvision>> provisions;
            provisions.push_back(std::make_shared<ServiceProvision>(ServiceType::Tower, userTowerCallsign));
            serviceProviders.SetProvidersForAirfield("EGKK", provisions);

            ON_CALL(mockFlightplan, GetCallsign).WillByDefault(testing::Return("BAW123"));
            ON_CALL(mockFlightplan, GetDestination).WillByDefault(testing::Return("EGKK"));
            ON_CALL(mockFlightplanBristol, GetCallsign).WillByDefault(testing::Return("BAW123"));
            ON_CALL(mockFlightplanBristol, GetDestination).WillByDefault(testing::Return("EGGD"));

            ON_CALL(mockFlightplan, GetDistanceToDestination()).WillByDefault(testing::Return(5.0));
            ON_CALL(mockRadarTarget, GetFlightLevel()).WillByDefault(testing::Return(3000));
            ON_CALL(mockRadarTarget, GetGroundSpeed()).WillByDefault(testing::Return(100));
        }

        bool successCalled = false;
        bool failCalled = false;
        std::function<void(void)> successFunction;
        std::function<void(std::vector<std::string>)> failFunction;
        AirfieldServiceProviderCollection serviceProviders;
        NiceMock<Integration::MockOutboundIntegrationEventHandler> mockIntegration;
        NiceMock<Euroscope::MockEuroScopeCRadarTargetInterface> mockRadarTarget;
        NiceMock<Euroscope::MockEuroScopeCFlightPlanInterface> mockFlightplan;
        NiceMock<Euroscope::MockEuroScopeCFlightPlanInterface> mockFlightplanBristol;
        ControllerPosition kkTwr;
        ControllerPosition kkApp;
        std::shared_ptr<ActiveCallsign> userTowerCallsign;
        std::shared_ptr<ActiveCallsign> notUserTowerCallsign;
        std::shared_ptr<ActiveCallsign> userAppCallsign;
        NiceMock<MockWinApi> windows;
        NiceMock<MockApiInterface> api;
        NiceMock<MockEuroscopePluginLoopbackInterface> plugin;
        std::shared_ptr<class MissedApproach> missed1;
        std::shared_ptr<class MissedApproach> missed2;
        std::shared_ptr<class MissedApproach> missed3;
        std::shared_ptr<MissedApproachCollection> collection;
        std::shared_ptr<MissedApproachOptions> options;
        std::shared_ptr<MissedApproachAudioAlert> audioAlert;
        TriggerMissedApproach trigger;
    };

    TEST_F(TriggerMissedApproachTest, ItTriggersAMissedApproach)
    {
        nlohmann::json responseData = {{"id", 55}, {"expires_at", "2021-08-23 14:00:00"}};
        EXPECT_CALL(api, CreateMissedApproach("BAW123")).Times(1).WillOnce(testing::Return(responseData));

        EXPECT_CALL(windows, OpenMessageBox(testing::_, testing::_, testing::_))
            .Times(1)
            .WillOnce(testing::Return(IDYES));

        auto expectedMessage =
            std::make_shared<MissedApproachTriggeredMessage>("BAW123", true, ParseTimeString("2021-08-23 14:00:00"));
        EXPECT_CALL(this->mockIntegration, SendEvent(MatchMessageInterface(expectedMessage))).Times(1);

        trigger.Trigger(mockFlightplan, mockRadarTarget, true, successFunction, failFunction);
        EXPECT_EQ(3, collection->Count());
        auto missed = collection->Get(55);
        EXPECT_NE(nullptr, missed);
        EXPECT_EQ(55, missed->Id());
        EXPECT_EQ("BAW123", missed->Callsign());
        EXPECT_EQ(ParseTimeString("2021-08-23 14:00:00"), missed->ExpiresAt());
        EXPECT_TRUE(this->successCalled);
        EXPECT_FALSE(this->failCalled);
    }

    TEST_F(TriggerMissedApproachTest, ItTriggersAMissedApproachWithNoConfirm)
    {
        nlohmann::json responseData = {{"id", 55}, {"expires_at", "2021-08-23 14:00:00"}};
        EXPECT_CALL(api, CreateMissedApproach("BAW123")).Times(1).WillOnce(testing::Return(responseData));

        EXPECT_CALL(windows, OpenMessageBox(testing::_, testing::_, testing::_)).Times(0);

        auto expectedMessage =
            std::make_shared<MissedApproachTriggeredMessage>("BAW123", true, ParseTimeString("2021-08-23 14:00:00"));
        EXPECT_CALL(this->mockIntegration, SendEvent(MatchMessageInterface(expectedMessage))).Times(1);

        trigger.Trigger(mockFlightplan, mockRadarTarget, false, successFunction, failFunction);
        EXPECT_EQ(3, collection->Count());
        auto missed = collection->Get(55);
        EXPECT_NE(nullptr, missed);
        EXPECT_EQ(55, missed->Id());
        EXPECT_EQ("BAW123", missed->Callsign());
        EXPECT_EQ(ParseTimeString("2021-08-23 14:00:00"), missed->ExpiresAt());
        EXPECT_TRUE(this->successCalled);
        EXPECT_FALSE(this->failCalled);
    }

    TEST_F(TriggerMissedApproachTest, ItTriggersAMissedApproachWhenNoneExistForCallsign)
    {
        collection->Remove(missed3);
        nlohmann::json responseData = {{"id", 55}, {"expires_at", "2021-08-23 14:00:00"}};
        EXPECT_CALL(api, CreateMissedApproach("BAW123")).Times(1).WillOnce(testing::Return(responseData));

        EXPECT_CALL(windows, OpenMessageBox(testing::_, testing::_, testing::_))
            .Times(1)
            .WillOnce(testing::Return(IDYES));

        auto expectedMessage =
            std::make_shared<MissedApproachTriggeredMessage>("BAW123", true, ParseTimeString("2021-08-23 14:00:00"));
        EXPECT_CALL(this->mockIntegration, SendEvent(MatchMessageInterface(expectedMessage))).Times(1);

        trigger.Trigger(mockFlightplan, mockRadarTarget, true, successFunction, failFunction);
        EXPECT_EQ(2, collection->Count());
        auto missed = collection->Get(55);
        EXPECT_NE(nullptr, missed);
        EXPECT_EQ(55, missed->Id());
        EXPECT_EQ("BAW123", missed->Callsign());
        EXPECT_EQ(ParseTimeString("2021-08-23 14:00:00"), missed->ExpiresAt());
        EXPECT_TRUE(missed->CreatedByUser());
        EXPECT_TRUE(this->successCalled);
        EXPECT_FALSE(this->failCalled);
    }

    TEST_F(TriggerMissedApproachTest, ItHandlesExpiresAtNotValidTimestampInResponse)
    {
        nlohmann::json responseData = {{"id", 55}, {"expires_at", "abc"}};
        EXPECT_CALL(api, CreateMissedApproach("BAW123")).Times(1).WillOnce(testing::Return(responseData));

        EXPECT_CALL(windows, OpenMessageBox(testing::_, testing::_, testing::_))
            .Times(1)
            .WillOnce(testing::Return(IDYES));

        EXPECT_CALL(this->mockIntegration, SendEvent(testing::_)).Times(0);

        trigger.Trigger(mockFlightplan, mockRadarTarget, true, successFunction, failFunction);
        EXPECT_EQ(2, collection->Count());
        EXPECT_EQ(nullptr, collection->Get(55));
        EXPECT_TRUE(this->failCalled);
        EXPECT_FALSE(this->successCalled);
    }

    TEST_F(TriggerMissedApproachTest, ItHandlesExpiresAtNotStringInResponse)
    {
        nlohmann::json responseData = {{"id", 55}, {"expires_at", 123}};
        EXPECT_CALL(api, CreateMissedApproach("BAW123")).Times(1).WillOnce(testing::Return(responseData));

        EXPECT_CALL(windows, OpenMessageBox(testing::_, testing::_, testing::_))
            .Times(1)
            .WillOnce(testing::Return(IDYES));

        trigger.Trigger(mockFlightplan, mockRadarTarget, true, successFunction, failFunction);
        EXPECT_EQ(2, collection->Count());
        EXPECT_EQ(nullptr, collection->Get(55));
        EXPECT_TRUE(this->failCalled);
        EXPECT_FALSE(this->successCalled);
    }

    TEST_F(TriggerMissedApproachTest, ItHandlesNoExpiresAtInResponse)
    {
        nlohmann::json responseData = {{"id", 55}};
        EXPECT_CALL(api, CreateMissedApproach("BAW123")).Times(1).WillOnce(testing::Return(responseData));

        EXPECT_CALL(windows, OpenMessageBox(testing::_, testing::_, testing::_))
            .Times(1)
            .WillOnce(testing::Return(IDYES));

        trigger.Trigger(mockFlightplan, mockRadarTarget, true, successFunction, failFunction);
        EXPECT_EQ(2, collection->Count());
        EXPECT_EQ(nullptr, collection->Get(55));
        EXPECT_TRUE(this->failCalled);
        EXPECT_FALSE(this->successCalled);
    }

    TEST_F(TriggerMissedApproachTest, ItHandlesIdNotIntInResponse)
    {
        nlohmann::json responseData = {{"id", "abc"}, {"expires_at", "2021-08-23 14:00:00"}};
        EXPECT_CALL(api, CreateMissedApproach("BAW123")).Times(1).WillOnce(testing::Return(responseData));

        EXPECT_CALL(windows, OpenMessageBox(testing::_, testing::_, testing::_))
            .Times(1)
            .WillOnce(testing::Return(IDYES));

        trigger.Trigger(mockFlightplan, mockRadarTarget, true, successFunction, failFunction);
        EXPECT_EQ(2, collection->Count());
        EXPECT_EQ(nullptr, collection->Get(55));
        EXPECT_TRUE(this->failCalled);
        EXPECT_FALSE(this->successCalled);
    }

    TEST_F(TriggerMissedApproachTest, ItHandlesIdMissingInResponse)
    {
        nlohmann::json responseData = {{"expires_at", "2021-08-23 14:00:00"}};
        EXPECT_CALL(api, CreateMissedApproach("BAW123")).Times(1).WillOnce(testing::Return(responseData));

        EXPECT_CALL(windows, OpenMessageBox(testing::_, testing::_, testing::_))
            .Times(1)
            .WillOnce(testing::Return(IDYES));

        trigger.Trigger(mockFlightplan, mockRadarTarget, true, successFunction, failFunction);
        EXPECT_EQ(2, collection->Count());
        EXPECT_EQ(nullptr, collection->Get(55));
        EXPECT_TRUE(this->failCalled);
        EXPECT_FALSE(this->successCalled);
    }

    TEST_F(TriggerMissedApproachTest, ItHandlesResponseNotObject)
    {
        nlohmann::json responseData = nlohmann::json::array();
        EXPECT_CALL(api, CreateMissedApproach("BAW123")).Times(1).WillOnce(testing::Return(responseData));

        EXPECT_CALL(windows, OpenMessageBox(testing::_, testing::_, testing::_))
            .Times(1)
            .WillOnce(testing::Return(IDYES));

        trigger.Trigger(mockFlightplan, mockRadarTarget, true, successFunction, failFunction);
        EXPECT_EQ(2, collection->Count());
        EXPECT_EQ(nullptr, collection->Get(55));
        EXPECT_TRUE(this->failCalled);
        EXPECT_FALSE(this->successCalled);
    }

    TEST_F(TriggerMissedApproachTest, ItHandlesApiException)
    {
        EXPECT_CALL(api, CreateMissedApproach("BAW123")).Times(1).WillOnce(testing::Throw(ApiException("Foo")));

        EXPECT_CALL(windows, OpenMessageBox(testing::_, testing::_, testing::_))
            .Times(1)
            .WillOnce(testing::Return(IDYES));

        EXPECT_CALL(this->mockIntegration, SendEvent(testing::_)).Times(0);

        trigger.Trigger(mockFlightplan, mockRadarTarget, true, successFunction, failFunction);
        EXPECT_EQ(2, collection->Count());
        EXPECT_EQ(nullptr, collection->Get(55));
        EXPECT_TRUE(this->failCalled);
        EXPECT_FALSE(this->successCalled);
    }

    TEST_F(TriggerMissedApproachTest, ItDoesntTriggerIfUserDoesNotConfirm)
    {
        EXPECT_CALL(api, CreateMissedApproach(testing::_)).Times(0);

        EXPECT_CALL(windows, OpenMessageBox(testing::_, testing::_, testing::_))
            .Times(1)
            .WillOnce(testing::Return(IDNO));

        EXPECT_CALL(this->mockIntegration, SendEvent(testing::_)).Times(0);

        trigger.Trigger(mockFlightplan, mockRadarTarget, true, successFunction, failFunction);
        EXPECT_EQ(2, collection->Count());
        EXPECT_EQ(nullptr, collection->Get(55));
        EXPECT_TRUE(this->failCalled);
        EXPECT_FALSE(this->successCalled);
    }

    TEST_F(TriggerMissedApproachTest, ItDoesntTriggerIfAlreadyActive)
    {
        collection->Add(missed1);
        EXPECT_CALL(api, CreateMissedApproach(testing::_)).Times(0);

        EXPECT_CALL(windows, OpenMessageBox(testing::_, testing::_, testing::_)).Times(0);

        trigger.Trigger(mockFlightplan, mockRadarTarget, true, successFunction, failFunction);
        EXPECT_EQ(3, collection->Count());
        EXPECT_EQ(nullptr, collection->Get(55));
        EXPECT_TRUE(this->failCalled);
        EXPECT_FALSE(this->successCalled);
    }

    TEST_F(TriggerMissedApproachTest, ItDoesntTriggerUserDoesntCoverArrivalAirfield)
    {
        EXPECT_CALL(api, CreateMissedApproach(testing::_)).Times(0);

        EXPECT_CALL(windows, OpenMessageBox(testing::_, testing::_, testing::_)).Times(0);

        trigger.Trigger(mockFlightplanBristol, mockRadarTarget, true, successFunction, failFunction);
        EXPECT_EQ(2, collection->Count());
        EXPECT_EQ(nullptr, collection->Get(55));
        EXPECT_TRUE(this->failCalled);
        EXPECT_FALSE(this->successCalled);
    }

    TEST_F(TriggerMissedApproachTest, ItDoesntTriggerUserIsNotTower)
    {
        std::vector<std::shared_ptr<ServiceProvision>> provisions;
        provisions.push_back(std::make_shared<ServiceProvision>(ServiceType::Tower, userAppCallsign));
        serviceProviders.SetProvidersForAirfield("EGKK", provisions);

        EXPECT_CALL(api, CreateMissedApproach(testing::_)).Times(0);

        EXPECT_CALL(windows, OpenMessageBox(testing::_, testing::_, testing::_)).Times(0);

        trigger.Trigger(mockFlightplan, mockRadarTarget, true, successFunction, failFunction);
        EXPECT_EQ(2, collection->Count());
        EXPECT_EQ(nullptr, collection->Get(55));
        EXPECT_TRUE(this->failCalled);
        EXPECT_FALSE(this->successCalled);
    }

    TEST_F(TriggerMissedApproachTest, ItDoesntTriggerNonUserProvidingTower)
    {
        std::vector<std::shared_ptr<ServiceProvision>> provisions;
        provisions.push_back(std::make_shared<ServiceProvision>(ServiceType::Tower, notUserTowerCallsign));
        serviceProviders.SetProvidersForAirfield("EGKK", provisions);

        EXPECT_CALL(api, CreateMissedApproach(testing::_)).Times(0);

        EXPECT_CALL(windows, OpenMessageBox(testing::_, testing::_, testing::_)).Times(0);

        trigger.Trigger(mockFlightplan, mockRadarTarget, true, successFunction, failFunction);
        EXPECT_EQ(2, collection->Count());
        EXPECT_EQ(nullptr, collection->Get(55));
        EXPECT_TRUE(this->failCalled);
        EXPECT_FALSE(this->successCalled);
    }

    TEST_F(TriggerMissedApproachTest, ItDoesntTriggerUserNotProvidingTower)
    {
        std::vector<std::shared_ptr<ServiceProvision>> provisions;
        provisions.push_back(std::make_shared<ServiceProvision>(ServiceType::Approach, userTowerCallsign));
        serviceProviders.SetProvidersForAirfield("EGKK", provisions);

        EXPECT_CALL(api, CreateMissedApproach(testing::_)).Times(0);

        EXPECT_CALL(windows, OpenMessageBox(testing::_, testing::_, testing::_)).Times(0);

        trigger.Trigger(mockFlightplan, mockRadarTarget, true, successFunction, failFunction);
        EXPECT_EQ(2, collection->Count());
        EXPECT_EQ(nullptr, collection->Get(55));
        EXPECT_TRUE(this->failCalled);
        EXPECT_FALSE(this->successCalled);
    }

    TEST_F(TriggerMissedApproachTest, ItDoesntTriggerUserNotActive)
    {
        serviceProviders.Flush();
        EXPECT_CALL(api, CreateMissedApproach(testing::_)).Times(0);

        EXPECT_CALL(windows, OpenMessageBox(testing::_, testing::_, testing::_)).Times(0);

        trigger.Trigger(mockFlightplan, mockRadarTarget, true, successFunction, failFunction);
        EXPECT_EQ(2, collection->Count());
        EXPECT_EQ(nullptr, collection->Get(55));
        EXPECT_TRUE(this->failCalled);
        EXPECT_FALSE(this->successCalled);
    }

    TEST_F(TriggerMissedApproachTest, ItDoesntTriggerAircraftTooFarFromDestination)
    {
        ON_CALL(mockFlightplan, GetDistanceToDestination()).WillByDefault(testing::Return(9.0));
        EXPECT_CALL(api, CreateMissedApproach(testing::_)).Times(0);

        EXPECT_CALL(windows, OpenMessageBox(testing::_, testing::_, testing::_)).Times(0);

        trigger.Trigger(mockFlightplan, mockRadarTarget, true, successFunction, failFunction);
        EXPECT_EQ(2, collection->Count());
        EXPECT_EQ(nullptr, collection->Get(55));
        EXPECT_TRUE(this->failCalled);
        EXPECT_FALSE(this->successCalled);
    }

    TEST_F(TriggerMissedApproachTest, ItDoesntTriggerAircraftTooHigh)
    {
        ON_CALL(mockRadarTarget, GetFlightLevel()).WillByDefault(testing::Return(9000));
        EXPECT_CALL(api, CreateMissedApproach(testing::_)).Times(0);

        EXPECT_CALL(windows, OpenMessageBox(testing::_, testing::_, testing::_)).Times(0);

        trigger.Trigger(mockFlightplan, mockRadarTarget, true, successFunction, failFunction);
        EXPECT_EQ(2, collection->Count());
        EXPECT_EQ(nullptr, collection->Get(55));
        EXPECT_TRUE(this->failCalled);
        EXPECT_FALSE(this->successCalled);
    }

    TEST_F(TriggerMissedApproachTest, ItDoesntTriggerAircraftTooSlow)
    {
        ON_CALL(mockRadarTarget, GetGroundSpeed()).WillByDefault(testing::Return(30));
        EXPECT_CALL(api, CreateMissedApproach(testing::_)).Times(0);

        EXPECT_CALL(windows, OpenMessageBox(testing::_, testing::_, testing::_)).Times(0);

        trigger.Trigger(mockFlightplan, mockRadarTarget, true, successFunction, failFunction);
        EXPECT_EQ(2, collection->Count());
        EXPECT_EQ(nullptr, collection->Get(55));
        EXPECT_TRUE(this->failCalled);
        EXPECT_FALSE(this->successCalled);
    }
} // namespace UKControllerPluginTest::MissedApproach
