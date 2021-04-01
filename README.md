# Esp32FoundationLib

## Loading and storing of parameters from EEPROM:

```cpp
#include <Arduino.h>
#include <Esp32Foundation.h>

using namespace esp32::foundation;

IntegerParameter sleepDuration(
    "sleep_duration", // parameter name
    1000,  // default value
    10,    // min value
    5000); // max value

StringParameter wifiSsid(
    "wifi_ssid",  // parameter name
    "MyWifi");    // default value

StringParameter wifiKey(
    "wifi_key",   // parameter name
    "TopSecret"); // default value

void printParams()
{
    Serial.println(sleepDuration.Name + "=" + sleepDuration);
    Serial.println(wifiSsid.Name + "=" + wifiSsid);
    Serial.println(wifiKey.Name + "=" + wifiKey);
}

void setup()
{
    Serial.begin(9600);
    Serial.setTimeout(60000);
}

void loop()
{
    if (Serial.available())
    {
        String commandLine = Serial.readStringUntil('\n');
        if (commandLine.endsWith("\r"))
        {
            // remove carriage return 
            commandLine.remove(commandLine.length() - 1);
        }

        Serial.println(String("INPUT: ") + commandLine);
        
        // e.g.: sleep_duration=500
        if (commandLine.startsWith("sleep_duration="))
        {
            String value = commandLine.substring(15);
            sleepDuration.SetFromString(value);
        }

        // e.g.: wifi_ssid=blabla
        if (commandLine.startsWith("wifi_ssid="))
        {
            String value = commandLine.substring(10);
            wifiSsid.SetFromString(value);
        }

        // e.g.: wifi_key=foobar
        if (commandLine.startsWith("wifi_key="))
        {
            String value = commandLine.substring(9);
            wifiKey.SetFromString(value);
        }

        // save parameter values to eeprom
        if (commandLine == "save")
        {
            DefaultParameterSet.SaveToEEPROM();
            Serial.println("parameters saved to EEPROM");
        }

        printParams();
    }

    delay(sleepDuration);
}
```
