#pragma once
#include "acars/IncomingAcarsMessage.h"
#include "acars/AcarsMessageHandler.h"

namespace UKControllerPlugin {
    namespace Acars {

        /*
            A repository of event handlers for incoming ACARS messages
        */
        class AcarsMessageHandlerCollection
        {
            public:
                int CountHandlers(void) const;
                void HandleIncomingMessage(const IncomingAcarsMessage & message) const;
                void RegisterHandler(std::shared_ptr<AcarsMessageHandler> handler);

            private:
                // Vector of registered handlers
                std::list<std::shared_ptr<AcarsMessageHandler>> handlerList;
        };
    }  // namespace Acars
}  // namespace UKControllerPlugin
