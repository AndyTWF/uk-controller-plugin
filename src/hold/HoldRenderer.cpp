#include "pch/stdafx.h"
#include "hold/HoldRenderer.h"
#include "hold/HoldManager.h"
#include "hold/HoldDisplay.h"
#include "hold/HoldDisplayFactory.h"
#include "helper/HelperFunctions.h"
#include "euroscope/UserSetting.h"

using UKControllerPlugin::Euroscope::UserSetting;
using UKControllerPlugin::Euroscope::EuroscopeRadarLoopbackInterface;
using UKControllerPlugin::Windows::GdiGraphicsInterface;
using UKControllerPlugin::Hold::HoldManager;
using UKControllerPlugin::Hold::HoldDisplay;
using UKControllerPlugin::Hold::HoldDisplayFactory;
using UKControllerPlugin::Plugin::PopupMenuItem;
using UKControllerPlugin::HelperFunctions;
using UKControllerPlugin::Euroscope::UserSetting;

namespace UKControllerPlugin {
    namespace Hold {

        HoldRenderer::HoldRenderer(
            const std::shared_ptr<const HoldDisplayManager> displays,
            const int screenObjectId,
            const int toggleCallbackFunctionId
        )
            : displays(displays), screenObjectId(screenObjectId), toggleCallbackFunctionId(toggleCallbackFunctionId)
        {

        }

        /*
            Load user settings from the ASR - whether to render the hold displays.
        */
        void HoldRenderer::AsrLoadedEvent(UserSetting & userSetting)
        {
            this->renderHolds = userSetting.GetBooleanEntry(
                this->asrVisibleKey,
                true
            );
        }

        /*
            Save user settings to the ASR - whether to render the hold displays.
        */
        void HoldRenderer::AsrClosingEvent(UserSetting & userSetting)
        {
            userSetting.Save(
                this->asrVisibleKey,
                this->asrVisibleDescription,
                this->renderHolds
            );
        }

        /*
            Should holds be rendered
        */
        bool HoldRenderer::IsVisible(void) const
        {
            return this->renderHolds;
        }

        /*
            A button has been left clicked
        */
        void HoldRenderer::LeftClick(
            EuroscopeRadarLoopbackInterface& radarScreen,
            int objectId,
            std::string objectDescription,
            POINT mousePos,
            RECT itemArea
        ) {
            std::string holdName = this->GetHoldNameFromObjectDescription(objectDescription);
            auto display = std::find_if(
                this->displays->cbegin(),
                this->displays->cend(),
                [holdName](const std::unique_ptr<HoldDisplay>& hold) -> bool {
                    return hold->navaid.identifier == holdName;
                }
            );

            if (display == this->displays->cend()) {
                LogWarning("Tried to interact with invalid hold display");
                return;
            }

            if (objectDescription.find("cleared") != std::string::npos) {
                (*display)->ClearedLevelClicked(
                    this->GetCallsignFromObjectDescription(objectDescription),
                    radarScreen,
                    mousePos,
                    itemArea
                );
                return;
            }

            if (objectDescription.find("callsign") != std::string::npos) {
                (*display)->CallsignClicked(
                    this->GetCallsignFromObjectDescription(objectDescription),
                    radarScreen,
                    mousePos,
                    itemArea
                );
                return;
            }

            (*display)->ButtonClicked(this->GetButtonNameFromObjectDescription(objectDescription));
        }

        /*
            For now, nothing accepts a right click
        */
        void HoldRenderer::RightClick(
            int objectId,
            std::string objectDescription,
            EuroscopeRadarLoopbackInterface & radarScreen
        ) {

        }

        /*
            Move a given hold display
        */
        void HoldRenderer::Move(RECT position, std::string objectDescription)
        {
            std::string holdName = this->GetHoldNameFromObjectDescription(objectDescription);
            auto display = std::find_if(
                this->displays->cbegin(),
                this->displays->cend(),
                [holdName](const std::unique_ptr<HoldDisplay> & hold) -> bool {
                    return hold->navaid.identifier == holdName;
                }
            );

            if (display == this->displays->cend()) {
                LogWarning("Tried to move invalid hold display");
                return;
            }

            (*display)->Move({ position.left, position.top });
        }

        /*
            Render all the hold displays
        */
        void HoldRenderer::Render(
            GdiGraphicsInterface & graphics,
            EuroscopeRadarLoopbackInterface & radarScreen
        ) {
            for (
                HoldDisplayManager::const_iterator it = this->displays->cbegin();
                it != this->displays->cend();
                ++it
            ) {
                (*it)->PaintWindow(graphics, radarScreen, this->screenObjectId);
            }
        }

        /*
            Set whether or not holds should be rendered.
        */
        void HoldRenderer::SetVisible(bool visible)
        {
            this->renderHolds = visible;
        }

        /*
            Reset the position of each hold display
        */
        void HoldRenderer::ResetPosition(void)
        {
            for (
                HoldDisplayManager::const_iterator it = this->displays->cbegin();
                it != this->displays->cend();
                ++it
            ) {
                (*it)->Move({ 100, 100 });
            }
        }

        /*
            Toggle hold visibility
        */
        void HoldRenderer::Configure(int functionId, std::string subject, RECT screenObjectArea)
        {
            this->renderHolds = !this->renderHolds;
        }

        /*
            Return the configuration menu item for this class
        */
        UKControllerPlugin::Plugin::PopupMenuItem HoldRenderer::GetConfigurationMenuItem(void) const
        {
            PopupMenuItem returnVal;
            returnVal.firstValue = this->menuItemDescription;
            returnVal.secondValue = "";
            returnVal.callbackFunctionId = this->toggleCallbackFunctionId;
            returnVal.checked = this->renderHolds;
            returnVal.disabled = false;
            returnVal.fixedPosition = false;
            return returnVal;
        }

        /*
            Given an item description, return the hold id
        */
        std::string HoldRenderer::GetHoldNameFromObjectDescription(std::string objectDescription) const
        {
            return objectDescription.substr(0, objectDescription.find("/"));
        }

        std::string HoldRenderer::GetCallsignFromObjectDescription(std::string objectDescription) const
        {
            return objectDescription.substr(objectDescription.find_last_of("/") + 1);
        }

        /*
            Given an item description, return the button name
        */
        std::string HoldRenderer::GetButtonNameFromObjectDescription(std::string objectDescription) const
        {
            return objectDescription.substr(objectDescription.find("/") + 1);
        }
    }  // namespace Hold
}  // namespace UKControllerPlugin
