#pragma once
#include "coordination/CoordinationChatAreaMessage.h"

using UKControllerPlugin::Coordination::CoordinationChatAreaMessage;

namespace UKControllerPluginTest::Coordination {
    class MockCoordinationChatAreaMessage : public CoordinationChatAreaMessage
    {
        public:
        MOCK_METHOD(void, SendMessage, (const std::string&), (override));
    };
} // namespace UKControllerPluginTest::Coordination
