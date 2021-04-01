#include <Arduino.h>
#include <Esp32Foundation.h>

using namespace esp32::foundation;

WiFiSmartClient wifiClient;

StringParameter wifiSsid("wifi_ssid", "MyWifi");
StringParameter wifiKey("wifi_key", "topsecret");
StringParameter hostname("hostname", "MyEsp");

void setup()
{
    Serial.begin(9600);

    wifiClient.Connect(wifiSsid, wifiKey, hostname, 0);
}

void loop()
{
    if (wifiClient.Connected())
    {
        Serial.println("online");
    }
    else
    {
        Serial.println("offline");
    }
    delay(1000);
}