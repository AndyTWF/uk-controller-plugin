#include "pch/pch.h"
#include "airfield/Airfield.h"
#include "airfield/Runway.h"
#include "controller/ActiveCallsign.h"
#include "controller/ControllerPosition.h"

using UKControllerPlugin::Airfield::Airfield;
using UKControllerPlugin::Controller::ActiveCallsign;
using UKControllerPlugin::Controller::ControllerPosition;
using UKControllerPlugin::Airfield::Runway;
using ::testing::ElementsAre;

namespace UKControllerPluginTest {
    namespace AirfieldOwnership {

        TEST(Airfield, ConstructorThrowsExceptionIfIcaoTooLong)
        {
            EXPECT_THROW(Airfield("EGXYZ", {}), std::invalid_argument);
        }

        TEST(Airfield, ConstructorThrowsExceptionIfIcaoNotUk)
        {
            EXPECT_THROW(Airfield("LIML", {}), std::invalid_argument);
        }

        TEST(Airfield, GetIcaoReturnsIcao)
        {
            std::string icao = "EGKK";
            Airfield airfield(icao, {});
            EXPECT_EQ(0, icao.compare(airfield.GetIcao()));
        }

        TEST(Airfield, GetOwnershipPresedenceReturnsOwnership)
        {
            Airfield airfield("EGKK", { "EGKK_DEL", "EGKK_GND" });
            EXPECT_THAT(airfield.GetOwnershipPresedence(), ElementsAre("EGKK_DEL", "EGKK_GND"));
        }

        TEST(Airfield, EqualityOperatorReturnsFalseDifferentIcao)
        {
            Airfield airfield("EGKK", { "EGKK_DEL", "EGKK_GND" });
            Airfield airfield2("EGLL", { "EGKK_DEL", "EGKK_GND" });
            EXPECT_FALSE(airfield == airfield2);
        }

        TEST(Airfield, EqualityOperatorReturnsTrueSameIcao)
        {
            Airfield airfield("EGKK", { "EGKK_DEL", "EGKK_GND" });
            Airfield airfield2("EGKK", { "EGKK_DEL", "EGKK_GND" });
            EXPECT_TRUE(airfield == airfield2);
        }

        TEST(Airfield, GetRunwayReturnsNullPtrIfNotFound)
        {
            Airfield airfield("EGKK", {},
                { std::make_unique<Runway>("08R"), std::make_unique<Runway>("26L") }
            );
            airfield.AddRunway(std::make_unique<Runway>("08R"));
            airfield.AddRunway(std::make_unique<Runway>("26L"));
            EXPECT_EQ(nullptr, airfield.GetRunway("08L"));
        }

        TEST(Airfield, GetRunwayReturnsRunway)
        {
            Runway expectedRunway = { "08R" };
            Airfield airfield(
                "EGKK",
                {},
                { std::make_unique<Runway>("08R"), std::make_unique<Runway>("26L") }
            );
            EXPECT_EQ(expectedRunway, *airfield.GetRunway("08R"));
        }
    }  // namespace AirfieldOwnership
}  // namespace UKControllerPluginTest
