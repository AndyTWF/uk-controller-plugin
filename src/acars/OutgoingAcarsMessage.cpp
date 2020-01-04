#include "pch/stdafx.h"
#include "acars/OutgoingAcarsMessage.h"

namespace UKControllerPlugin {
    namespace Acars {

        OutgoingAcarsMessage::OutgoingAcarsMessage(std::string to, std::string type, std::string message)
            : to(to), type(type), message(message)
        {

        }
    }  // namespace Acars
}  // namespace UKControllerPlugin
