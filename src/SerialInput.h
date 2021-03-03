#pragma once
#include <Arduino.h>

class SerialInput
{
public:
    SerialInput(HardwareSerial& serial = Serial);
    ~SerialInput();

    bool ReadCommand(String &cmd, String &arg);

private:
    bool SplitKeyValue(const String &input, const String& delimiter, String &key, String &value);

private:
    HardwareSerial& _serial;
};