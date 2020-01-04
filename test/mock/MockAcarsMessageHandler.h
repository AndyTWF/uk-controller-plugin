#pragma once
#include "acars/AcarsMessageHandler.h"
#include "acars/IncomingAcarsMessage.h"

namespace UKControllerPluginTest {
    namespace Acars {

        class MockAcarsMessageHandler :
            public UKControllerPlugin::Acars::AcarsMessageHandler
        {
            public:
                MOCK_METHOD1(
                    HandleAcarsMessage,
                    bool(const UKControllerPlugin::Acars::IncomingAcarsMessage& message)
                );
        };
    }  // namespace Acars
}  // namespace UKControllerPluginTest
