#include <Arduino.h>
#include <Esp32Foundation.h>

using namespace esp32::foundation;

HtmlConfigurator configurator;

FloatParameter x(
    "x", // parameter name
    0.5f,  // default value
    1,     // decimal places
    0.0f,  // min value
    1.0f); // max value

StringParameter foo(
    "foo",  // parameter name
    "bar"); // default value

void setup()
{
    Serial.begin(9600);
    configurator.Start("MyCaptivePortal", "topsecret", "ESP");

    delay (1000);
    Serial.println("WiFi-Hotspot created: 'MyCaptivePortal'");
}

void loop()
{
    configurator.Update();
    if (configurator.IsStopped())
    {
        Serial.println("Configuration done!");
        Serial.println(x.Name + "=" + x);
        Serial.println(foo.Name + "=" + foo);
        delay(5000);
    }

    delay(100);
}