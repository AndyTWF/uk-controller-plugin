#include "controller/ControllerPosition.h"
#include "releases/CompareDepartureReleases.h"
#include "releases/DepartureReleaseRequest.h"

using ::testing::Test;
using UKControllerPlugin::Controller::ControllerPosition;
using UKControllerPlugin::Releases::CompareDepartureReleases;
using UKControllerPlugin::Releases::DepartureReleaseRequest;

namespace UKControllerPluginTest::Releases {

    class CompareDepartureReleasesTest : public Test
    {
        public:
        CompareDepartureReleasesTest()
        {
            const auto controller = std::make_shared<ControllerPosition>(
                1, "EGKK_TWR", 124.225, std::vector<std::string>{"EGKK"}, true, false);
            request1 = std::make_shared<DepartureReleaseRequest>(
                1, "BAW123", controller, controller, std::chrono::system_clock::now());
            request2 = std::make_shared<DepartureReleaseRequest>(
                2, "BAW456", controller, controller, std::chrono::system_clock::now());
        }

        std::shared_ptr<DepartureReleaseRequest> request1;
        std::shared_ptr<DepartureReleaseRequest> request2;
        CompareDepartureReleases compare;
    };

    TEST_F(CompareDepartureReleasesTest, LessThanIntReturnsTrueIfLessThan)
    {
        EXPECT_TRUE(compare(request1, 2));
    }

    TEST_F(CompareDepartureReleasesTest, LessThanClassReturnsTrueIfLessThan)
    {
        EXPECT_TRUE(compare(0, request1));
    }

    TEST_F(CompareDepartureReleasesTest, CompareReturnsTrueIfFirstLessThanLast)
    {
        EXPECT_TRUE(compare(request1, request2));
    }
} // namespace UKControllerPluginTest::Releases
