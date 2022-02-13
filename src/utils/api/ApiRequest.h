#pragma once
#include "ApiRequestData.h"
#include "Response.h"

namespace UKControllerPluginUtils::Api {
    class ApiRequestPerformerInterface;
    class ApiRequestException;
    class ChainableRequest;

    class ApiRequest
    {
        public:
        ApiRequest(
            const ApiRequestData& data,
            ApiRequestPerformerInterface& performer,
            std::function<void(void)> onCompletionHandler);
        ~ApiRequest();
        ApiRequest(ApiRequest&&) = delete;
        ApiRequest(const ApiRequest&) = delete;
        auto operator=(const ApiRequest&) = delete;
        auto operator=(ApiRequest&&) -> ApiRequest& = delete;
        auto Then(const std::function<void(Response)>& function) -> ApiRequest&;
        auto Then(const std::function<void()>& function) -> ApiRequest&;
        auto Catch(const std::function<void(const ApiRequestException&)>& function) -> ApiRequest&;
        void Await();

        private:
        // Allows continuation
        std::shared_ptr<ChainableRequest> chain;

        // Run on main thread or async
        bool async = true;
    };
} // namespace UKControllerPluginUtils::Api
