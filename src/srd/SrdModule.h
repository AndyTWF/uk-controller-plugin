#pragma once
#include "bootstrap/PersistenceContainer.h"
#include "radarscreen/ConfigurableDisplayCollection.h"

namespace UKControllerPlugin {
    namespace Srd {

        extern const int srdDialogTagFunctionId;

        void BootstrapPlugin(UKControllerPlugin::Bootstrap::PersistenceContainer& container);

        void BootstrapRadarScreen(
            UKControllerPlugin::RadarScreen::ConfigurableDisplayCollection & configurables
        );
    }  // namespace Srd
}  // namespace UKControllerPlugin

