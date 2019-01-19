#pragma once

namespace UKControllerPlugin {
    namespace Euroscope {
        class UserSetting;
    }  // namespace Euroscope
}  // namespace UKControllerPlugin

namespace UKControllerPlugin {
    namespace Euroscope {

        /*
            An ATL dialog for the plugins general settings.
        */
        class GeneralSettingsDialog : public CDialog
        {
            DECLARE_DYNAMIC(GeneralSettingsDialog)

            public:

                GeneralSettingsDialog(
                    CWnd * parentWindow,
                    UKControllerPlugin::Euroscope::UserSetting & userSettings
                );
                GeneralSettingsDialog(const GeneralSettingsDialog & newObject);

                BOOL OnInitDialog(void);
                void OnOK(void);

                // Dialog Data
                #ifdef AFX_DESIGN_TIME
                enum { IDD = IDD_GENERAL_SETTINGS };  // namespace Euroscope
                #endif

            protected:
                DECLARE_MESSAGE_MAP()

            private:

                void DoDataExchange(CDataExchange* pDX);

                // The checkbox for whether prenotes should be notfied to the user
                CButton prenoteEnabledCheckbox;

                // The checkbox for whether automatic squawk assignment should be done
                CButton squawksEnabledCheckbox;

                // The checkbox for whether automatic initial altitude assignment should be done
                CButton initialAltitudeEnabledCheckbox;

                // A place where user settings are retrieved and stored
                UKControllerPlugin::Euroscope::UserSetting & userSettings;
        };
    }  // namespace Euroscope
}  // namespace UKControllerPlugin
