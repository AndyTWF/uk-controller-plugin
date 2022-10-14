#pragma once

namespace UKControllerPlugin::Releases {
    class DepartureReleaseRequest;

    class DepartureReleaseRequestCollection
    {
        public:
        void Add(std::shared_ptr<DepartureReleaseRequest> request);
        [[nodiscard]] auto CountRequests() const -> size_t;
        [[nodiscard]] auto FindById(int id) const -> std::shared_ptr<DepartureReleaseRequest>;
        [[nodiscard]] auto
        FirstWhere(const std::function<bool(const std::shared_ptr<DepartureReleaseRequest>&)>& predicate) const
            -> std::shared_ptr<DepartureReleaseRequest>;
        void Iterate(const std::function<void(const std::shared_ptr<DepartureReleaseRequest>&)>& function);
        auto Remove(int id) -> void;
        auto RemoveWhere(
            const std::function<bool(const DepartureReleaseRequest& request)>& predicate,
            const std::function<void(const DepartureReleaseRequest& request)>& onRemove = {}) -> void;

        private:
        [[nodiscard]] auto RequestExists(int id) const -> bool;
        [[nodiscard]] auto FindRequestMatchingId(int id) const -> std::shared_ptr<DepartureReleaseRequest>;
        [[nodiscard]] auto LockRequestsList() const -> std::lock_guard<std::mutex>;

        // Mutex for protecting the collection
        mutable std::mutex requestsMutex;

        // All the requests
        std::vector<std::shared_ptr<DepartureReleaseRequest>> requests;
    };
} // namespace UKControllerPlugin::Releases
