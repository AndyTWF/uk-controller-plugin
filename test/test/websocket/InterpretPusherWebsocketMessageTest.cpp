#include "pch/pch.h"
#include "websocket/InterpretPusherWebsocketMessage.h"
#include "websocket/WebsocketMessage.h"

using UKControllerPlugin::Websocket::WebsocketMessage;

TEST(InterpretPusherMessageTest, ItReturnsInvalidIfJsonInvalid)
{
    EXPECT_EQ(invalidMessage, InterpretPusherMessage("{]"));
}

TEST(InterpretPusherMessageTest, ItReturnsInvalidIfNoEvent)
{
    nlohmann::json message;
    message["channel"] = "test-channel";
    message["data"] = { {"test", "lol"} };

    EXPECT_EQ(invalidMessage, InterpretPusherMessage(message.dump()));
}

TEST(InterpretPusherMessageTest, ItReturnsInvalidIfEventNotAString)
{
    nlohmann::json message;
    message["event"] = 1;
    message["channel"] = "test-channel";
    message["data"] = { {"test", "lol"} };

    EXPECT_EQ(invalidMessage, InterpretPusherMessage(message.dump()));
}

TEST(InterpretPusherMessageTest, ItHandlesMissingChannel)
{
    nlohmann::json message;
    message["event"] = "test-event";
    message["data"] = nlohmann::json({ {"test", "lol"} }).dump();

    WebsocketMessage expectedMessage = {
        "test-event",
        "none",
        { {"test", "lol"} },
        false
    };

    EXPECT_EQ(expectedMessage, InterpretPusherMessage(message.dump()));
}

TEST(InterpretPusherMessageTest, ItHandlesNonStringChannel)
{
    nlohmann::json message;
    message["event"] = "test-event";
    message["channel"] = 1;
    message["data"] = nlohmann::json({ {"test", "lol"} }).dump();

    WebsocketMessage expectedMessage = {
        "test-event",
        "none",
        { {"test", "lol"} },
        false
    };

    EXPECT_EQ(expectedMessage, InterpretPusherMessage(message.dump()));
}

TEST(InterpretPusherMessageTest, ItHandlesMissingData)
{
    nlohmann::json message;
    message["event"] = "test-event";
    message["channel"] = "test-channel";

    WebsocketMessage expectedMessage = {
        "test-event",
        "test-channel",
        {},
        false
    };

    EXPECT_EQ(expectedMessage, InterpretPusherMessage(message.dump()));
}

TEST(InterpretPusherMessageTest, ItHandlesNonObjectData)
{
    nlohmann::json message;
    message["event"] = "test-event";
    message["channel"] = "test-channel";
    message["data"] = nlohmann::json({ 1, 2, 3 }).dump();

    WebsocketMessage expectedMessage = {
        "test-event",
        "test-channel",
        {},
        false
    };

    EXPECT_EQ(expectedMessage, InterpretPusherMessage(message.dump()));
}

TEST(InterpretPusherMessageTest, ItDetectsProtocolMessages)
{
    nlohmann::json message;
    message["event"] = "pusher:test";
    message["channel"] = "test-channel";
    message["data"] = nlohmann::json({ {"test", "lol"} }).dump();

    WebsocketMessage expectedMessage = {
        "pusher:test",
        "test-channel",
        { {"test", "lol"} },
        true
    };

    EXPECT_EQ(expectedMessage, InterpretPusherMessage(message.dump()));
}
