#include "pch/stdafx.h"
#include "acars/AcarsMessageHandlerCollection.h"

namespace UKControllerPlugin {
    namespace Acars {

        /*
            Returns the number of registered handlers.
        */
        int AcarsMessageHandlerCollection::CountHandlers(void) const
        {
            return this->handlerList.size();
        }

        /*
            Handle incoming ACARS messages
        */
        void AcarsMessageHandlerCollection::HandleIncomingMessage(const IncomingAcarsMessage & message) const
        {
            for (
                std::list<std::shared_ptr<AcarsMessageHandler>>::const_iterator it =
                this->handlerList.cbegin();
                it != this->handlerList.cend();
                ++it
            ) {
                if ((*it)->HandleAcarsMessage(message)) {
                    break;
                }
            }
        }

        /*
            Registers an object to handle and event.
        */
        void AcarsMessageHandlerCollection::RegisterHandler(std::shared_ptr<AcarsMessageHandler> handler)
        {
            if (std::find(this->handlerList.begin(), this->handlerList.end(), handler) != this->handlerList.end()) {
                return;
            }

            this->handlerList.push_back(handler);
        }
    }  // namespace Flightplan
}  // namespace UKControllerPlugin
