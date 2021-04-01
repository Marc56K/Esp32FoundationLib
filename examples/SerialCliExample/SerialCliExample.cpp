#include <Arduino.h>
#include <Esp32Foundation.h>

using namespace esp32::foundation;

SerialCLI serialCli(Serial);

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

void setup()
{
    Serial.begin(9600);

    serialCli.On("help", [&]() -> bool
    {
        serialCli.PrintCommands();
        return true;
    }, "Prints this help.");

    serialCli.On("restart", [&]() -> bool
    {
        ESP.restart();
        return true;
    }, "Restart the device.");

    serialCli.On("save", [&]() -> bool
    {
        DefaultParameterSet.SaveToEEPROM();
        return true;
    }, "Save configuration to EEPROM.");

    serialCli.On("params", [&]() -> bool
    {
        DefaultParameterSet.PrintParameters(Serial, true);
        return true;
    }, "List parameters.");

    serialCli.On("set", [&](const String& arg0, const String& arg1) -> bool
    {
        auto parameter = DefaultParameterSet.GetParameter(arg0);
        if (parameter != nullptr)
        {
            parameter->SetFromString(arg1);
            return true;
        }
        return false;
    }, "Set parameter.");

    serialCli.On("delete", [&](const String& arg) -> bool
    {
        DefaultParameterSet.Unset(arg);
        return true;
    }, "Delete parameter.");

    delay(3000);
    serialCli.PrintCommands();
}

void loop()
{
    serialCli.Update();
    delay(100);
}