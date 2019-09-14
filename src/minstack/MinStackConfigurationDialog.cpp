#include "pch/stdafx.h"
#include "minstack/MinStackConfigurationDialog.h"
#include "dialog/DialogCallArgument.h"
#include "helper/HelperFunctions.h"

using UKControllerPlugin::Dialog::DialogCallArgument;
using UKControllerPlugin::MinStack::MinStackRendererConfiguration;
using UKControllerPlugin::MinStack::MinStackManager;
using UKControllerPlugin::HelperFunctions;

namespace UKControllerPlugin {
    namespace MinStack {

        MinStackConfigurationDialog::MinStackConfigurationDialog(const MinStackManager & manager)
            : manager(manager)
        {

        }

        /*
            Public facing window procedure
        */
        LRESULT MinStackConfigurationDialog::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
        {
            if (msg == WM_INITDIALOG) {
                LogInfo("Minstack configuration dialog opened");
                SetWindowLongPtr(
                    hwnd,
                    GWLP_USERDATA,
                    reinterpret_cast<DialogCallArgument *>(lParam)->dialogArgument
                );
            } else if (msg == WM_DESTROY) {
                SetWindowLongPtr(hwnd, GWLP_USERDATA, NULL);
                LogInfo("Minstack configuration dialog closed");
            }

            MinStackConfigurationDialog * dialog = reinterpret_cast<MinStackConfigurationDialog*>(
                GetWindowLongPtr(hwnd, GWLP_USERDATA)
            );
            return dialog ? dialog->_WndProc(hwnd, msg, wParam, lParam) : FALSE;
        }

        /*
            Private window procedure bound to the objects
        */
        LRESULT MinStackConfigurationDialog::_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
        {
            switch (msg) {
                // Initialise
                case WM_INITDIALOG: {
                    this->InitDialog(hwnd, lParam);
                    return TRUE;
                };
                // Dialog Closed
                case WM_CLOSE: {
                    EndDialog(hwnd, wParam);
                    return TRUE;
                }
                // Buttons pressed
                case WM_COMMAND: {
                    switch (LOWORD(wParam)) {
                        case IDOK: {
                            this->SaveDialog(hwnd);
                            EndDialog(hwnd, wParam);
                            return TRUE;
                        }
                        case IDCANCEL: {
                            EndDialog(hwnd, wParam);
                            return TRUE;
                        }
                    }
                }
            }

            return FALSE;
        }

        /*
            Initialise the controls from config
        */
        void MinStackConfigurationDialog::InitDialog(HWND hwnd, LPARAM lParam)
        {
            this->config = reinterpret_cast<MinStackRendererConfiguration *>(
                reinterpret_cast<DialogCallArgument *>(lParam)->contextArgument
            );

            for (
                MinStackRendererConfiguration::const_iterator it = this->config->cbegin();
                it != this->config->cend();
                ++it
            ) {
                HRESULT itemIndex = SendDlgItemMessage(
                    hwnd,
                    IDC_MINSTACK_SELECT,
                    CB_ADDSTRING,
                    NULL,
                    reinterpret_cast<LPARAM>(this->GetListEntryForKey(it->key).c_str())
                );

                SendDlgItemMessage(
                    hwnd,
                    IDC_MINSTACK_SELECT,
                    CB_SETITEMDATA,
                    itemIndex,
                    reinterpret_cast<LPARAM>(it->key.c_str())
                );
            }
        }

        /*
            Save the controls to config
        */
        void MinStackConfigurationDialog::SaveDialog(HWND hwnd)
        {
            
        }

        /*
            Transform each MSL key to how it should be displayed
        */
        std::wstring MinStackConfigurationDialog::GetListEntryForKey(std::string mslKey)
        {
            std::string facility = mslKey.substr(0, 3) == "tma" ? "TMA" : "Airfield";
            std::string identifier = facility == "TMA" ? mslKey.substr(4) : mslKey.substr(9);

            return HelperFunctions::ConvertToWideString(facility + " - " + identifier);
        }
    }  // namespace MinStack
}  // namespace UKControllerPlugin