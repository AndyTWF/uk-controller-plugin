#pragma once

namespace UKControllerPlugin::IntentionCode {

    /*
        Structure containing data about an intention code.
    */
    using IntentionCodeData = struct IntentionCodeData
    {

        IntentionCodeData()
            : intentionCode("--"), exitPointIndex(INVALID_EXIT_POINT), ukExitPointIndex(INVALID_EXIT_POINT),
              exitPoint(""), ukExitPoint("")
        {
        }

        IntentionCodeData(
            std::string intentionCode,
            int exitPointIndex,
            int ukExitPointIndex,
            std::string exitPoint,
            std::string ukExitPoint)
            : intentionCode(intentionCode), exitPointIndex(exitPointIndex), ukExitPointIndex(ukExitPointIndex),
              exitPoint(std::move(exitPoint)), ukExitPoint(ukExitPoint){};

        // The code
        std::string intentionCode;

        // The index on the extracted route at which the aircraft leaves the FIR
        int exitPointIndex;

        // The index on the extracted route at which the aircraft leaves the UK airspace
        int ukExitPointIndex;

        // The exit point itself
        std::string exitPoint;

        // The UK exit point itself
        std::string ukExitPoint;

        inline static const int INVALID_EXIT_POINT = -1;
    };
} // namespace UKControllerPlugin::IntentionCode
