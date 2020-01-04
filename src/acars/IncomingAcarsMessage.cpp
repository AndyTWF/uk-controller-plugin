#include "pch/stdafx.h"
#include "acars/IncomingAcarsMessage.h"

namespace UKControllerPlugin {
    namespace Acars {

        IncomingAcarsMessage::IncomingAcarsMessage(std::string from, std::string type, std::string message)
            : from(from), type(type), message(message)
        {

        }
    }  // namespace Acars
}  // namespace UKControllerPlugin
