#include "DepartureWakeInterval.h"
#include "WakeCategory.h"

namespace UKControllerPlugin::Wake {

    WakeCategory::WakeCategory(
        int id,
        std::string code,
        std::string description,
        int relativeWeighting,
        std::list<std::shared_ptr<DepartureWakeInterval>> subsequentDepartureIntervals)
        : id(id), code(std::move(code)), description(std::move(description)), relativeWeighting(relativeWeighting),
          subsequentDepartureIntervals(std::move(subsequentDepartureIntervals))
    {
    }

    int WakeCategory::Id() const
    {
        return id;
    }

    auto WakeCategory::Code() const -> const std::string&
    {
        return code;
    }

    auto WakeCategory::Description() const -> const std::string&
    {
        return description;
    }

    auto WakeCategory::RelativeWeighting() const -> int
    {
        return relativeWeighting;
    }

    auto WakeCategory::SubsequentDepartureIntervals() const -> const std::list<std::shared_ptr<DepartureWakeInterval>>&
    {
        return subsequentDepartureIntervals;
    }

    auto WakeCategory::DepartureInterval(const WakeCategory& nextAircraftCategory, bool intermediate) const
        -> std::shared_ptr<DepartureWakeInterval>
    {
        auto matchingInterval = std::find_if(
            subsequentDepartureIntervals.cbegin(),
            subsequentDepartureIntervals.cend(),
            [&nextAircraftCategory, &intermediate](const std::shared_ptr<DepartureWakeInterval>& interval) -> bool {
                return interval->intervalIsIntermediate == intermediate &&
                       interval->subsequentWakeCategoryId == nextAircraftCategory.Id();
            });

        return matchingInterval == subsequentDepartureIntervals.cend() ? nullptr : *matchingInterval;
    }
} // namespace UKControllerPlugin::Wake
