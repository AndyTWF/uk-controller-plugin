#include "bootstrap/PersistenceContainer.h"
#include "controller/ActiveCallsignCollection.h"
#include "euroscope/UserSettingAwareCollection.h"
#include "flightplan/FlightPlanEventHandlerCollection.h"
#include "plugin/FunctionCallEventHandler.h"
#include "squawk/SquawkEventHandler.h"
#include "squawk/SquawkModule.h"
#include "timedevent/TimedEventCollection.h"
#include "memory"
#include "memory"

using ::testing::Test;
using UKControllerPlugin::Bootstrap::PersistenceContainer;
using UKControllerPlugin::Controller::ActiveCallsignCollection;
using UKControllerPlugin::Euroscope::UserSettingAwareCollection;
using UKControllerPlugin::Flightplan::FlightPlanEventHandlerCollection;
using UKControllerPlugin::Plugin::FunctionCallEventHandler;
using UKControllerPlugin::Squawk::SquawkModule;
using UKControllerPlugin::TimedEvent::TimedEventCollection;

namespace UKControllerPluginTest::Squawk {

    class SquawkModuleTest : public Test
    {
        public:
        void SetUp() override
        {
            this->container.flightplanHandler = std::make_unique<FlightPlanEventHandlerCollection>();
            this->container.pluginFunctionHandlers = std::make_unique<FunctionCallEventHandler>();
            this->container.timedHandler = std::make_unique<TimedEventCollection>();
            this->container.userSettingHandlers = std::make_shared<UserSettingAwareCollection>();
            this->container.activeCallsigns = std::make_unique<ActiveCallsignCollection>();
        }

        PersistenceContainer container;
    };

    TEST_F(SquawkModuleTest, BootstrapPluginRegistersForFlightplanEvents)
    {
        SquawkModule::BootstrapPlugin(container, false);
        EXPECT_EQ(1, this->container.flightplanHandler->CountHandlers());
    }

    TEST_F(SquawkModuleTest, BootstrapPluginRegistersForTimedEvents)
    {
        SquawkModule::BootstrapPlugin(container, false);
        EXPECT_EQ(1, container.timedHandler->CountHandlersForFrequency(SquawkModule::allocationCheckFrequency));
    }

    TEST_F(SquawkModuleTest, BootstrapPluginRegistersEventHandlerForTimedEvents)
    {
        SquawkModule::BootstrapPlugin(container, false);
        EXPECT_EQ(
            1, this->container.timedHandler->CountHandlersForFrequency(SquawkModule::squawkAssignmentsCheckFrequency));
    }

    TEST_F(SquawkModuleTest, BootstrapPluginRegistersEventHandlerForActiveCallsignEvents)
    {
        SquawkModule::BootstrapPlugin(container, false);
        EXPECT_EQ(1, this->container.activeCallsigns->CountHandlers());
    }

    TEST_F(SquawkModuleTest, BootstrapPluginRegistersFunctionCallbacks)
    {
        SquawkModule::BootstrapPlugin(container, false);
        EXPECT_EQ(2, this->container.pluginFunctionHandlers->CountTagFunctions());
        EXPECT_EQ(0, this->container.pluginFunctionHandlers->CountCallbacks());
    }

    TEST_F(SquawkModuleTest, BootstrapPluginRegistersForUserSettingsEvents)
    {
        SquawkModule::BootstrapPlugin(container, false);
        EXPECT_EQ(1, this->container.userSettingHandlers->Count());
    }

    TEST_F(SquawkModuleTest, BootstrapPluginDisablesAutomaticGenerationWhereRequired)
    {
        SquawkModule::BootstrapPlugin(container, true);
        EXPECT_TRUE(this->container.squawkEvents->AutomaticAssignmentsDisabled());
    }
} // namespace UKControllerPluginTest::Squawk
