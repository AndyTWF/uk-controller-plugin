#pragma once

namespace UKControllerPlugin::Integration {

    class OutboundIntegrationEventHandler;
    class InboundIntegrationMessageHandler;
    class IntegrationServer;

    /*
     * Persists things relating to the integration
     * module so that we can manage their destruction
     * time more carefully
     */
    using IntegrationPersistenceContainer = struct IntegrationPersistenceContainer
    {
        IntegrationPersistenceContainer(
            std::shared_ptr<OutboundIntegrationEventHandler> outboundMessageHandler,
            std::shared_ptr<InboundIntegrationMessageHandler> inboundMessageHandler,
            std::unique_ptr<IntegrationServer> server);
        ~IntegrationPersistenceContainer();
        IntegrationPersistenceContainer(const IntegrationPersistenceContainer&) = delete;
        IntegrationPersistenceContainer(IntegrationPersistenceContainer&&) noexcept;
        auto operator=(const IntegrationPersistenceContainer&) -> IntegrationPersistenceContainer& = delete;
        auto operator=(IntegrationPersistenceContainer&&) noexcept -> IntegrationPersistenceContainer&;

        // Handles outbound messages
        std::shared_ptr<OutboundIntegrationEventHandler> outboundMessageHandler{};

        // Handles inbound messages
        std::shared_ptr<InboundIntegrationMessageHandler> inboundMessageHandler{};

        // The server that listens for connections
        std::unique_ptr<IntegrationServer> server{};
    };
} // namespace UKControllerPlugin::Integration
