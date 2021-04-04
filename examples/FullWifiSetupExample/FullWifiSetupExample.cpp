#include <Arduino.h>
#include <Esp32Foundation.h>

using namespace esp32::foundation;

HtmlConfigurator configurator;
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

        // register callbacks
        configurator.OnCancel([&]()
        {
            Serial.println("Configuration aborted!");
            configurator.Stop();
        });
        configurator.OnApply([&](ParameterSet& params)
        {
            Serial.println("Configuration completed!");
            params.SaveToEEPROM();
            configurator.Stop();

            ESP.restart();
        });

        // start wifi-hotspot
        if (configurator.Start("EspSetup", "test1234", hostname))
        {
            Serial.println("WiFi-Hotspot created");
        }
    }
}

void loop()
{
    configurator.Update();

    // add your code here ...

    delay(1000);
}