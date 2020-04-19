#include "pch/stdafx.h"
#include "srd/SrdSearchHandler.h"

using UKControllerPlugin::Dialog::DialogManager;
using UKControllerPlugin::Plugin::PopupMenuItem;

namespace UKControllerPlugin {
    namespace Srd {

        SrdSearchHandler::SrdSearchHandler(const int menuCallbackId, const DialogManager& dialog)
            : menuCallbackId(menuCallbackId), dialog(dialog)
        {
        }

        void SrdSearchHandler::Configure(int functionId, std::string subject, RECT screenObjectArea)
        {
            this->dialog.OpenDialog(IDD_SRD_SEARCH, NULL);
        }

        UKControllerPlugin::Plugin::PopupMenuItem SrdSearchHandler::GetConfigurationMenuItem(void) const
        {
            PopupMenuItem returnVal;
            returnVal.firstValue = this->itemDescription;
            returnVal.secondValue = "";
            returnVal.callbackFunctionId = this->menuCallbackId;
            returnVal.checked = EuroScopePlugIn::POPUP_ELEMENT_NO_CHECKBOX;
            returnVal.disabled = false;
            returnVal.fixedPosition = false;
            return returnVal;
        }
    }  // namespace Srd
}  // UKControllerPlugin
