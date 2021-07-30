#include "pch/pch.h"
#include "regional/RegionalPressureManager.h"

using UKControllerPlugin::TaskManager::TaskRunnerInterface;
using UKControllerPlugin::HelperFunctions;
using UKControllerPlugin::Push::PushEventSubscription;
using UKControllerPlugin::Push::PushEvent;

namespace UKControllerPlugin {
    namespace Regional {

        RegionalPressureManager::RegionalPressureManager(void)
        {
        }

        RegionalPressureManager::RegionalPressureManager(std::map<std::string, std::string> keyMap)
            : keyMap(keyMap)
        {
        }

        void RegionalPressureManager::AcknowledgePressure(std::string key)
        {
            if (!this->pressureMap.count(key)) {
                return;
            }

            this->pressureMap.at(key).Acknowledge();
        }

        /*
            Add a MSL with the specified key
        */
        void RegionalPressureManager::AddRegionalPressure(std::string key, std::string name, unsigned int pressure)
        {
            if (this->pressureMap.count(key)) {
                return;
            }

            this->pressureMap[key] = {
                key,
                name,
                pressure
            };
        }

        size_t RegionalPressureManager::CountAltimeterSettingRegions(void) const
        {
            return this->keyMap.size();
        }

        /*
            Get all keys for MSLs
        */
        std::set<std::string> RegionalPressureManager::GetAllRegionalPressureKeys(void) const
        {
            std::set<std::string> keys;
            for (
                std::map<std::string, RegionalPressure>::const_iterator it = this->pressureMap.cbegin();
                it != this->pressureMap.cend();
                ++it
            ) {
                keys.insert(it->first);
            }

            return keys;
        }

        /*
            Get the selected minstack level
        */
        const RegionalPressure & RegionalPressureManager::GetRegionalPressure(std::string key) const
        {
            return this->pressureMap.count(key)
                ? this->pressureMap.at(key)
                : this->invalidPressure;
        }

        /*
            Get the name part of the key
        */
        std::string RegionalPressureManager::GetNameFromKey(std::string key) const
        {
            return this->keyMap.count(key) ? this->keyMap.at(key) : key;
        }

        /*
            We've received some new MSLs from the web API, update them locally
        */
        void RegionalPressureManager::ProcessPushEvent(const PushEvent& message)
        {
            if (message.event != "App\\Events\\RegionalPressuresUpdatedEvent") {
                return;
            }

            if (!message.data.is_object() || !message.data.count("pressures")) {
                return;
            }

            this->UpdateAllPressures(message.data.at("pressures"));
        }

        std::set<PushEventSubscription> RegionalPressureManager::GetPushEventSubscriptions(void) const
        {
            return {
                {
                    PushEventSubscription::SUB_TYPE_CHANNEL,
                    "private-rps-updates"
                }
            };
        }

        void RegionalPressureManager::PluginEventsSynced()
        {
            // Nothing here
        }

        /*
            Set the MSL for the given key
        */
        void RegionalPressureManager::SetPressure(std::string key, unsigned int pressure)
        {
            if (!this->pressureMap.count(key)) {
                return;
            }

            this->pressureMap.at(key).pressure = pressure;
            this->pressureMap.at(key).updatedAt = std::chrono::system_clock::now();
        }

        void RegionalPressureManager::UpdateAllPressures(nlohmann::json pressureData)
        {
            // Update the airfield pressures
            if (!pressureData.is_object()) {
                LogWarning("Invalid regional pressure data");
            }

            for (
                nlohmann::json::const_iterator it = pressureData.cbegin();
                it != pressureData.cend();
                ++it
            ) {
                if (it.value().is_null()) {
                    continue;
                }

                if (this->pressureMap.count(it.key())) {
                    this->pressureMap[it.key()].pressure = it.value().get<unsigned int>();
                } else {
                    this->pressureMap[it.key()] = {
                        it.key(),
                        this->keyMap.count(it.key()) ? this->keyMap.at(it.key()) : it.key(),
                        it.value().get<unsigned int>()
                    };
                }
            }
        }
    }  // namespace Regional
}  // namespace UKControllerPlugin
