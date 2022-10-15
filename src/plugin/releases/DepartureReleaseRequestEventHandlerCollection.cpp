#include "DepartureReleaseRequestEventHandlerCollection.h"
#include "DepartureReleaseRequestEventHandlerInterface.h"

namespace UKControllerPlugin::Releases {

    void DepartureReleaseRequestEventHandlerCollection::AddHandler(
        const std::shared_ptr<DepartureReleaseRequestEventHandlerInterface>& handler)
    {
        if (handlers.insert(handler).second) {
            LogError("Duplicate departure release request handler added");
        }
    }

    auto DepartureReleaseRequestEventHandlerCollection::CountHandlers() const -> size_t
    {
        return handlers.size();
    }

    void
    DepartureReleaseRequestEventHandlerCollection::ReleaseAcknowledged(const DepartureReleaseRequest& request) const
    {
        for (const auto& handler : handlers) {
            handler->ReleaseAcknowledged(request);
        }
    }

    void DepartureReleaseRequestEventHandlerCollection::ReleaseCancelled(const DepartureReleaseRequest& request) const
    {
        for (const auto& handler : handlers) {
            handler->ReleaseCancelled(request);
        }
    }

    void DepartureReleaseRequestEventHandlerCollection::NewRelease(const DepartureReleaseRequest& request) const
    {
        for (const auto& handler : handlers) {
            handler->NewRelease(request);
        }
    }

    void DepartureReleaseRequestEventHandlerCollection::ReleaseTimeout(const DepartureReleaseRequest& request) const
    {
        for (const auto& handler : handlers) {
            handler->ReleaseTimeout(request);
        }
    }
} // namespace UKControllerPlugin::Releases
