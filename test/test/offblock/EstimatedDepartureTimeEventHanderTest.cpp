#include "pch/pch.h"
#include "offblock/EstimatedDepartureTimeEventHandler.h"
#include "mock/MockEuroScopeCFlightplanInterface.h"
#include "mock/MockEuroScopeCRadarTargetInterface.h"
#include "flightplan/StoredFlightplanCollection.h"
#include "flightplan/StoredFlightplan.h"
#include "helper/HelperFunctions.h"

using ::testing::Test;
using ::testing::Return;
using ::testing::NiceMock;
using UKControllerPlugin::Flightplan::StoredFlightplan;
using UKControllerPlugin::Flightplan::StoredFlightplanCollection;
using UKControllerPlugin::Datablock::EstimatedDepartureTimeEventHandler;
using UKControllerPluginTest::Euroscope::MockEuroScopeCFlightPlanInterface;
using UKControllerPluginTest::Euroscope::MockEuroScopeCRadarTargetInterface;
using UKControllerPlugin::HelperFunctions;

namespace UKControllerPluginTest {
namespace Datablock {

class EstimatedDepartureTimeEventHandlerTest : public Test
{
    public:
        EstimatedDepartureTimeEventHandlerTest()
            : handler(this->flightplans)
        {
            ON_CALL(this->mockFlightplan, GetCallsign())
                .WillByDefault(Return("BAW123"));

            ON_CALL(this->mockFlightplan, GetExpectedDepartureTime())
                .WillByDefault(Return("1900"));
        }

        EstimatedDepartureTimeEventHandler handler;
        StoredFlightplanCollection flightplans;
        NiceMock<MockEuroScopeCFlightPlanInterface> mockFlightplan;
        NiceMock<MockEuroScopeCRadarTargetInterface> mockRadarTarget;
};

TEST_F(EstimatedDepartureTimeEventHandlerTest, TestItHasATagItemDescription)
{
    EXPECT_TRUE("Estimated Departure Time" == handler.GetTagItemDescription());
}

TEST_F(EstimatedDepartureTimeEventHandlerTest, TestItDoesntUpdateOnNoStoredPlan)
{
    ON_CALL(this->mockFlightplan, GetCallsign())
        .WillByDefault(Return("BAW123"));

    EXPECT_NO_THROW(this->handler.FlightPlanEvent(this->mockFlightplan, this->mockRadarTarget));
}

TEST_F(EstimatedDepartureTimeEventHandlerTest, TestItUpdatesEdtFromFlightplan)
{
    StoredFlightplan storedPlan(this->mockFlightplan);
    this->flightplans.UpdatePlan(storedPlan);

    this->handler.FlightPlanEvent(this->mockFlightplan, this->mockRadarTarget);
    EXPECT_NE(
        (std::chrono::system_clock::time_point::max)(),
        this->flightplans.GetFlightplanForCallsign("BAW123").GetEstimatedDepartureTime()
    );
    EXPECT_TRUE("19:00" == this->handler.GetTagItemData(this->mockFlightplan, this->mockRadarTarget));
}

TEST_F(EstimatedDepartureTimeEventHandlerTest, TestItReturnsEmptyOnNoStoredPlan)
{
    EXPECT_TRUE(this->handler.noTime == this->handler.GetTagItemData(this->mockFlightplan, this->mockRadarTarget));
}

TEST_F(EstimatedDepartureTimeEventHandlerTest, TestItReturnsEmptyOnNoEdt)
{
    StoredFlightplan storedPlan(this->mockFlightplan);
    this->flightplans.UpdatePlan(storedPlan);
    EXPECT_TRUE(this->handler.noTime == this->handler.GetTagItemData(this->mockFlightplan, this->mockRadarTarget));
}

TEST_F(EstimatedDepartureTimeEventHandlerTest, TestItReturnsEdt)
{
    StoredFlightplan storedPlan(this->mockFlightplan);
    storedPlan.SetEstimatedDepartureTime(HelperFunctions::GetTimeFromNumberString("1900"));
    this->flightplans.UpdatePlan(storedPlan);
    EXPECT_TRUE("19:00" == this->handler.GetTagItemData(this->mockFlightplan, this->mockRadarTarget));
}

}  // namespace Datablock
}  // namespace UKControllerPluginTest
