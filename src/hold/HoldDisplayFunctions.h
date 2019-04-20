#pragma once
#include "hold/HoldingData.h"
#include "hold/HoldProfile.h"

namespace UKControllerPlugin {
    namespace Hold {

        extern const unsigned int holdIdNotFound;
        extern const unsigned int minimumVerticalSpeed;
        extern const int minimumLevelOffset;

        TCHAR * ConvertToTchar(std::string string);
        TCHAR * ConvertToTchar(int value);
        std::string ConvertFromTchar(TCHAR * string);
        std::wstring GetLevelDisplayString(int altitude);
        unsigned int GetOccupiedLevel(int altitude, int verticalSpeed);
        unsigned int GetDisplayRow(int holdMax, int occupiedLevel);
        std::wstring GetTimeInHoldDisplayString(const std::chrono::system_clock::time_point & entryTime);
        TCHAR * GetSelectedHoldProfileText(const UKControllerPlugin::Hold::HoldProfile & profile);
    }  // namespace Hold
}  // namespace UKControllerPlugin
