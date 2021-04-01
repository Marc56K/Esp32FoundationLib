# Esp32FoundationLib

## Loading and storing of parameters from EEPROM:

```cpp
#include <Arduino.h>
#include <Esp32Foundation.h>

using namespace esp32::foundation;

IntegerParameter startCounter(
    "start_counter", // parameter name
    0,    // default value
    0,    // min value
    1000); // max value

StringParameter foo(
    "foo",  // parameter name
    "bar"); // default value

void printParams()
{
    Serial.println(startCounter.Name + "=" + startCounter);
    Serial.println(foo.Name + "=" + foo);
}

void setup()
{
    Serial.begin(9600);
    
    // print current parameter values
    printParams();
    
    // change some parameters
    foo = "foobar";
    startCounter = startCounter + 1;
    
    // print new parameter values
    printParams();
    
    // save parameters to EEPROM
    DefaultParameterSet.SaveToEEPROM();    
}
```

## Modify parameters with the serial command-line interface
```cpp
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
```
```
*************************************************************
USAGE:
  delete     : Delete parameter.
  help       : Prints this help.
  params     : List parameters.
  restart    : Restart the device.
  save       : Save configuration to EEPROM.
  set        : Set parameter.
*************************************************************
```
