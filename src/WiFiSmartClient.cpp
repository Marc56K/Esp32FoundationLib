#include "WiFiSmartClient.h"

namespace esp32
{
    namespace foundation
    {
        WiFiSmartClient::WiFiSmartClient()
        {
        }

        WiFiSmartClient::~WiFiSmartClient()
        {
            Disconnect();
        }

        bool WiFiSmartClient::Connect(
                const String &wifiSSID,
                const String &wifiKey,
                const String &hostName,
                const uint32_t timeoutInMillis)
        {
            if (_wifiSSID != wifiSSID || _wifiKey != wifiKey || _hostName != hostName)
            {
                Disconnect();
            }

            if (_eventIds.empty())
            {
                _eventIds.push_back(
                    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
                        Serial.print("Obtained IP address: ");
                        Serial.println(WiFi.localIP());
                    }, WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP));

                _eventIds.push_back(
                    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
                        WiFi.reconnect();
                    }, WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED));

                _wifiSSID = wifiSSID;
                _wifiKey = wifiKey;
                _hostName = hostName;

                WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
                WiFi.setHostname(_hostName.c_str());
                WiFi.begin(_wifiSSID.c_str(), _wifiKey.c_str());
            }

            uint32_t t = timeoutInMillis;
            while (!Connected() && t > 0)
            {
                const uint32_t waitTime = std::min(10u, t);
                vTaskDelay(waitTime);
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

        std::vector<String> WiFiSmartClient::ScanNetworks()
        {
            WiFi.scanDelete();
            const int16_t numWifis = WiFi.scanNetworks(false, false);
            std::vector<String> result;
            result.reserve(numWifis);
            for (int16_t i = 0; i < numWifis; i++)
            {
                result.push_back(WiFi.SSID(i));        
            }
            std::sort(result.begin(), result.end());
            return result;
        }
    }
}