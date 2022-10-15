#pragma once

namespace UKControllerPlugin::Releases {
    class DepartureReleaseRequest;
    class DepartureReleaseRequestEventHandlerInterface;

    class DepartureReleaseRequestEventHandlerCollection
    {
        public:
        void AddHandler(const std::shared_ptr<DepartureReleaseRequestEventHandlerInterface>& handler);
        [[nodiscard]] auto CountHandlers() const -> size_t;
        void ReleaseAcknowledged(const DepartureReleaseRequest& request) const;
        void ReleaseCancelled(const DepartureReleaseRequest& request) const;
        void NewRelease(const DepartureReleaseRequest& request) const;
        void ReleaseTimeout(const DepartureReleaseRequest& request) const;

        private:
        // All the handlers
        std::set<std::shared_ptr<DepartureReleaseRequestEventHandlerInterface>> handlers;
    };
} // namespace UKControllerPlugin::Releases
