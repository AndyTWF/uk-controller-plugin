#pragma once
#include "releases/DepartureReleaseRequestEventHandlerInterface.h"

using UKControllerPlugin::Releases::DepartureReleaseRequest;
using UKControllerPlugin::Releases::DepartureReleaseRequestEventHandlerInterface;

namespace UKControllerPluginTest::Releases {

    class MockDepartureReleaseRequestEventHandlerInterface : public DepartureReleaseRequestEventHandlerInterface
    {
        public:
        MOCK_METHOD(void, NewRelease, (const DepartureReleaseRequest&), (override));
        MOCK_METHOD(void, ReleaseCancelled, (const DepartureReleaseRequest&), (override));
        MOCK_METHOD(void, ReleaseAcknowledged, (const DepartureReleaseRequest&), (override));
        MOCK_METHOD(void, ReleaseTimeout, (const DepartureReleaseRequest&), (override));
    };
} // namespace UKControllerPluginTest::Releases
