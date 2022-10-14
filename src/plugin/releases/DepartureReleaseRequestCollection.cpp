#include "DepartureReleaseRequest.h"
#include "DepartureReleaseRequestCollection.h"

namespace UKControllerPlugin::Releases {

    void DepartureReleaseRequestCollection::Add(std::shared_ptr<DepartureReleaseRequest> request)
    {
        const auto lock = LockRequestsList();
        if (RequestExists(request->Id())) {
            return;
        }

        requests.push_back(request);
    }

    auto DepartureReleaseRequestCollection::CountRequests() const -> size_t
    {
        const auto lock = LockRequestsList();
        return requests.size();
    }

    auto DepartureReleaseRequestCollection::FindById(int id) const -> std::shared_ptr<DepartureReleaseRequest>
    {
        const auto lock = LockRequestsList();
        return FindRequestMatchingId(id);
    }

    auto DepartureReleaseRequestCollection::FirstWhere(
        const std::function<bool(const std::shared_ptr<DepartureReleaseRequest>&)>& predicate) const
        -> std::shared_ptr<DepartureReleaseRequest>
    {
        const auto lock = LockRequestsList();
        const auto release = std::find_if(requests.begin(), requests.end(), predicate);
        return release == requests.cend() ? nullptr : *release;
    }

    void DepartureReleaseRequestCollection::Iterate(
        const std::function<void(const std::shared_ptr<DepartureReleaseRequest>&)>& function)
    {
        const auto lock = LockRequestsList();
        for (const auto& request : requests) {
            function(request);
        }
    }

    auto DepartureReleaseRequestCollection::Remove(int id) -> void
    {
        const auto lock = LockRequestsList();
        std::erase_if(
            requests, [&id](const std::shared_ptr<DepartureReleaseRequest>& request) { return request->Id() == id; });
    }

    auto DepartureReleaseRequestCollection::RemoveWhere(
        const std::function<bool(const DepartureReleaseRequest&)>& predicate,
        const std::function<void(const DepartureReleaseRequest&)>& onRemove) -> void
    {
        const auto lock = LockRequestsList();
        for (auto request = requests.cbegin(); request != requests.cend();) {
            if (predicate(**request)) {
                if (onRemove) {
                    onRemove(**request);
                }

                request = requests.erase(request);
            } else {
                ++request;
            }
        }
    }

    auto DepartureReleaseRequestCollection::RequestExists(int id) const -> bool
    {
        return FindRequestMatchingId(id) != nullptr;
    }

    auto DepartureReleaseRequestCollection::FindRequestMatchingId(int id) const
        -> std::shared_ptr<DepartureReleaseRequest>
    {
        const auto request =
            std::find_if(requests.begin(), requests.end(), [&id](const auto& request) { return request->Id() == id; });
        return request == requests.cend() ? nullptr : *request;
    }

    auto DepartureReleaseRequestCollection::LockRequestsList() const -> std::lock_guard<std::mutex>
    {
        return std::lock_guard(this->requestsMutex);
    }
} // namespace UKControllerPlugin::Releases
