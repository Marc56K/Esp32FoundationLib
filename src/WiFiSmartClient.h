#pragma once
#include <string>
#include <vector>
#include <WiFi.h>

namespace esp32
{
    namespace foundation
    {
        class WiFiSmartClient
        {
        public:
            WiFiSmartClient();
            ~WiFiSmartClient();

            bool Connect(
                const String &wifiSSID,
                const String &wifiKey,
                const String &hostName,
                const uint32_t timeoutInMillis = 5000);
            void Disconnect();
            bool Connected();

        private:
            String _wifiSSID;
            String _wifiKey;
            String _hostName;
            std::vector<WiFiEventId_t> _eventIds;
        };
    }
}