#include "pch/pch.h"
#include "acars/AcarsMessageHandlerCollection.h"
#include "mock/MockAcarsMessageHandler.h"
#include "acars/IncomingAcarsMessage.h"

using UKControllerPlugin::Acars::AcarsMessageHandlerCollection;
using UKControllerPluginTest::Acars::MockAcarsMessageHandler;
using UKControllerPlugin::Acars::IncomingAcarsMessage;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Test;
using ::testing::Ref;
using ::testing::Return;

namespace UKControllerPluginTest {
    namespace EventHandler {

        class AcarsMessageHandlerCollectionTest : public Test
        {
            public:
                AcarsMessageHandlerCollectionTest()
                    : message("BAW123", "TELEX", "FOO")
                {
                    mockHandler1.reset(new NiceMock<MockAcarsMessageHandler>);
                    mockHandler2.reset(new NiceMock<MockAcarsMessageHandler>);
                    mockHandler3.reset(new NiceMock<MockAcarsMessageHandler>);
                }

                AcarsMessageHandlerCollection collection;
                IncomingAcarsMessage message;
                std::shared_ptr<NiceMock<MockAcarsMessageHandler>> mockHandler1;
                std::shared_ptr<NiceMock<MockAcarsMessageHandler>> mockHandler2;
                std::shared_ptr<NiceMock<MockAcarsMessageHandler>> mockHandler3;
        };

        TEST_F(AcarsMessageHandlerCollectionTest, TestHandleMessageCallsTheHandlers)
        {

            this->collection.RegisterHandler(this->mockHandler1);
            this->collection.RegisterHandler(this->mockHandler2);
            this->collection.RegisterHandler(this->mockHandler3);

            EXPECT_CALL(*this->mockHandler1, HandleAcarsMessage(Ref(this->message)))
                .Times(1)
                .WillOnce(Return(false));

            EXPECT_CALL(*this->mockHandler2, HandleAcarsMessage(Ref(this->message)))
                .Times(1)
                .WillOnce(Return(false));

            EXPECT_CALL(*this->mockHandler3, HandleAcarsMessage(Ref(this->message)))
                .Times(1)
                .WillOnce(Return(false));

            this->collection.HandleIncomingMessage(this->message);
        }

        TEST_F(AcarsMessageHandlerCollectionTest, TestItSkipsHandlersIfOneSuccessfullyHandlesMessage)
        {

            this->collection.RegisterHandler(this->mockHandler1);
            this->collection.RegisterHandler(this->mockHandler2);
            this->collection.RegisterHandler(this->mockHandler3);

            EXPECT_CALL(*this->mockHandler1, HandleAcarsMessage(Ref(this->message)))
                .Times(1)
                .WillOnce(Return(true));

            EXPECT_CALL(*this->mockHandler2, HandleAcarsMessage(Ref(this->message)))
                .Times(0);

            EXPECT_CALL(*this->mockHandler3, HandleAcarsMessage(Ref(this->message)))
                .Times(0);

            this->collection.HandleIncomingMessage(this->message);
        }


        TEST_F(AcarsMessageHandlerCollectionTest, CountHandlersReturnsTheNumberOfHandlers)
        {
            this->collection.RegisterHandler(this->mockHandler1);
            EXPECT_EQ(1, this->collection.CountHandlers());

            this->collection.RegisterHandler(this->mockHandler2);
            EXPECT_EQ(2, this->collection.CountHandlers());

            this->collection.RegisterHandler(this->mockHandler3);
            EXPECT_EQ(3, this->collection.CountHandlers());
        }

        TEST_F(AcarsMessageHandlerCollectionTest, StartsEmpty)
        {
            EXPECT_EQ(0, this->collection.CountHandlers());
        }

        TEST_F(AcarsMessageHandlerCollectionTest, RegisterHandlerDoesNotAddDuplicates)
        {
            this->collection.RegisterHandler(this->mockHandler1);
            EXPECT_EQ(1, this->collection.CountHandlers());
            this->collection.RegisterHandler(this->mockHandler1);
            EXPECT_EQ(1, this->collection.CountHandlers());
            this->collection.RegisterHandler(this->mockHandler1);
            EXPECT_EQ(1, this->collection.CountHandlers());
        }
    }  // namespace EventHandler
}  // namespace UKControllerPluginTest
