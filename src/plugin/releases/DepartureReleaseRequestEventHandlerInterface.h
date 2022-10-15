#pragma once

namespace UKControllerPlugin::Releases {
    class DepartureReleaseRequest;

    class DepartureReleaseRequestEventHandlerInterface
    {
        public:
        virtual ~DepartureReleaseRequestEventHandlerInterface() = default;
        virtual void NewRelease(const DepartureReleaseRequest& request)
        {
        }

        virtual void ReleaseCancelled(const DepartureReleaseRequest& request)
        {
        }

        virtual void ReleaseAcknowledged(const DepartureReleaseRequest& request)
        {
        }

        virtual void ReleaseTimeout(const DepartureReleaseRequest& request)
        {
        }
    };
} // namespace UKControllerPlugin::Releases
