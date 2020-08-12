#include "pch/pch.h"
#include "releases/EnrouteReleaseEventHandler.h"
#include "mock/MockApiInterface.h"
#include "websocket/WebsocketSubscription.h"
#include "releases/EnrouteReleaseType.h"
#include "releases/EnrouteRelease.h"
#include "releases/CompareEnrouteReleaseTypes.h"
#include "websocket/WebsocketMessage.h"
#include "mock/MockEuroScopeCFlightplanInterface.h"
#include "mock/MockEuroScopeCRadarTargetInterface.h"
#include "tag/TagData.h"

using ::testing::Test;
using ::testing::NiceMock;
using ::testing::Return;
using UKControllerPlugin::Releases::EnrouteReleaseEventHandler;
using UKControllerPlugin::Websocket::WebsocketSubscription;
using UKControllerPluginTest::Api::MockApiInterface;
using UKControllerPluginTest::Euroscope::MockEuroScopeCFlightPlanInterface;
using UKControllerPluginTest::Euroscope::MockEuroScopeCRadarTargetInterface;
using UKControllerPlugin::Releases::CompareEnrouteReleaseTypes;
using UKControllerPlugin::Releases::EnrouteReleaseType;
using UKControllerPlugin::Releases::EnrouteRelease;
using UKControllerPlugin::Websocket::WebsocketMessage;
using UKControllerPlugin::Tag::TagData;

namespace UKControllerPluginTest {
    namespace Releases {

        class EnrouteReleaseEventHandlerTest : public Test
        {
            public:
                EnrouteReleaseEventHandlerTest()
                    : releases({ { 1, "test1", "testdesc1" }, { 2, "test2", "testdesc2" } }),
                    handler(mockApi, releases)
                {

                }

                double fontSize = 24.1;
                COLORREF tagColour = RGB(255, 255, 255);
                int euroscopeColourCode = EuroScopePlugIn::TAG_COLOR_ASSUMED;
                char itemString[16] = "Foooooo";
                NiceMock<MockEuroScopeCFlightPlanInterface> flightplan;
                NiceMock< MockEuroScopeCRadarTargetInterface> radarTarget;
                std::set<EnrouteReleaseType, CompareEnrouteReleaseTypes> releases;
                NiceMock<MockApiInterface> mockApi;
                EnrouteReleaseEventHandler handler;
        };

        TEST_F(EnrouteReleaseEventHandlerTest, ItSubscribesToChannels)
        {
            std::set<WebsocketSubscription> expectedSubscriptions;
            expectedSubscriptions.insert(
                {
                    WebsocketSubscription::SUB_TYPE_CHANNEL,
                    "private-enroute-releases"
                }
            );
            EXPECT_EQ(expectedSubscriptions, this->handler.GetSubscriptions());
        }

        TEST_F(EnrouteReleaseEventHandlerTest, ItChecksIfTheMessageIsValid)
        {
            nlohmann::json data{
                {"callsign", "BAW123"},
                {"type", 1},
                {"release_point", "foo"},
                {"initiating_controller", "LON_S_CTR"},
                {"target_controller", "LON_C_CTR"}
            };
            EXPECT_TRUE(this->handler.ReleaseMessageValid(data));
        }

        TEST_F(EnrouteReleaseEventHandlerTest, ItChecksIfTheMessageIsValidWhenReleasePointIsNull)
        {
            nlohmann::json data{
                {"callsign", "BAW123"},
                {"type", 1},
                {"release_point", nlohmann::json::value_t::null},
                {"initiating_controller", "LON_S_CTR"},
                {"target_controller", "LON_C_CTR"}
            };
            EXPECT_TRUE(this->handler.ReleaseMessageValid(data));
        }

        TEST_F(EnrouteReleaseEventHandlerTest, MessageIsInvalidIfMissingCallsign)
        {
            nlohmann::json data{
                {"type", 1},
                {"release_point", "foo"},
                {"initiating_controller", "LON_S_CTR"},
                {"target_controller", "LON_C_CTR"}
            };
            EXPECT_FALSE(this->handler.ReleaseMessageValid(data));
        }

        TEST_F(EnrouteReleaseEventHandlerTest, MessageIsInvalidIfCallsignInvalid)
        {
            nlohmann::json data{
                {"callsign", 1},
                {"type", 1},
                {"release_point", "foo"},
                {"initiating_controller", "LON_S_CTR"},
                {"target_controller", "LON_C_CTR"}
            };
            EXPECT_FALSE(this->handler.ReleaseMessageValid(data));
        }

        TEST_F(EnrouteReleaseEventHandlerTest, MessageIsInvalidIfMissingType)
        {
            nlohmann::json data{
                {"callsign", "BAW123"},
                {"release_point", "foo"},
                {"initiating_controller", "LON_S_CTR"},
                {"target_controller", "LON_C_CTR"}
            };
            EXPECT_FALSE(this->handler.ReleaseMessageValid(data));
        }

        TEST_F(EnrouteReleaseEventHandlerTest, MessageIsInvalidIfTypeInvalid)
        {
            nlohmann::json data{
                {"callsign", "BAW123"},
                {"type", "abc"},
                {"release_point", "foo"},
                {"initiating_controller", "LON_S_CTR"},
                {"target_controller", "LON_C_CTR"}
            };
            EXPECT_FALSE(this->handler.ReleaseMessageValid(data));
        }

        TEST_F(EnrouteReleaseEventHandlerTest, MessageIsInvalidTypeUnknown)
        {
            nlohmann::json data{
                {"callsign", "BAW123"},
                {"type", 3},
                {"release_point", "foo"},
                {"initiating_controller", "LON_S_CTR"},
                {"target_controller", "LON_C_CTR"}
            };
            EXPECT_FALSE(this->handler.ReleaseMessageValid(data));
        }

        TEST_F(EnrouteReleaseEventHandlerTest, MessageIsInvalidIfMissingReleasePoint)
        {
            nlohmann::json data{
                {"callsign", "BAW123"},
                {"type", 1},
                {"initiating_controller", "LON_S_CTR"},
                {"target_controller", "LON_C_CTR"}
            };
            EXPECT_FALSE(this->handler.ReleaseMessageValid(data));
        }

        TEST_F(EnrouteReleaseEventHandlerTest, MessageIsInvalidIfReleasePointInvalid)
        {
            nlohmann::json data{
                {"callsign", "BAW123"},
                {"type", 1},
                {"release_point", 1},
                {"initiating_controller", "LON_S_CTR"},
                {"target_controller", "LON_C_CTR"}
            };
            EXPECT_FALSE(this->handler.ReleaseMessageValid(data));
        }

        TEST_F(EnrouteReleaseEventHandlerTest, MessageIsInvalidIfMissingInitiatingController)
        {
            nlohmann::json data{
                {"callsign", "BAW123"},
                {"type", 1},
                {"release_point", "foo"},
                {"target_controller", "LON_C_CTR"}
            };
            EXPECT_FALSE(this->handler.ReleaseMessageValid(data));
        }

        TEST_F(EnrouteReleaseEventHandlerTest, MessageIsInvalidIfInitiatingControllerInvalid)
        {
            nlohmann::json data{
                {"callsign", "BAW123"},
                {"type", 1},
                {"release_point", "foo"},
                {"initiating_controller", 1},
                {"target_controller", "LON_C_CTR"}
            };
            EXPECT_FALSE(this->handler.ReleaseMessageValid(data));
        }

        TEST_F(EnrouteReleaseEventHandlerTest, MessageIsInvalidIfMissingTargetController)
        {
            nlohmann::json data{
                {"callsign", "BAW123"},
                {"type", 1},
                {"release_point", "foo"},
                {"initiating_controller", "LON_S_CTR"},
            };
            EXPECT_FALSE(this->handler.ReleaseMessageValid(data));
        }

        TEST_F(EnrouteReleaseEventHandlerTest, MessageIsInvalidIfTargetControllerInvalid)
        {
            nlohmann::json data{
                {"callsign", "BAW123"},
                {"type", 1},
                {"release_point", "foo"},
                {"initiating_controller", "LON_S_CTR"},
                {"target_controller", 1}
            };
            EXPECT_FALSE(this->handler.ReleaseMessageValid(data));
        }

        TEST_F(EnrouteReleaseEventHandlerTest, ItHasATagItemDescriptionForReleaseType)
        {
            EXPECT_EQ(
                "Enroute Release Type",
                this->handler.GetTagItemDescription(this->handler.enrouteReleaseTypeTagItemId)
            );
        }

        TEST_F(EnrouteReleaseEventHandlerTest, ItHasATagItemDescriptionForReleasePoint)
        {
            EXPECT_EQ(
                "Enroute Release Point",
                this->handler.GetTagItemDescription(this->handler.enrouteReleasePointTagItemId)
            );
        }

        TEST_F(EnrouteReleaseEventHandlerTest, ItReceivesIncomingReleases)
        {
            WebsocketMessage message{
                "App\\Events\\EnrouteReleaseEvent",
                "private-enroute-releases",
                nlohmann::json {
                    {"callsign", "BAW123"},
                    {"type", 1},
                    {"initiating_controller", "LON_S_CTR"},
                    {"target_controller", "LON_C_CTR"},
                    {"release_point", nlohmann::json::value_t::null}
                }
            };

            this->handler.ProcessWebsocketMessage(message);
            
            const EnrouteRelease& incomingRelease = this->handler.GetIncomingRelease("BAW123");
            EXPECT_EQ(1, incomingRelease.releaseType);
            EXPECT_FALSE(incomingRelease.hasReleasePoint);
            std::chrono::minutes minutesFromNow = std::chrono::duration_cast<std::chrono::minutes>(
                incomingRelease.clearTime - std::chrono::system_clock::now() + std::chrono::seconds(10)
            );
            EXPECT_EQ(std::chrono::minutes(3), minutesFromNow);
        }

        TEST_F(EnrouteReleaseEventHandlerTest, ItReceivesIncomingReleasesWithReleasePoints)
        {
            WebsocketMessage message{
                "App\\Events\\EnrouteReleaseEvent",
                "private-enroute-releases",
                nlohmann::json {
                    {"callsign", "BAW123"},
                    {"type", 1},
                    {"initiating_controller", "LON_S_CTR"},
                    {"target_controller", "LON_C_CTR"},
                    {"release_point", "ARNUN"}
                }
            };

            this->handler.ProcessWebsocketMessage(message);

            const EnrouteRelease& incomingRelease = this->handler.GetIncomingRelease("BAW123");
            EXPECT_EQ(1, incomingRelease.releaseType);
            EXPECT_TRUE(incomingRelease.hasReleasePoint);
            EXPECT_EQ("ARNUN", incomingRelease.releasePoint);
            std::chrono::minutes minutesFromNow = std::chrono::duration_cast<std::chrono::minutes>(
                incomingRelease.clearTime - std::chrono::system_clock::now() + std::chrono::seconds(10)
            );
            EXPECT_EQ(std::chrono::minutes(3), minutesFromNow);
        }

        TEST_F(EnrouteReleaseEventHandlerTest, ItRejectsInvalidReleaseMessages)
        {
            WebsocketMessage message{
                "App\\Events\\EnrouteReleaseEvent",
                "private-enroute-releases",
                nlohmann::json {
                    {"callsign", "BAW123"},
                    {"type", 1},
                    {"initiating_controller", "LON_S_CTR"},
                    {"target_controller", "LON_C_CTR"},
                }
            };

            this->handler.ProcessWebsocketMessage(message);

            EXPECT_EQ(this->handler.invalidRelease, this->handler.GetIncomingRelease("BAW123"));
        }

        TEST_F(EnrouteReleaseEventHandlerTest, ItIgnoresIncorrectEvents)
        {
            WebsocketMessage message{
                "App\\Events\\NotEnrouteReleaseEvent",
                "private-enroute-releases",
                nlohmann::json {
                    {"callsign", "BAW123"},
                    {"type", 1},
                    {"initiating_controller", "LON_S_CTR"},
                    {"target_controller", "LON_C_CTR"},
                    {"release_point", nlohmann::json::value_t::null}
                }
            };

            this->handler.ProcessWebsocketMessage(message);

            EXPECT_EQ(this->handler.invalidRelease, this->handler.GetIncomingRelease("BAW123"));
        }

        TEST_F(EnrouteReleaseEventHandlerTest, ItDisplaysOutgoingReleaseTypeTagItem)
        {
            ON_CALL(this->flightplan, GetCallsign())
                .WillByDefault(Return("BAW123"));

            TagData tagData(
                this->flightplan,
                this->radarTarget,
                108,
                EuroScopePlugIn::TAG_DATA_CORRELATED,
                this->itemString,
                &this->euroscopeColourCode,
                &this->tagColour,
                &this->fontSize
            );

            this->handler.AddOutgoingRelease("BAW123", { 1, false });
            this->handler.SetTagItemData(tagData);

            EXPECT_EQ("test1", tagData.GetItemString());
            EXPECT_EQ(RGB(255, 255, 0), tagData.GetTagColour());
        }

        TEST_F(EnrouteReleaseEventHandlerTest, ItDisplaysIncomingReleaseTypeTagItem)
        {
            ON_CALL(this->flightplan, GetCallsign())
                .WillByDefault(Return("BAW123"));

            TagData tagData(
                this->flightplan,
                this->radarTarget,
                108,
                EuroScopePlugIn::TAG_DATA_CORRELATED,
                this->itemString,
                &this->euroscopeColourCode,
                &this->tagColour,
                &this->fontSize
            );

            this->handler.AddIncomingRelease("BAW123", { 1, false });
            this->handler.SetTagItemData(tagData);

            EXPECT_EQ("test1", tagData.GetItemString());
            EXPECT_EQ(RGB(255, 0, 0), tagData.GetTagColour());
        }

        TEST_F(EnrouteReleaseEventHandlerTest, ItPrefersOutgoingReleaseTypeTagItem)
        {
            ON_CALL(this->flightplan, GetCallsign())
                .WillByDefault(Return("BAW123"));

            TagData tagData(
                this->flightplan,
                this->radarTarget,
                108,
                EuroScopePlugIn::TAG_DATA_CORRELATED,
                this->itemString,
                &this->euroscopeColourCode,
                &this->tagColour,
                &this->fontSize
            );

            this->handler.AddIncomingRelease("BAW123", { 1, false });
            this->handler.AddOutgoingRelease("BAW123", { 2, false });
            this->handler.SetTagItemData(tagData);

            EXPECT_EQ("test2", tagData.GetItemString());
            EXPECT_EQ(RGB(255, 255, 0), tagData.GetTagColour());
        }

        TEST_F(EnrouteReleaseEventHandlerTest, ItHandlesNoReleaseForCallsign)
        {
            ON_CALL(this->flightplan, GetCallsign())
                .WillByDefault(Return("BAW345"));

            TagData tagData(
                this->flightplan,
                this->radarTarget,
                108,
                EuroScopePlugIn::TAG_DATA_CORRELATED,
                this->itemString,
                &this->euroscopeColourCode,
                &this->tagColour,
                &this->fontSize
            );

            this->handler.AddOutgoingRelease("BAW123", { 1, false });
            this->handler.SetTagItemData(tagData);

            EXPECT_EQ("", tagData.GetItemString());
            EXPECT_EQ(RGB(255, 255, 255), tagData.GetTagColour());
        }

        TEST_F(EnrouteReleaseEventHandlerTest, ItDisplaysOutgoingReleasePointTagItem)
        {
            ON_CALL(this->flightplan, GetCallsign())
                .WillByDefault(Return("BAW123"));

            TagData tagData(
                this->flightplan,
                this->radarTarget,
                109,
                EuroScopePlugIn::TAG_DATA_CORRELATED,
                this->itemString,
                &this->euroscopeColourCode,
                &this->tagColour,
                &this->fontSize
            );

            this->handler.AddOutgoingRelease("BAW123", { 1, true, "ARNUN" });
            this->handler.SetTagItemData(tagData);

            EXPECT_EQ("ARNUN", tagData.GetItemString());
            EXPECT_EQ(RGB(255, 255, 0), tagData.GetTagColour());
        }

        TEST_F(EnrouteReleaseEventHandlerTest, ItDisplaysNoOutgoingReleasePointTagItem)
        {
            ON_CALL(this->flightplan, GetCallsign())
                .WillByDefault(Return("BAW123"));

            TagData tagData(
                this->flightplan,
                this->radarTarget,
                109,
                EuroScopePlugIn::TAG_DATA_CORRELATED,
                this->itemString,
                &this->euroscopeColourCode,
                &this->tagColour,
                &this->fontSize
            );

            this->handler.AddOutgoingRelease("BAW123", { 1, false, "ARNUN" });
            this->handler.SetTagItemData(tagData);

            EXPECT_EQ("", tagData.GetItemString());
            EXPECT_EQ(RGB(255, 255, 255), tagData.GetTagColour());
        }

        TEST_F(EnrouteReleaseEventHandlerTest, ItDisplaysIncomingReleasePointTagItem)
        {
            ON_CALL(this->flightplan, GetCallsign())
                .WillByDefault(Return("BAW123"));

            TagData tagData(
                this->flightplan,
                this->radarTarget,
                109,
                EuroScopePlugIn::TAG_DATA_CORRELATED,
                this->itemString,
                &this->euroscopeColourCode,
                &this->tagColour,
                &this->fontSize
            );

            this->handler.AddIncomingRelease("BAW123", { 1, true, "ABTUM" });
            this->handler.SetTagItemData(tagData);

            EXPECT_EQ("ABTUM", tagData.GetItemString());
            EXPECT_EQ(RGB(255, 0, 0), tagData.GetTagColour());
        }

        TEST_F(EnrouteReleaseEventHandlerTest, ItDisplaysNoIncomingReleasePointTagItem)
        {
            ON_CALL(this->flightplan, GetCallsign())
                .WillByDefault(Return("BAW123"));

            TagData tagData(
                this->flightplan,
                this->radarTarget,
                109,
                EuroScopePlugIn::TAG_DATA_CORRELATED,
                this->itemString,
                &this->euroscopeColourCode,
                &this->tagColour,
                &this->fontSize
            );

            this->handler.AddIncomingRelease("BAW123", { 1, false, "ABTUM" });
            this->handler.SetTagItemData(tagData);

            EXPECT_EQ("", tagData.GetItemString());
            EXPECT_EQ(RGB(255, 255, 255), tagData.GetTagColour());
        }

        TEST_F(EnrouteReleaseEventHandlerTest, ItPrefersOutgoingReleasePointTagItem)
        {
            ON_CALL(this->flightplan, GetCallsign())
                .WillByDefault(Return("BAW123"));

            TagData tagData(
                this->flightplan,
                this->radarTarget,
                109,
                EuroScopePlugIn::TAG_DATA_CORRELATED,
                this->itemString,
                &this->euroscopeColourCode,
                &this->tagColour,
                &this->fontSize
            );

            this->handler.AddIncomingRelease("BAW123", { 1, true, "ARNUN" });
            this->handler.AddOutgoingRelease("BAW123", { 2, true, "ABTUM" });
            this->handler.SetTagItemData(tagData);

            EXPECT_EQ("ABTUM", tagData.GetItemString());
            EXPECT_EQ(RGB(255, 255, 0), tagData.GetTagColour());
        }

        TEST_F(EnrouteReleaseEventHandlerTest, ItClearsExpiredReleases)
        {
            TagData tagData(
                this->flightplan,
                this->radarTarget,
                109,
                EuroScopePlugIn::TAG_DATA_CORRELATED,
                this->itemString,
                &this->euroscopeColourCode,
                &this->tagColour,
                &this->fontSize
            );

            this->handler.AddIncomingRelease(
                "BAW123",
                { 1, true, "ARNUN", std::chrono::system_clock::now() + std::chrono::seconds(3) }
            );
            this->handler.AddOutgoingRelease(
                "BAW123",
                { 2, true, "ABTUM", std::chrono::system_clock::now() + std::chrono::seconds(3) }
            );

            this->handler.AddIncomingRelease(
                "BAW456",
                { 1, true, "ARNUN", std::chrono::system_clock::now() - std::chrono::seconds(1) }
            );
            this->handler.AddOutgoingRelease(
                "BAW456",
                { 2, true, "ABTUM", std::chrono::system_clock::now() - std::chrono::seconds(1) }
            );
            this->handler.TimedEventTrigger();

            EXPECT_EQ("ARNUN", this->handler.GetIncomingRelease("BAW123").releasePoint);
            EXPECT_EQ("ABTUM", this->handler.GetOutgoingRelease("BAW123").releasePoint);
            EXPECT_EQ(this->handler.invalidRelease, this->handler.GetIncomingRelease("BAW456"));
            EXPECT_EQ(this->handler.invalidRelease, this->handler.GetOutgoingRelease("BAW456"));
        }
    }  // namespace Releases
}  // namespace UKControllerPluginTest
