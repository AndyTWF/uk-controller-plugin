#include "controller/ControllerPosition.h"
#include "controller/ControllerPositionCollection.h"
#include "push/PushEvent.h"
#include "releases/DepartureReleaseRequest.h"
#include "releases/DepartureReleaseRequestCollection.h"
#include "releases/DepartureReleaseRequestEventHandlerCollection.h"
#include "releases/NewDepartureReleasePushEventHandler.h"
#include "time/ParseTimeStrings.h"

using UKControllerPlugin::Controller::ControllerPosition;
using UKControllerPlugin::Controller::ControllerPositionCollection;
using UKControllerPlugin::Push::PushEvent;
using UKControllerPlugin::Push::PushEventSubscription;
using UKControllerPlugin::Releases::DepartureReleaseRequestCollection;
using UKControllerPlugin::Releases::DepartureReleaseRequestEventHandlerCollection;
using UKControllerPlugin::Releases::NewDepartureReleasePushEventHandler;
using UKControllerPlugin::Time::ParseTimeString;

namespace UKControllerPluginTest::Releases {
    class NewDepartureReleasePushEventHandlerTest : public testing::Test
    {
        public:
        NewDepartureReleasePushEventHandlerTest()
            : releases(std::make_shared<DepartureReleaseRequestCollection>()),
              eventHandlers(std::make_shared<DepartureReleaseRequestEventHandlerCollection>()),
              handler(releases, eventHandlers, controllers)
        {
            controllers.AddPosition(std::make_shared<ControllerPosition>(
                1, "EGFF_APP", 125.850, std::vector<std::string>{"EGGD", "EGFF"}, true, true));

            controllers.AddPosition(std::make_shared<ControllerPosition>(
                2, "LON_W_CTR", 126.020, std::vector<std::string>{"EGGD", "EGFF"}, true, true));

            mockHandler = std::make_shared<testing::NiceMock<MockDepartureReleaseRequestEventHandlerInterface>>();
            eventHandlers->AddHandler(mockHandler);
        };

        /*
         * Make an event based on the merge of some base data and overriding data so we dont
         * have to repeat ourselves
         */
        static PushEvent MakePushEvent(
            const nlohmann::json& overridingData = nlohmann::json::object(), const std::string& keyToRemove = "")
        {
            nlohmann::json eventData{
                {"id", 1},
                {"callsign", "BAW123"},
                {"requesting_controller", 1},
                {"target_controller", 2},
                {"expires_at", "2021-05-12 19:55:00"}};
            eventData.update(overridingData);

            if (!keyToRemove.empty()) {
                eventData.erase(eventData.find(keyToRemove));
            }

            return {"departure_release.requested", "test", eventData, eventData.dump()};
        };

        std::shared_ptr<testing::NiceMock<MockDepartureReleaseRequestEventHandlerInterface>> mockHandler;
        ControllerPositionCollection controllers;
        std::shared_ptr<DepartureReleaseRequestCollection> releases;
        std::shared_ptr<DepartureReleaseRequestEventHandlerCollection> eventHandlers;
        NewDepartureReleasePushEventHandler handler;
    };

    TEST_F(NewDepartureReleasePushEventHandlerTest, ItHasSubscriptions)
    {
        auto subs =
            std::set<PushEventSubscription>{{PushEventSubscription::SUB_TYPE_EVENT, "departure_release.requested"}};

        EXPECT_EQ(subs, handler.GetPushEventSubscriptions());
    }

    TEST_F(NewDepartureReleasePushEventHandlerTest, ItAddsAReleaseRequestFromMessage)
    {
        EXPECT_CALL(*mockHandler, NewRelease(testing::_)).Times(1);

        handler.ProcessPushEvent(MakePushEvent());

        EXPECT_EQ(1, releases->CountRequests());
        auto release = releases->FindById(1);
        EXPECT_EQ(1, release->Id());
        EXPECT_EQ("BAW123", release->Callsign());
        EXPECT_EQ(controllers.FetchPositionById(1), release->RequestingController());
        EXPECT_EQ(controllers.FetchPositionById(2), release->TargetController());
        EXPECT_EQ(ParseTimeString("2021-05-12 19:55:00"), release->RequestExpiryTime());
    }

    TEST_F(NewDepartureReleasePushEventHandlerTest, ItDoesntAddARequestIfIdIsMissing)
    {
        EXPECT_CALL(*mockHandler, NewRelease(testing::_)).Times(0);

        handler.ProcessPushEvent(MakePushEvent(nlohmann::json::object(), "id"));
        EXPECT_EQ(0, releases->CountRequests());
    }

    TEST_F(NewDepartureReleasePushEventHandlerTest, ItDoesntAddARequestIfIdIsNotAnInteger)
    {
        EXPECT_CALL(*mockHandler, NewRelease(testing::_)).Times(0);

        handler.ProcessPushEvent(MakePushEvent(nlohmann::json::object({{"id", "abc"}})));
        EXPECT_EQ(0, releases->CountRequests());
    }

    TEST_F(NewDepartureReleasePushEventHandlerTest, ItDoesntAddARequestIfCallsignIsMissing)
    {
        EXPECT_CALL(*mockHandler, NewRelease(testing::_)).Times(0);

        handler.ProcessPushEvent(MakePushEvent(nlohmann::json::object(), "callsign"));
        EXPECT_EQ(0, releases->CountRequests());
    }

    TEST_F(NewDepartureReleasePushEventHandlerTest, ItDoesntAddARequestIfCallsignIsNotAString)
    {
        EXPECT_CALL(*mockHandler, NewRelease(testing::_)).Times(0);

        handler.ProcessPushEvent(MakePushEvent(nlohmann::json::object({{"callsign", 123}})));
        EXPECT_EQ(0, releases->CountRequests());
    }

    TEST_F(NewDepartureReleasePushEventHandlerTest, ItDoesntAddARequestIfRequestingControllerIsMissing)
    {
        EXPECT_CALL(*mockHandler, NewRelease(testing::_)).Times(0);

        handler.ProcessPushEvent(MakePushEvent(nlohmann::json::object(), "requesting_controller"));
        EXPECT_EQ(0, releases->CountRequests());
    }

    TEST_F(NewDepartureReleasePushEventHandlerTest, ItDoesntAddARequestIfRequestingControllerIsNotAnInteger)
    {
        EXPECT_CALL(*mockHandler, NewRelease(testing::_)).Times(0);

        handler.ProcessPushEvent(MakePushEvent(nlohmann::json::object({{"requesting_controller", "abc"}})));
        EXPECT_EQ(0, releases->CountRequests());
    }

    TEST_F(NewDepartureReleasePushEventHandlerTest, ItDoesntAddARequestIfRequestingControllerIsNotValid)
    {
        EXPECT_CALL(*mockHandler, NewRelease(testing::_)).Times(0);

        handler.ProcessPushEvent(MakePushEvent(nlohmann::json::object({{"requesting_controller", 999}})));
        EXPECT_EQ(0, releases->CountRequests());
    }

    TEST_F(NewDepartureReleasePushEventHandlerTest, ItDoesntAddARequestIfTargetControllerIsMissing)
    {
        EXPECT_CALL(*mockHandler, NewRelease(testing::_)).Times(0);

        handler.ProcessPushEvent(MakePushEvent(nlohmann::json::object(), "target_controller"));
        EXPECT_EQ(0, releases->CountRequests());
    }

    TEST_F(NewDepartureReleasePushEventHandlerTest, ItDoesntAddARequestIfTargetControllerIsNotAnInteger)
    {
        EXPECT_CALL(*mockHandler, NewRelease(testing::_)).Times(0);

        handler.ProcessPushEvent(MakePushEvent(nlohmann::json::object({{"target_controller", "abc"}})));
        EXPECT_EQ(0, releases->CountRequests());
    }

    TEST_F(NewDepartureReleasePushEventHandlerTest, ItDoesntAddARequestIfTargetControllerIsNotValid)
    {
        EXPECT_CALL(*mockHandler, NewRelease(testing::_)).Times(0);

        handler.ProcessPushEvent(MakePushEvent(nlohmann::json::object({{"target_controller", 999}})));
        EXPECT_EQ(0, releases->CountRequests());
    }

    TEST_F(NewDepartureReleasePushEventHandlerTest, ItDoesntAddARequestIfExpiryIsMissing)
    {
        EXPECT_CALL(*mockHandler, NewRelease(testing::_)).Times(0);

        handler.ProcessPushEvent(MakePushEvent(nlohmann::json::object(), "expires_at"));
        EXPECT_EQ(0, releases->CountRequests());
    }

    TEST_F(NewDepartureReleasePushEventHandlerTest, ItDoesntAddARequestIfExpiryIsNotAString)
    {
        EXPECT_CALL(*mockHandler, NewRelease(testing::_)).Times(0);

        handler.ProcessPushEvent(MakePushEvent(nlohmann::json::object({{"expires_at", 123}})));
        EXPECT_EQ(0, releases->CountRequests());
    }

    TEST_F(NewDepartureReleasePushEventHandlerTest, ItDoesntAddARequestIfExpiryIsNotAValidTimeString)
    {
        EXPECT_CALL(*mockHandler, NewRelease(testing::_)).Times(0);

        handler.ProcessPushEvent(MakePushEvent(nlohmann::json::object({{"expires_at", "abc"}})));
        EXPECT_EQ(0, releases->CountRequests());
    }
} // namespace UKControllerPluginTest::Releases
