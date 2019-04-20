#include "pch/pch.h"
#include "hold/HoldManager.h"
#include "mock/MockEuroscopePluginLoopbackInterface.h"
#include "mock/MockEuroScopeCFlightplanInterface.h"
#include "mock/MockEuroScopeCRadarTargetInterface.h"
#include "hold/ManagedHold.h"
#include "hold/HoldingData.h"
#include "hold/HoldingAircraft.h"

using UKControllerPlugin::Hold::HoldManager;
using UKControllerPluginTest::Euroscope::MockEuroscopePluginLoopbackInterface;
using UKControllerPluginTest::Euroscope::MockEuroScopeCFlightPlanInterface;
using UKControllerPluginTest::Euroscope::MockEuroScopeCRadarTargetInterface;
using UKControllerPlugin::Hold::ManagedHold;
using UKControllerPlugin::Hold::HoldingData;
using UKControllerPlugin::Hold::HoldingAircraft;
using ::testing::Return;
using ::testing::NiceMock;
using ::testing::Test;

namespace UKControllerPluginTest {
    namespace Hold {

        class HoldManagerTest : public Test
        {
            public:
                HoldManagerTest(void)
                {
                    manager.AddHold(ManagedHold(std::move(hold1)));

                    ON_CALL(mockFlightplan, GetCallsign())
                        .WillByDefault(Return("BAW123"));

                    ON_CALL(mockFlightplan, GetClearedAltitude())
                        .WillByDefault(Return(8000));

                    ON_CALL(mockRadarTarget, GetFlightLevel())
                        .WillByDefault(Return(9000));

                    ON_CALL(mockRadarTarget, GetVerticalSpeed())
                        .WillByDefault(Return(300));
                }

                HoldingData hold1 = { 1, "WILLO", "WILLO", 8000, 15000, 209, "left", {} };
                NiceMock<MockEuroScopeCFlightPlanInterface> mockFlightplan;
                NiceMock<MockEuroScopeCRadarTargetInterface> mockRadarTarget;
                NiceMock<MockEuroscopePluginLoopbackInterface> mockPlugin;
                HoldManager manager;
        };


        TEST_F(HoldManagerTest, ItStartsEmpty)
        {
            EXPECT_EQ(1, this->manager.CountHolds());
        }

        TEST_F(HoldManagerTest, ItAddsAHold)
        {
            HoldingData hold2 = { 2, "TIMBA", "TIMBA", 8000, 15000, 209, "left", {} };
            this->manager.AddHold(ManagedHold(std::move(hold2)));
            EXPECT_EQ(2, this->manager.CountHolds());
        }

        TEST_F(HoldManagerTest, ItHandlesDuplicateHolds)
        {
            HoldingData duplicate = { 1, "WILLO", "WILLO", 8000, 15000, 209, "left", {} };
            this->manager.AddHold(ManagedHold(std::move(duplicate)));
            EXPECT_EQ(1, this->manager.CountHolds());
        }

        TEST_F(HoldManagerTest, ItAddsAircraftToHold)
        {
            this->manager.AddAircraftToHold(mockFlightplan, mockRadarTarget, 1);
            EXPECT_TRUE(this->manager.GetManagedHold(1)->HasAircraft("BAW123"));
            EXPECT_EQ(1, this->manager.GetManagedHold(1)->CountHoldingAircraft());
        }

        TEST_F(HoldManagerTest, AddingAircraftToAHoldSetsData)
        {
            this->manager.AddAircraftToHold(mockFlightplan, mockRadarTarget, 1);
            HoldingAircraft holding = *this->manager.GetManagedHold(1)->cbegin();

            int64_t seconds = std::chrono::duration_cast<std::chrono::seconds> (
                holding.entryTime - std::chrono::system_clock::now()
            ).count();

            EXPECT_TRUE(holding.callsign == "BAW123");
            EXPECT_EQ(8000, holding.clearedLevel);
            EXPECT_EQ(9000, holding.reportedLevel);
            EXPECT_EQ(300, holding.verticalSpeed);
            EXPECT_LT(seconds, 2);
        }

        TEST_F(HoldManagerTest, AddingAircraftToOneHoldRemovesFromAnother)
        {
            HoldingData hold2 = { 2, "TIMBA", "TIMBA", 8000, 15000, 209, "left" };
            this->manager.AddHold(ManagedHold(std::move(hold2)));
            this->manager.AddAircraftToHold(mockFlightplan, mockRadarTarget, 1);
            EXPECT_TRUE(this->manager.GetManagedHold(1)->HasAircraft("BAW123"));
            this->manager.AddAircraftToHold(mockFlightplan, mockRadarTarget, 2);
            EXPECT_FALSE(this->manager.GetManagedHold(1)->HasAircraft("BAW123"));
            EXPECT_TRUE(this->manager.GetManagedHold(2)->HasAircraft("BAW123"));
        }

        TEST_F(HoldManagerTest, ItHandlesAddingToNonExistantHold)
        {
            EXPECT_NO_THROW(this->manager.AddAircraftToHold(mockFlightplan, mockRadarTarget, 555));
        }

        TEST_F(HoldManagerTest, GetHoldReturnsNullPointerIfDoesntExist)
        {
            EXPECT_EQ(nullptr, this->manager.GetManagedHold(555));
        }

        TEST_F(HoldManagerTest, ItRemovesAnAircraftFromAnyHold)
        {
            HoldingData hold2 = { 2, "TIMBA", "TIMBA", 8000, 15000, 209, "left" };
            this->manager.AddHold(ManagedHold(std::move(hold2)));
            manager.AddAircraftToHold(mockFlightplan, mockRadarTarget, 1);
            EXPECT_TRUE(this->manager.GetManagedHold(1)->HasAircraft("BAW123"));
            manager.RemoveAircraftFromAnyHold("BAW123");
            EXPECT_FALSE(this->manager.GetManagedHold(1)->HasAircraft("BAW123"));
        }

        TEST_F(HoldManagerTest, ItHandlesRemovingNonExistantAircraft)
        {
            EXPECT_NO_THROW(manager.RemoveAircraftFromAnyHold("BAW123"));
        }

        TEST_F(HoldManagerTest, ItUpdatesHoldingAircraftData)
        {
            this->manager.AddAircraftToHold(mockFlightplan, mockRadarTarget, 1);
            std::shared_ptr<NiceMock<MockEuroScopeCFlightPlanInterface>> updatedmockFp(
                new NiceMock<MockEuroScopeCFlightPlanInterface>
            );

            std::shared_ptr<NiceMock<MockEuroScopeCRadarTargetInterface>> updatedmockRt(
                new NiceMock<MockEuroScopeCRadarTargetInterface>
            );

            ON_CALL(*updatedmockFp, GetClearedAltitude())
                .WillByDefault(Return(7000));

            ON_CALL(*updatedmockRt, GetFlightLevel())
                .WillByDefault(Return(8000));

            ON_CALL(*updatedmockRt, GetVerticalSpeed())
                .WillByDefault(Return(-250));

            ON_CALL(mockPlugin, GetFlightplanForCallsign("BAW123"))
                .WillByDefault(Return(updatedmockFp));

            ON_CALL(mockPlugin, GetRadarTargetForCallsign("BAW123"))
                .WillByDefault(Return(updatedmockRt));

            manager.UpdateHoldingAircraft(this->mockPlugin);
            EXPECT_EQ(7000, manager.GetManagedHold(1)->cbegin()->clearedLevel);
            EXPECT_EQ(8000, manager.GetManagedHold(1)->cbegin()->reportedLevel);
            EXPECT_EQ(-250, manager.GetManagedHold(1)->cbegin()->verticalSpeed);
        }

        TEST_F(HoldManagerTest, ItDoesNothingIfNoUpdatesRequired)
        {
            EXPECT_NO_THROW(manager.UpdateHoldingAircraft(this->mockPlugin));
        }

        TEST_F(HoldManagerTest, ItGetsAHoldForAGivenAircraft)
        {
            this->manager.AddAircraftToHold(mockFlightplan, mockRadarTarget, 1);
            EXPECT_TRUE(this->hold1 == this->manager.GetAircraftHold("BAW123")->GetHoldParameters());
        }

        TEST_F(HoldManagerTest, ItReturnsNullIfAircraftDoesntHaveAHold)
        {
            EXPECT_EQ(NULL, this->manager.GetAircraftHold("BAW123"));
        }
    }  // namespace Hold
}  // namespace UKControllerPluginTest
