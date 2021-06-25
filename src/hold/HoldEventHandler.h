#pragma once
#include "tag/TagItemInterface.h"
#include "timedevent/AbstractTimedEvent.h"
#include "navaids/NavaidCollection.h"
#include "push/PushEventProcessorInterface.h"
#include "tag/TagData.h"

namespace UKControllerPlugin {
    namespace Euroscope {
        class EuroscopePluginLoopbackInterface;
    }  // namespace Euroscope
    namespace Hold {
        class HoldManager;
    }  // namespace Hold
}  // namespace UKControllerPlugin

namespace UKControllerPlugin {
    namespace Hold {

        /*
            Handles events that relate to holding - namely the periodic
            update of holding data.
        */
        class HoldEventHandler : public UKControllerPlugin::Tag::TagItemInterface,
            public UKControllerPlugin::TimedEvent::AbstractTimedEvent,
            public Push::PushEventProcessorInterface
        {
            public:
                HoldEventHandler(
                    UKControllerPlugin::Hold::HoldManager& holdManager,
                    const UKControllerPlugin::Navaids::NavaidCollection& navaids,
                    UKControllerPlugin::Euroscope::EuroscopePluginLoopbackInterface & plugin,
                    const int popupMenuItemId
                );
                ~HoldEventHandler() override = default;

                // Inherited via TagItemInterface
                std::string GetTagItemDescription(int tagItemId) const override;
                void SetTagItemData(UKControllerPlugin::Tag::TagData& tagData) override;

                // Inherited via AbstractTimedEvent
                void TimedEventTrigger(void) override;

                // Inherited via WebsocketEventProcessorInterface
                void ProcessPushEvent(
                    const Push::PushEvent& message
                ) override;
                std::set<Push::PushEventSubscription>
                GetPushEventSubscriptions(void) const override;

                void PluginEventsSynced() override;

                // The string to display when an aircraft is not holding
                const std::string noHold = "NOHOLD";

                // The id of this handlers popup menu item
                const int popupMenuItemId;

                // How far from a navaid an aircraft can be before its considered in proximity of the hold
                const double proximityDistance = 12.0;

            private:

                // Navaids against which holds are based
                const UKControllerPlugin::Navaids::NavaidCollection& navaids;

                // Gives access to the plugin
                UKControllerPlugin::Euroscope::EuroscopePluginLoopbackInterface & plugin;

                // Manages holds
                UKControllerPlugin::Hold::HoldManager & holdManager;
        };
    }  // namespace Hold
}  // namespace UKControllerPlugin
