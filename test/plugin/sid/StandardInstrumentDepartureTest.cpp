#include "pch/pch.h"
#include "sid/StandardInstrumentDeparture.h"

using ::testing::Test;
using UKControllerPlugin::Sid::StandardInstrumentDeparture;

namespace UKControllerPluginTest {
    namespace Sid {

        class StandardInstrumentDepartureTest : public Test
        {
            public:

                StandardInstrumentDepartureTest()
                    : sid("EGGD", "WOTAN1Z", 6000, 55)
                { }

                StandardInstrumentDeparture sid;
        };

        TEST_F(StandardInstrumentDepartureTest, ItHasAnAirfield)
        {
            EXPECT_EQ("EGGD", sid.Airfield());
        }

        TEST_F(StandardInstrumentDepartureTest, ItHasAnIdentifier)
        {
            EXPECT_EQ("WOTAN1Z", sid.Identifier());
        }

        TEST_F(StandardInstrumentDepartureTest, ItHasAnInitialAltitude)
        {
            EXPECT_EQ(6000, sid.InitialAltitude());
        }

        TEST_F(StandardInstrumentDepartureTest, ItHasAnInitialHeading)
        {
            EXPECT_EQ(55, sid.InitialHeading());
        }
    }  // namespace Sid
}  // namespace UKControllerPluginTest
