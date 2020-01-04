#include "pch/pch.h"
#include "curl/CurlEscaper.h"

using UKControllerPlugin::Curl::CreateEscapedStringFromMap;

namespace UKControllerPluginTest {
    namespace Curl {

        TEST(CurlEscaperTest, ItEscapesAMap)
        {
            std::string expected = "baz=bob&foo=bar";
            std::string actual = CreateEscapedStringFromMap({ {"foo", "bar"}, {"baz", "bob"} });

            EXPECT_EQ(expected, actual);
        }

        TEST(CurlEscaperTest, ItEscapesAMapWithSpecialCharacters)
        {
            std::string expected = "baz=bo%23b&foo=ba%20r";
            std::string actual = CreateEscapedStringFromMap({ {"foo", "ba r"}, {"baz", "bo#b"} });

            EXPECT_EQ(expected, actual);
        }

    }  // namespace Curl
}  // namespace UKControllerPluginTest
