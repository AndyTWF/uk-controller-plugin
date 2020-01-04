#pragma once
#include "pch/stdafx.h"
#include "curl/CurlEscaper.h"
#include "curl/curl.h"

namespace UKControllerPlugin {
    namespace Curl {

        /*
            Create a URL escaped string from a list of fields.
        */
        std::string CreateEscapedStringFromMap(std::map<std::string, std::string> map)
        {
            std::string escapedString;
            CURL* curlObject;
            curlObject = curl_easy_init();
            char* escapedField = nullptr;

            for (
                std::map<std::string, std::string>::const_iterator it = map.cbegin();
                it != map.cend();
                ++it
            ) {
                escapedField = curl_easy_escape(curlObject, it->second.c_str(), 0);
                escapedString += it->first + "=" + std::string(escapedField) + "&";
            }

            if (escapedField != nullptr) {
                curl_free(escapedField);
            }
            curl_easy_cleanup(curlObject);

            return escapedString.substr(0, escapedString.size() - 1);
        }
    }  // namespace Curl
}  // namespace UKControllerPlugin
