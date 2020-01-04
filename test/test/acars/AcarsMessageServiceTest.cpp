#include "pch/pch.h"
#include "acars/AcarsMessageService.h"
#include "acars/AcarsCredentials.h"
#include "mock/MockCurlApi.h"
#include "mock/MockTaskRunnerInterface.h"

using UKControllerPlugin::Acars::AcarsMessageService;
using UKControllerPlugin::Acars::AcarsCredentials;
using UKControllerPlugin::Curl::CurlRequest;
using UKControllerPlugin::Curl::CurlResponse;
using UKControllerPluginTest::Curl::MockCurlApi;
using UKControllerPluginTest::TaskManager::MockTaskRunnerInterface;
using ::testing::Test;
using ::testing::NiceMock;
using ::testing::Return;

namespace UKControllerPluginTest {
    namespace Acars {

        class AcarsMessageServiceTest : public Test
        {
            public:

            AcarsMessageServiceTest()
                : service(mockCurl, mockTaskRunner),
                expectedRequest("http://www.hoppie.nl/acars/system/connect.html", CurlRequest::METHOD_POST)
            {
                expectedRequest.SetBody("from=foo&logon=bar&to=SERVER&type=PING");
                expectedRequest.AddHeader("Content-Type", "application/x-www-form-urlencoded");
            }

            CurlRequest expectedRequest;
            NiceMock<MockCurlApi> mockCurl;
            MockTaskRunnerInterface mockTaskRunner;
            AcarsMessageService service;
        };

        TEST_F(AcarsMessageServiceTest, ItDoesntLogAUserInIfCredentialsAreInvalid)
        {
            this->service.Login(AcarsCredentials());
            EXPECT_FALSE(this->service.IsLoggedIn());
        }

        TEST_F(AcarsMessageServiceTest, ItDoesntLogAUserInIfCurlError)
        {
            CurlResponse expectedResponse("", true, 0L);
            EXPECT_CALL(this->mockCurl, MakeCurlRequest(this->expectedRequest))
                .Times(1)
                .WillOnce(Return(expectedResponse));

            this->service.Login(AcarsCredentials("foo", "bar"));
            EXPECT_FALSE(this->service.IsLoggedIn());
        }

        TEST_F(AcarsMessageServiceTest, ItDoesntLogAUserInIfNot200)
        {
            CurlResponse expectedResponse("", false, 404L);
            EXPECT_CALL(this->mockCurl, MakeCurlRequest(this->expectedRequest))
                .Times(1)
                .WillOnce(Return(expectedResponse));

            this->service.Login(AcarsCredentials("foo", "bar"));
            EXPECT_FALSE(this->service.IsLoggedIn());
        }

        TEST_F(AcarsMessageServiceTest, ItDoesntLogAUserInIfNotOk)
        {
            CurlResponse expectedResponse("error", false, 200L);
            EXPECT_CALL(this->mockCurl, MakeCurlRequest(this->expectedRequest))
                .Times(1)
                .WillOnce(Return(expectedResponse));

            this->service.Login(AcarsCredentials("foo", "bar"));
            EXPECT_FALSE(this->service.IsLoggedIn());
        }

        TEST_F(AcarsMessageServiceTest, ItLogsAUserIn)
        {
            CurlResponse expectedResponse("ok yay", false, 200L);
            EXPECT_CALL(this->mockCurl, MakeCurlRequest(this->expectedRequest))
                .Times(1)
                .WillOnce(Return(expectedResponse));

            this->service.Login(AcarsCredentials("foo", "bar"));
            EXPECT_TRUE(this->service.IsLoggedIn());
        }

    }  // namespace Acars
}  // namespace UKControllerPluginTest
