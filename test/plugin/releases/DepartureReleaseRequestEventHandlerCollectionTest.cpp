#include "controller/ControllerPosition.h"
#include "releases/DepartureReleaseRequest.h"
#include "releases/DepartureReleaseRequestEventHandlerCollection.h"

using UKControllerPlugin::Controller::ControllerPosition;
using UKControllerPlugin::Releases::DepartureReleaseRequest;
using UKControllerPlugin::Releases::DepartureReleaseRequestEventHandlerCollection;

namespace UKControllerPluginTest::Releases {
    class DepartureReleaseRequestEventHandlerCollectionTest : public testing::Test
    {
        public:
        DepartureReleaseRequestEventHandlerCollectionTest()
        {
            controller = std::make_shared<ControllerPosition>(
                2, "EGFF_APP", 125.850, std::vector<std::string>{"EGGD", "EGFF"}, true, true);
        }
        std::shared_ptr<ControllerPosition> controller;
        DepartureReleaseRequestEventHandlerCollection collection;
    };

    TEST_F(DepartureReleaseRequestEventHandlerCollectionTest, ItStartsWithNoHandlers)
    {
        EXPECT_EQ(0, collection.CountHandlers());
    }

    TEST_F(DepartureReleaseRequestEventHandlerCollectionTest, ItAddsHandlers)
    {
        auto handler1 = std::make_shared<testing::NiceMock<MockDepartureReleaseRequestEventHandlerInterface>>();
        auto handler2 = std::make_shared<testing::NiceMock<MockDepartureReleaseRequestEventHandlerInterface>>();
        collection.AddHandler(handler1);
        collection.AddHandler(handler2);
        EXPECT_EQ(2, collection.CountHandlers());
    }

    TEST_F(DepartureReleaseRequestEventHandlerCollectionTest, ItDoesntAddDuplicateHandlers)
    {
        auto handler1 = std::make_shared<testing::NiceMock<MockDepartureReleaseRequestEventHandlerInterface>>();
        auto handler2 = std::make_shared<testing::NiceMock<MockDepartureReleaseRequestEventHandlerInterface>>();
        collection.AddHandler(handler1);
        collection.AddHandler(handler1);
        collection.AddHandler(handler1);
        collection.AddHandler(handler2);
        collection.AddHandler(handler2);
        collection.AddHandler(handler2);
        EXPECT_EQ(2, collection.CountHandlers());
    }

    TEST_F(DepartureReleaseRequestEventHandlerCollectionTest, ItProcessesNewMessageEvent)
    {
        auto handler1 = std::make_shared<testing::NiceMock<MockDepartureReleaseRequestEventHandlerInterface>>();
        auto handler2 = std::make_shared<testing::NiceMock<MockDepartureReleaseRequestEventHandlerInterface>>();
        collection.AddHandler(handler1);
        collection.AddHandler(handler2);

        const DepartureReleaseRequest release(1, "BAW123", controller, controller, std::chrono::system_clock::now());

        EXPECT_CALL(*handler1, NewRelease(testing::Ref(release))).Times(1);

        EXPECT_CALL(*handler2, NewRelease(testing::Ref(release))).Times(1);

        collection.NewRelease(release);
    }

    TEST_F(DepartureReleaseRequestEventHandlerCollectionTest, ItProcessesCancelledMessageEvent)
    {
        auto handler1 = std::make_shared<testing::NiceMock<MockDepartureReleaseRequestEventHandlerInterface>>();
        auto handler2 = std::make_shared<testing::NiceMock<MockDepartureReleaseRequestEventHandlerInterface>>();
        collection.AddHandler(handler1);
        collection.AddHandler(handler2);

        const DepartureReleaseRequest release(1, "BAW123", controller, controller, std::chrono::system_clock::now());

        EXPECT_CALL(*handler1, ReleaseCancelled(testing::Ref(release))).Times(1);

        EXPECT_CALL(*handler2, ReleaseCancelled(testing::Ref(release))).Times(1);

        collection.ReleaseCancelled(release);
    }

    TEST_F(DepartureReleaseRequestEventHandlerCollectionTest, ItProcessesAcknowledgedMessageEvent)
    {
        auto handler1 = std::make_shared<testing::NiceMock<MockDepartureReleaseRequestEventHandlerInterface>>();
        auto handler2 = std::make_shared<testing::NiceMock<MockDepartureReleaseRequestEventHandlerInterface>>();
        collection.AddHandler(handler1);
        collection.AddHandler(handler2);

        const DepartureReleaseRequest release(1, "BAW123", controller, controller, std::chrono::system_clock::now());

        EXPECT_CALL(*handler1, ReleaseAcknowledged(testing::Ref(release))).Times(1);

        EXPECT_CALL(*handler2, ReleaseAcknowledged(testing::Ref(release))).Times(1);

        collection.ReleaseAcknowledged(release);
    }

    TEST_F(DepartureReleaseRequestEventHandlerCollectionTest, ItProcessesTimeoutMessageEvent)
    {
        auto handler1 = std::make_shared<testing::NiceMock<MockDepartureReleaseRequestEventHandlerInterface>>();
        auto handler2 = std::make_shared<testing::NiceMock<MockDepartureReleaseRequestEventHandlerInterface>>();
        collection.AddHandler(handler1);
        collection.AddHandler(handler2);

        const DepartureReleaseRequest release(1, "BAW123", controller, controller, std::chrono::system_clock::now());

        EXPECT_CALL(*handler1, ReleaseTimeout(testing::Ref(release))).Times(1);

        EXPECT_CALL(*handler2, ReleaseTimeout(testing::Ref(release))).Times(1);

        collection.ReleaseTimeout(release);
    }
} // namespace UKControllerPluginTest::Releases
