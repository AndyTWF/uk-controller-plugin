#include "pch/pch.h"
#include "hold/HoldManager.h"
#include "hold/HoldEventHandler.h"
#include "mock/MockEuroscopePluginLoopbackInterface.h"
#include "mock/MockEuroScopeCFlightplanInterface.h"
#include "mock/MockEuroScopeCRadarTargetInterface.h"
#include "plugin/PopupMenuItem.h"
#include "hold/ManagedHold.h"
#include "hold/HoldingData.h"

using UKControllerPlugin::Hold::ManagedHold;
using UKControllerPlugin::Hold::HoldingData;
using UKControllerPlugin::Hold::HoldManager;
using UKControllerPluginTest::Euroscope::MockEuroscopePluginLoopbackInterface;
using UKControllerPluginTest::Euroscope::MockEuroScopeCFlightPlanInterface;
using UKControllerPluginTest::Euroscope::MockEuroScopeCRadarTargetInterface;
using UKControllerPlugin::Hold::HoldEventHandler;
using UKControllerPlugin::Plugin::PopupMenuItem;
using ::testing::Return;
using ::testing::NiceMock;
using ::testing::Test;

namespace UKControllerPluginTest {
    namespace Hold {

        class HoldEventHandlerTest : public Test
        {
            public:
                HoldEventHandlerTest(void)
                    : handler(
                        this->manager,
                        this->mockPlugin,
                        1
                    )
                {
                    manager.AddHold(ManagedHold(std::move(holdData)));

                    // Add a FP to the holds initially.
                    NiceMock<MockEuroScopeCFlightPlanInterface> mockFlightplanInitial;
                    NiceMock<MockEuroScopeCRadarTargetInterface> mockRadarTargetInitial;

                    ON_CALL(mockFlightplanInitial, GetCallsign())
                        .WillByDefault(Return("BAW123"));

                    ON_CALL(mockFlightplanInitial, GetClearedAltitude())
                        .WillByDefault(Return(8000));

                    ON_CALL(mockRadarTargetInitial, GetFlightLevel())
                        .WillByDefault(Return(9000));

                    this->manager.AddAircraftToHold(mockFlightplanInitial, mockRadarTargetInitial, 1);


                    this->mockFlightplan.reset(new NiceMock<MockEuroScopeCFlightPlanInterface>);
                    this->mockRadarTarget.reset(new NiceMock<MockEuroScopeCRadarTargetInterface>);

                    // On subsequent calls, we use this to update the fp
                    ON_CALL(*this->mockFlightplan, GetCallsign())
                        .WillByDefault(Return("BAW123"));

                    ON_CALL(*this->mockFlightplan, GetClearedAltitude())
                        .WillByDefault(Return(7000));

                    ON_CALL(*this->mockRadarTarget, GetFlightLevel())
                        .WillByDefault(Return(8000));

                    ON_CALL(mockPlugin, GetFlightplanForCallsign("BAW123"))
                        .WillByDefault(Return(this->mockFlightplan));

                    ON_CALL(mockPlugin, GetRadarTargetForCallsign("BAW123"))
                        .WillByDefault(Return(this->mockRadarTarget));
                }

                HoldingData holdData = { 1, "TIMBA", "TIMBA", 8000, 15000, 209, "left", {} };
                std::shared_ptr<NiceMock<MockEuroScopeCFlightPlanInterface>> mockFlightplan;
                std::shared_ptr<NiceMock<MockEuroScopeCRadarTargetInterface>> mockRadarTarget;
                NiceMock<MockEuroscopePluginLoopbackInterface> mockPlugin;
                HoldManager manager;
                HoldEventHandler handler;
        };

        TEST_F(HoldEventHandlerTest, ItRemovesAnAircraftFromTheHoldIfTheFlightplanDisconnects)
        {
            EXPECT_TRUE(this->manager.GetManagedHold(1)->HasAircraft("BAW123"));
            this->handler.FlightPlanDisconnectEvent(*this->mockFlightplan);
            EXPECT_FALSE(this->manager.GetManagedHold(1)->HasAircraft("BAW123"));
        }

        TEST_F(HoldEventHandlerTest, TimedEventTriggersDataUpdate)
        {
            EXPECT_EQ(8000, this->manager.GetManagedHold(1)->cbegin()->clearedLevel);
            EXPECT_EQ(9000, this->manager.GetManagedHold(1)->cbegin()->reportedLevel);
            this->manager.UpdateHoldingAircraft(this->mockPlugin);
            EXPECT_EQ(7000, this->manager.GetManagedHold(1)->cbegin()->clearedLevel);
            EXPECT_EQ(8000, this->manager.GetManagedHold(1)->cbegin()->reportedLevel);
        }

        TEST_F(HoldEventHandlerTest, ItHasATagItemDescription)
        {
            EXPECT_TRUE("Selected Hold" == this->handler.GetTagItemDescription());
        }

        TEST_F(HoldEventHandlerTest, ItReturnsTheSelectedHoldForAnAircraft)
        {
            EXPECT_TRUE("HTIMBA" == this->handler.GetTagItemData(*this->mockFlightplan, *this->mockRadarTarget));
        }

        TEST_F(HoldEventHandlerTest, ItReturnsNoHoldIfAircraftNotInHold)
        {
            this->manager.RemoveAircraftFromAnyHold("BAW123");
            EXPECT_TRUE(
                this->handler.noHold == this->handler.GetTagItemData(*this->mockFlightplan, *this->mockRadarTarget)
            );
        }
    }  // namespace Hold
}  // namespace UKControllerPluginTest
