#include "ApiCurlRequestFactory.h"
#include "ApiHeaderApplicator.h"
#include "ApiUrlBuilder.h"
#include "CurlApiRequestPerformer.h"
#include "CurlApiRequestPerformerFactory.h"
#include "curl/CurlInterface.h"

using UKControllerPlugin::Curl::CurlInterface;

namespace UKControllerPluginUtils::Api {

    CurlApiRequestPerformerFactory::CurlApiRequestPerformerFactory(std::unique_ptr<CurlInterface> curl)
        : curl(std::move(curl))
    {
    }

    CurlApiRequestPerformerFactory::~CurlApiRequestPerformerFactory() = default;

    ApiRequestPerformerInterface& CurlApiRequestPerformerFactory::Make(const ApiSettings& apiSettings)
    {
        if (performer == nullptr) {
            urlBuilder = std::make_unique<ApiUrlBuilder>(apiSettings);
            headerApplicator = std::make_unique<ApiHeaderApplicator>(apiSettings);
            curlRequestFactory = std::make_unique<ApiCurlRequestFactory>(*urlBuilder, *headerApplicator);
            performer = std::make_unique<CurlApiRequestPerformer>(*curl, *curlRequestFactory);
        }

        return *performer;
    }
} // namespace UKControllerPluginUtils::Api
