#pragma once
#include "acars/IncomingAcarsMessage.h"

namespace UKControllerPlugin {
    namespace Acars {

        /*
            Represents an incoming ACARS message
        */
        class AcarsMessageHandler
        {
            public:

                /*
                    Handle an incoming ACARS message. Returns true if the message has been
                    successfully handled, false otherwise.
                */
                virtual bool HandleAcarsMessage(const IncomingAcarsMessage & message) = 0;
        };
    }  // namespace Acars
}  // namespace UKControllerPlugin
