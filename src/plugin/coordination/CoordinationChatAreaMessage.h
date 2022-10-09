#pragma once

namespace UKControllerPlugin::Coordination {
    /**
     * An interface to send chat area messages for coordination.
     */
    class CoordinationChatAreaMessage
    {
        public:
        virtual ~CoordinationChatAreaMessage() = default;
        virtual void SendMessage(const std::string& message) = 0;
    };
} // namespace UKControllerPlugin::Coordination
