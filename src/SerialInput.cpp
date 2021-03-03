#include "SerialInput.h"

SerialInput::SerialInput(HardwareSerial& serial)
    : _serial(serial)
{
}

SerialInput::~SerialInput()
{
}

bool SerialInput::ReadCommand(String &cmd, String &arg)
{
    cmd = "";
    arg = "";
    String buff = "";
    while (_serial.available())
    {
        char c = _serial.read();
        if (c == '\r' || c == '\n')
        {
            continue;
        }

        buff += c;
        if (_serial.available() == 0)
        {
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }
    }

    buff.trim();

    if (buff.length() > 0)
    {
        SplitKeyValue(buff, " ", cmd, arg);
    }

    return cmd.length() > 0;
}

bool SerialInput::SplitKeyValue(const String &input, const String& delimiter, String &key, String &value)
{
    int idx = input.indexOf(delimiter);
    if (idx > -1)
    {
        key = input.substring(0, idx);
        value = (idx + 1 < input.length()) ? input.substring(idx + 1) : "";
        return true;
    }

    key = input;
    value = "";
    return false;
}