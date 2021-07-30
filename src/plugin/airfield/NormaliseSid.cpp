#include "pch/pch.h"
#include "airfield/NormaliseSid.h"

namespace UKControllerPlugin {
    namespace Airfield {

        /*
            Strips the deprecation marks from a SID (leading #).
            Returns the original if there are no deprecation marks.
        */
        std::string NormaliseSid::StripSidDeprecation(std::string sid) const
        {
            // Return the SID with all instances of the deprecation character removed.
            sid.erase(std::remove(sid.begin(), sid.end(), this->sidDeprecationPrefix), sid.end());
            return sid;
        }
    }  // namespace Airfield
}  // namespace UKControllerPlugin
