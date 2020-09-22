#include "pch/pch.h"
#include "stands/StandModule.h"
#include "bootstrap/PersistenceContainer.h"
#include "mock/MockDependencyLoader.h"
#include "websocket/WebsocketEventProcessorCollection.h"
#include "tag/TagItemCollection.h"
#include "plugin/FunctionCallEventHandler.h"
#include "controller/HandoffEventHandlerCollection.h"

using ::testing::NiceMock;
using ::testing::Test;
using ::testing::Return;
using UKControllerPlugin::Bootstrap::PersistenceContainer;
using UKControllerPlugin::Controller::HandoffEventHandlerCollection;
using UKControllerPlugin::Plugin::FunctionCallEventHandler;
using UKControllerPlugin::Stands::BootstrapPlugin;
using UKControllerPlugin::Tag::TagItemCollection;
using UKControllerPlugin::Websocket::WebsocketEventProcessorCollection;
using UKControllerPluginTest::Dependency::MockDependencyLoader;

namespace UKControllerPluginTest {
    namespace Stands {

        class StandModuleTest : public Test
        {
            public:
                StandModuleTest()
                {
                    container.websocketProcessors.reset(new WebsocketEventProcessorCollection);
                    container.tagHandler.reset(new TagItemCollection);
                    container.pluginFunctionHandlers.reset(new FunctionCallEventHandler);

                    nlohmann::json gatwick = nlohmann::json::array();
                    gatwick.push_back(
                        {
                            {"id", 1},
                            {"identifier", "31R"},
                        }
                    );
                    gatwick.push_back(
                        {
                            {"id", 2},
                            {"identifier", "35"},
                        }
                    );
                    nlohmann::json heathrow = nlohmann::json::array();
                    heathrow.push_back(
                        {
                            {"id", 3},
                            {"identifier", "76R"},
                        }
                    );
                    nlohmann::json dependency = {
                        {"EGKK", gatwick},
                        {"EGLL", heathrow}
                    };

                    EXPECT_CALL(
                        this->dependencyLoader,
                        LoadDependency("DEPENDENCY_STANDS", nlohmann::json::object())
                    )
                        .Times(1)
                        .WillOnce(Return(dependency));
                }

                NiceMock<MockDependencyLoader> dependencyLoader;
                PersistenceContainer container;
        };

        TEST_F(StandModuleTest, ItRegistersAssignedStandsTagItem)
        {
            BootstrapPlugin(this->container, this->dependencyLoader);
            EXPECT_TRUE(this->container.tagHandler->HasHandlerForItemId(110));
        }
    }  // namespace Stands
}  // namespace UKControllerPluginTest