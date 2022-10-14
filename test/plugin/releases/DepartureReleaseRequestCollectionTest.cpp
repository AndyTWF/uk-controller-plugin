#include "controller/ControllerPosition.h"
#include "releases/DepartureReleaseRequest.h"
#include "releases/DepartureReleaseRequestCollection.h"
#include "time/SystemClock.h"

using UKControllerPlugin::Controller::ControllerPosition;
using UKControllerPlugin::Releases::DepartureReleaseRequest;
using UKControllerPlugin::Releases::DepartureReleaseRequestCollection;
using UKControllerPlugin::Time::TimeNow;

namespace UKControllerPluginTest::Releases {
    class DepartureReleaseRequestCollectionTest : public ::testing::Test
    {
        public:
        DepartureReleaseRequestCollectionTest()
        {
            sendingPosition = std::make_shared<ControllerPosition>(
                1, "EGKK_TWR", 124.225, std::vector<std::string>{"EGKK"}, true, false);
            receivingPosition = std::make_shared<ControllerPosition>(
                2, "EGKK_F_APP", 124.225, std::vector<std::string>{"EGKK"}, true, false);
            request1 = std::make_shared<DepartureReleaseRequest>(1, "BAW123", 1, 2, TimeNow());
            request2 = std::make_shared<DepartureReleaseRequest>(5, "BAW456", 1, 2, TimeNow());
            request3 = std::make_shared<DepartureReleaseRequest>(3, "BAW456", 1, 2, TimeNow());
        }

        std::shared_ptr<ControllerPosition> sendingPosition;
        std::shared_ptr<ControllerPosition> receivingPosition;
        DepartureReleaseRequestCollection collection;
        std::shared_ptr<DepartureReleaseRequest> request1;
        std::shared_ptr<DepartureReleaseRequest> request2;
        std::shared_ptr<DepartureReleaseRequest> request3;
    };

    TEST_F(DepartureReleaseRequestCollectionTest, ItStartsEmpty)
    {
        EXPECT_EQ(0, collection.CountRequests());
    }

    TEST_F(DepartureReleaseRequestCollectionTest, ItAddsMessages)
    {
        collection.Add(request1);
        collection.Add(request2);
        EXPECT_EQ(2, collection.CountRequests());
        EXPECT_EQ(request1, collection.FindById(1));
        EXPECT_EQ(request2, collection.FindById(5));
    }

    TEST_F(DepartureReleaseRequestCollectionTest, ItDoesntAddDuplicateMessages)
    {
        collection.Add(request1);
        collection.Add(request1);
        collection.Add(request1);
        collection.Add(request2);
        collection.Add(request2);
        collection.Add(request2);
        EXPECT_EQ(2, collection.CountRequests());
    }

    TEST_F(DepartureReleaseRequestCollectionTest, ItReturnsNullptrIfNoMessageFound)
    {
        collection.Add(request1);
        collection.Add(request2);
        EXPECT_EQ(nullptr, collection.FindById(999));
    }

    TEST_F(DepartureReleaseRequestCollectionTest, ItRemovesMessages)
    {
        collection.Add(request1);
        collection.Add(request2);
        collection.Remove(1);
        collection.Remove(5);
        EXPECT_EQ(0, collection.CountRequests());
        EXPECT_EQ(nullptr, collection.FindById(1));
        EXPECT_EQ(nullptr, collection.FindById(5));
    }

    TEST_F(DepartureReleaseRequestCollectionTest, ItHandlesRemovingNonExistentMessages)
    {
        collection.Add(request1);
        collection.Remove(5);
    }

    TEST_F(DepartureReleaseRequestCollectionTest, ItReturnsNullptrIfItDoesntFindRequestByPredicate)
    {
        collection.Add(request1);
        collection.Add(request2);
        collection.Add(request3);
        const auto foundRequest = collection.FirstWhere(
            [](const std::shared_ptr<DepartureReleaseRequest>& request) { return request->Callsign() == "BAW999"; });
        EXPECT_EQ(nullptr, foundRequest);
    }

    TEST_F(DepartureReleaseRequestCollectionTest, ItFindsTheFirstRequestByPredicate)
    {
        collection.Add(request1);
        collection.Add(request2);
        collection.Add(request3);
        const auto foundRequest = collection.FirstWhere(
            [](const std::shared_ptr<DepartureReleaseRequest>& request) { return request->Callsign() == "BAW456"; });
        EXPECT_EQ(request2, foundRequest);
    }

    TEST_F(DepartureReleaseRequestCollectionTest, ItIsIterable)
    {
        collection.Add(request1);
        collection.Add(request2);
        std::vector<int> ids;
        collection.Iterate(
            [&ids](const std::shared_ptr<DepartureReleaseRequest>& request) { ids.push_back(request->Id()); });
        EXPECT_EQ(std::vector<int>({1, 5}), ids);
    }

    TEST_F(DepartureReleaseRequestCollectionTest, ItemsCanBeRemovedByPredicate)
    {
        collection.Add(request1);
        collection.Add(request2);
        collection.RemoveWhere([](const DepartureReleaseRequest& request) { return request.Id() == 1; });
        EXPECT_EQ(1, collection.CountRequests());
        EXPECT_EQ(request2, collection.FindById(5));
        EXPECT_EQ(nullptr, collection.FindById(1));
        collection.RemoveWhere([](const DepartureReleaseRequest& request) { return request.Id() == 5; });
        EXPECT_EQ(0, collection.CountRequests());
        EXPECT_EQ(nullptr, collection.FindById(5));
    }

    TEST_F(DepartureReleaseRequestCollectionTest, ItemsRemovedByPredicateArePassedToCallback)
    {
        std::vector<int> requestIds;
        collection.Add(request1);
        collection.Add(request2);
        collection.Add(request3);
        collection.RemoveWhere(
            [](const DepartureReleaseRequest& request) { return request.Callsign() == "BAW456"; },
            [&requestIds](const DepartureReleaseRequest& request) { requestIds.push_back(request.Id()); });

        EXPECT_EQ(1, collection.CountRequests());
        EXPECT_EQ(std::vector<int>({5, 3}), requestIds);
    }
} // namespace UKControllerPluginTest::Releases
