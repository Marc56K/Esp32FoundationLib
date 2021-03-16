#include "CaptivePortal.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <ESPmDNS.h>
#include <soc/sens_reg.h>

#define DNS_PORT 53
#define GPIO_OUT_W1TS_REG (DR_REG_GPIO_BASE + 0x0008)
#define GPIO_OUT_W1TC_REG (DR_REG_GPIO_BASE + 0x000c)

IPAddress apIP(192, 168, 3, 1);
IPAddress netMsk(255, 255, 255, 0);

namespace esp32
{
    namespace foundation
    {
        CaptivePortal::CaptivePortal()
            : _httpServer(80),
              _isStopped(true),
              _registerA(0),
              _registerB(0),
              _registerC(0)
        {
            _callbacks["/"] = [](WebServer& httpServer)
            {
                httpServer.sendContent("CaptivePortal not configured!");
            };
        }

        CaptivePortal::~CaptivePortal()
        {
        }

        void CaptivePortal::On(
                const String& uri,
                std::function<void(WebServer&)> callback)
        {
            _callbacks[uri] = callback;
        }

        bool CaptivePortal::Start(
            const String &wifiSSID,
            const String &wifiKey,
            const String &hostName)
        {
            Stop();

            _isStopped = false;
            _wifiSSID = wifiSSID;
            _wifiKey = wifiKey;
            _hostName = hostName;
            
            // save some register states that wifi will screw up
            _registerA = READ_PERI_REG(SENS_SAR_START_FORCE_REG);
            _registerB = READ_PERI_REG(SENS_SAR_READ_CTRL2_REG);
            _registerC = READ_PERI_REG(SENS_SAR_MEAS_START2_REG);

            // init wifi
            WiFi.setAutoReconnect(false);
            WiFi.persistent(false);
            WiFi.disconnect();
            WiFi.setHostname(_hostName.c_str()); // Set the DHCP hostname assigned to ESP station.
            WiFi.disconnect();
            if (!WiFi.softAP(_wifiSSID.c_str(), _wifiKey.c_str()))
            {
                Stop();
                return false;
            }
            else
            {
                delay(2000);
                WiFi.softAPConfig(apIP, apIP, netMsk);
                _dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
                _dnsServer.start(DNS_PORT, "*", apIP);
            }

            // init http server
            _httpServer.onNotFound([this]() { HandleNotFound(); });
            for (auto& callback : _callbacks)
            {
                auto contentCallback = callback.second;
                _httpServer.on(callback.first, [this, contentCallback]()
                {
                    // HTML Header
                    _httpServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
                    _httpServer.sendHeader("Pragma", "no-cache");
                    _httpServer.sendHeader("Expires", "-1");

                    contentCallback(_httpServer);
                });
            }            

            // Web server start
            _httpServer.begin();

            return true;
        }

        void CaptivePortal::Stop()
        {
            if (!IsStopped())
            {
                _isStopped = true;
                WiFi.disconnect();
                WiFi.mode(WIFI_OFF);

                // fix ADC2 registers to make analog pin reads work again
                WRITE_PERI_REG(SENS_SAR_START_FORCE_REG, _registerA);
                WRITE_PERI_REG(SENS_SAR_READ_CTRL2_REG, _registerB);
                WRITE_PERI_REG(SENS_SAR_MEAS_START2_REG, _registerC);
            }
        }

        bool CaptivePortal::IsStopped() const
        {
            return _isStopped;
        }

        void CaptivePortal::Update()
        {
            if (!IsStopped())
            {
                _dnsServer.processNextRequest();
                _httpServer.handleClient();
            }
        }

        void CaptivePortal::HandleNotFound()
        {
            auto isIp = [](const String& str) -> bool
            {
                for (int i = 0; i < str.length(); i++)
                {
                    int c = str[i];
                    if (c != '.' && (c < '0' || c > '9'))
                    {
                        return false;
                    }
                }
                return true;
            };

            if (!isIp(_httpServer.hostHeader()) && _httpServer.hostHeader() != (_hostName + ".local"))
            {
                const String url = String("http://") + _httpServer.client().localIP().toString();
                _httpServer.sendHeader("Location", url, true);
                _httpServer.send(302, "text/plain", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
                _httpServer.client().stop();             // Stop is needed because we sent no content length
            }
        }
    }
}