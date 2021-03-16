#pragma once
#include <WebServer.h>
#include <DNSServer.h>
#include <functional>
#include <map>

namespace esp32
{
    namespace foundation
    {
        class CaptivePortal
        {
        public:
            CaptivePortal(const char* ip = "192.168.3.1");
            ~CaptivePortal();

            void On(
                const String& uri,
                std::function<void(WebServer&)> callback);

            bool Start(
                const String& wifiSSID,
                const String& wifiKey,
                const String& hostName);

            void Stop();

            bool IsStopped() const;
            void Update();

        private:
            void HandleNotFound();

        private:
            IPAddress _apIP;
            DNSServer _dnsServer;
            WebServer _httpServer;
            bool _isStopped;

            std::map<String, std::function<void(WebServer&)>> _callbacks; 

            String _wifiSSID;
            String _wifiKey;
            String _hostName;

            uint64_t _registerA;
            uint64_t _registerB;
            uint64_t _registerC;
        };
    }
}