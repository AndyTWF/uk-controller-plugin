#pragma once

namespace UKControllerPlugin {
    namespace Windows {
        class WinApiInterface;
    }  // namespace Windows
    namespace Setting {
        class SettingRepository;
    }  // namespace Setting
}  // namespace UKControllerPlugin


namespace UKControllerPlugin {
    namespace Api
    {

    void LocateApiSettings(
        UKControllerPlugin::Windows::WinApiInterface & winApi,
        UKControllerPlugin::Setting::SettingRepository & settings
    );

    bool UserRequestedKeyUpdateNoPrompts(
        UKControllerPlugin::Windows::WinApiInterface & winApi,
        UKControllerPlugin::Setting::SettingRepository & settings
    );
    bool ReplaceApiSettings(UKControllerPlugin::Windows::WinApiInterface & winApi);
    void UserRequestedKeyUpdate(UKControllerPlugin::Windows::WinApiInterface & winApi);
    } // namespace Api
}  // namespace UKControllerPlugin
