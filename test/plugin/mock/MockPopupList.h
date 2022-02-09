#pragma once
#include "list/PopupListInterface.h"

using UKControllerPlugin::List::PopupListInterface;

namespace UKControllerPluginTest::List {
    class MockPopupList : public PopupListInterface
    {
        public:
        MOCK_METHOD(void, Trigger, (const POINT&), (override));
    };
} // namespace UKControllerPluginTest::List
