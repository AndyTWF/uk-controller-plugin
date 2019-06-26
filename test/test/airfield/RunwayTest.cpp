#include "pch/pch.h"
#include "airfield/Runway.h"

using UKControllerPlugin::Airfield::Runway;

namespace UKControllerPluginTest {
    namespace Airfield {

        TEST(RunwayTest, EqualityReturnsTrueIfIdentifierSame)
        {
            Runway runway1 = { "09L", 15, true, true };
            Runway runway2 = { "09L", 16, false, false };
            EXPECT_TRUE(runway1 == runway2);
        }

        TEST(RunwayTest, EqualityReturnsFalseIfIdentifierDifferent)
        {
            Runway runway1 = { "09L", 15, true, true };
            Runway runway2 = { "09R", 15, true, true };
            EXPECT_FALSE(runway1 == runway2);
        }
    }  // namespace Airfield
}  // namespace UKControllerPluginTest
