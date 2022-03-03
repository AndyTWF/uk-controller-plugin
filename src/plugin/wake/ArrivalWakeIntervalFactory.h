#pragma once

namespace UKControllerPlugin::Wake {
    class ArrivalWakeInterval;

    [[nodiscard]] auto ArrivalIntervalFromJson(const nlohmann::json& json) -> std::shared_ptr<ArrivalWakeInterval>;
    [[nodiscard]] auto ArrivalIntervalValid(const nlohmann::json& json) -> bool;
} // namespace UKControllerPlugin::Wake
