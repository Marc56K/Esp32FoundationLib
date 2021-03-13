#include "WiFiSmartClient.h"

namespace esp32
{
    namespace foundation
    {
        WiFiSmartClient::WiFiSmartClient(
            const char *wifiSSID,
            const char *wifiKey,
            const char *hostName)
            : _wifiSSID(wifiSSID),
              _wifiKey(wifiKey),
              _hostName(hostName)
        {
        }

        WiFiSmartClient::~WiFiSmartClient()
        {
            Disconnect();
        }

        bool WiFiSmartClient::Connect(const uint32_t timeoutInMillis)
        {
            if (_eventIds.empty())
            {
                _eventIds.push_back(
                    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
                        Serial.print("Obtained IP address: ");
                        Serial.println(WiFi.localIP());
                    },
                                 WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP));

                _eventIds.push_back(
                    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
                        WiFi.reconnect();
                    },
                                 WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED));

                WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
                WiFi.setHostname(_hostName.c_str());
                WiFi.begin(_wifiSSID.c_str(), _wifiKey.c_str());
            }

            uint32_t t = timeoutInMillis;
            while (!Connected() && t > 0)
            {
                const uint32_t waitTime = std::min(10u, t);
                vTaskDelay(waitTime / portTICK_PERIOD_MS);
                t -= waitTime;
            }

            return Connected();
        }

        void WiFiSmartClient::Disconnect()
        {
            for (auto &eventId : _eventIds)
            {
                WiFi.removeEvent(eventId);
            }
            _eventIds.clear();
            WiFi.disconnect();
        }

        bool WiFiSmartClient::Connected()
        {
            return WiFi.isConnected();
        }
    }
}