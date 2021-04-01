#include <Arduino.h>
#include <Esp32Foundation.h>

using namespace esp32::foundation;

HtmlConfigurator* configurator = nullptr;
WiFiSmartClient wifiClient;

StringParameter wifiSsid("wifi_ssid", "MyWifi");
StringParameter wifiKey("wifi_key", "");
StringParameter hostname("hostname", "MyEsp");

void setup()
{
    Serial.begin(9600);

    // try for 5 seconds to connect
    if(!wifiClient.Connect(wifiSsid, wifiKey, hostname, 5000))
    {
        Serial.println("Can't connect to WiFi!");

        // stop auto-reconnect
        wifiClient.Disconnect();

        // start wifi-hotspot
        configurator = new HtmlConfigurator();
        if (configurator->Start("EspSetup", "test1234", hostname))
        {
            Serial.println("WiFi-Hotspot created");
        }
        else
        {
            delete configurator;
            configurator = nullptr;
            Serial.println("Can't create WiFi-Hotspot!");
        }
    }
}

void loop()
{
    if (configurator != nullptr)
    {
        if (configurator->IsStopped())
        {
            Serial.println("Configuration done!");
            delete configurator;
            configurator = nullptr;
            wifiClient.Connect(wifiSsid, wifiKey, hostname, 0);
        }
        else
        {
            configurator->Update();
        }
    }
    delay(1000);
}