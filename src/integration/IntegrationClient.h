#pragma once

namespace UKControllerPlugin::Integration {
    class IntegrationConnection;
    struct MessageType;
    class MessageInterface;

    /*
     * Stores information about an integrating application - what types of events they
     * are interested in etc.
     */
    class IntegrationClient
    {
        public:
            IntegrationClient(
                int id,
                std::string integrationName,
                std::string integrationVersion,
                std::shared_ptr<IntegrationConnection> connection
            );
            ~IntegrationClient();
            const std::set<std::shared_ptr<MessageType>>& InterestedMessages() const;
            bool InterestedInMessage(const MessageType& message) const;
            void ClearInterestedMessages();
            void AddInterestedMessage(std::shared_ptr<MessageType> message);
            const std::shared_ptr<IntegrationConnection> Connection() const;
            int Id() const;
            std::string GetIntegrationString() const;

        private:
            // Unique identifier of the integration
            const int id;

            // Name of the integration
            const std::string integrationName = "Unknown";

            // Version of the integration
            const std::string integrationVersion = "Unknown";

            // The messages that this target is interested in receiving
            std::set<std::shared_ptr<MessageType>> interestedMessages;

            // The connection for this client
            const std::shared_ptr<IntegrationConnection> connection;
    };
} // namespace UKControllerPlugin::Integration
