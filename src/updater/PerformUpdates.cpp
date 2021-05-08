#include "updater/pch.h"
#include "updater/PerformUpdates.h"
#include "helper/HelperFunctions.h"
#include "api/ApiInterface.h"
#include "windows/WinApiInterface.h"
#include "curl/CurlInterface.h"
#include "update/UpdateBinaries.h"
#include "data/PluginDataLocations.h"
#include "update/LoadChangelog.h"

using UKControllerPlugin::HelperFunctions;
using UKControllerPlugin::Api::ApiInterface;
using UKControllerPlugin::Windows::WinApiInterface;
using UKControllerPlugin::Curl::CurlInterface;

bool CheckForUpdates(
    const ApiInterface& api,
    WinApiInterface& windows,
    CurlInterface& curl,
    bool duplicatePlugin
)
{
    if (duplicatePlugin) {
        LogInfo("Skipping updates as plugin is duplicate");
        return true;
    }

    try {
        const nlohmann::json versionDetails = UKControllerPlugin::GetUpdateData(api);
        std::string version = GetVersionFromJson(versionDetails);
        std::wstring wideVersion = HelperFunctions::ConvertToWideString(version);
        if (UpdateRequired(windows, versionDetails)) {
            if (!DisplayPreUpdateConsentNotification(windows, wideVersion)) {
                return false;
            }

            LogInfo("Plugin update available, newest version " + version);
            PerformUpdates(curl, windows, versionDetails);
            DisplayPostUpdateNotification(
                windows,
                wideVersion
            );
        } else {
            LogInfo("Plugin is up to date at version " + version);
        }
    } catch (std::exception exception) {
        std::wstring message = std::wstring(HelperFunctions::ConvertToWideString(exception.what())) + L"\r\n";
        message += L"Plugin will attempt to load with previously downloaded version.";
        LogError("Exception when performing updates, message: " + std::string(exception.what()));

        windows.OpenMessageBox(
            message.c_str(),
            L"UKCP Automatic Update Failed",
            MB_OK | MB_ICONSTOP
        );
    }

    return true;
}

void PerformUpdates(
    CurlInterface& curl,
    WinApiInterface& windows,
    const nlohmann::json& versionDetails
)
{
    MoveOldUpdaterBinary(windows);
    bool updatedSuccessfully = UKControllerPlugin::DownloadCoreLibrary(versionDetails, windows, curl) &&
        UKControllerPlugin::DownloadUpdater(versionDetails, windows, curl);

    if (!updatedSuccessfully) {
        LogError("Error when updating plugin binaries");
        throw std::exception("Failed to update UKCP binaries.");
    }

    UpdateLockfile(windows, GetVersionFromJson(versionDetails));
}

bool UpdateRequired(WinApiInterface& windows, const nlohmann::json& versionDetails)
{
    return !windows.FileExists(GetVersionLockfileLocation()) ||
        windows.ReadFromFile(GetVersionLockfileLocation()) != GetVersionFromJson(versionDetails) ||
        !windows.FileExists(GetCoreBinaryRelativePath());
}

void MoveOldUpdaterBinary(WinApiInterface& windows)
{
    LogInfo("Moving updater binary to old version");
    windows.MoveFileToNewLocation(GetUpdaterBinaryRelativePath(), GetOldUpdaterBinaryRelativePath());
}

std::string GetVersionFromJson(const nlohmann::json& versionDetails)
{
    return versionDetails.at("version").get<std::string>();
}

void DisplayPostUpdateNotification(WinApiInterface& windows, std::wstring version)
{
    std::wstring message = L"The UK Controller Plugin has been updated to version " + version +
        L".\r\n\r\n";
    message +=
        L"You can view the changelog at any time by opening the OP menu and selecting the relevant item.\r\n\r\n";
    message += L"Would you like to view the changelog now?";

    int gotoChangelog = windows.OpenMessageBox(
        message.c_str(),
        L"UKCP Automatic Update Complete",
        MB_YESNO | MB_ICONINFORMATION
    );

    if (gotoChangelog == IDYES) {
        UKControllerPlugin::Update::LoadChangelog(windows);
    }
}

bool DisplayPreUpdateConsentNotification(WinApiInterface& windows, std::wstring version)
{
    std::wstring message = L"A new version of the UK Controller Plugin, version " + version +
        L", is available.\r\n\r\n";
    message += L"Would you like to update now?";

    int updateNow = windows.OpenMessageBox(
        message.c_str(),
        L"UKCP Automatic Update",
        MB_YESNO | MB_ICONINFORMATION
    ) == IDYES;

    if (updateNow == IDOK) {
        return true;
    }

    std::wstring warning = L"In order to use the UK Controller Plugin, you must update to the latest version.\r\n\r\n";
    warning +=
        L"This is because old or buggy versions of the plugin can adversely affect the experience for users ";
    warning += L"of the VATSIM network.\r\n\r\n";
    warning += L"Select yes to update now, or no to disable the plugin.";

    return windows.OpenMessageBox(
        warning.c_str(),
        L"UKCP Automatic Update",
        MB_YESNO | MB_ICONEXCLAMATION
    ) == IDYES;
}

std::wstring GetOldUpdaterLocation()
{
    return GetBinariesFolderRelativePath() + L"/UKControllerPluginUpdater.dll.old";
}

void UpdateLockfile(WinApiInterface& windows, std::string version)
{
    LogInfo("Updating version lockfile");
    windows.WriteToFile(GetVersionLockfileLocation(), version, true, false);
}
