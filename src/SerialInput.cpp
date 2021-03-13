#include "SerialInput.h"
#include "StringUtils.h"

#define MAX_BUFFER_SIZE 1024

namespace esp32
{
    namespace foundation
    {
        SerialInput::SerialInput(HardwareSerial &serial)
            : _serial(serial)
        {
            _buffer.reserve(128);
        }

        SerialInput::~SerialInput()
        {
        }

        bool SerialInput::ReadCommand(String &cmd, String &arg)
        {
            cmd = "";
            arg = "";
            while (_serial.available())
            {
                const char c = _serial.read();
                _serial.print(c);
                if (c == '\r' || c == '\n')
                {
                    _buffer.trim();
                    if (_buffer.length() > 0)
                    {
                        StringUtils::SplitKeyValue(_buffer, " ", cmd, arg);
                    }
                    _buffer.clear();
                    return !cmd.isEmpty();
                }
                else if (_buffer.length() >= MAX_BUFFER_SIZE)
                {
                    _buffer.clear();
                    _serial.flush();
                    break;
                }
                else
                {
                    _buffer += c;
                }
            }

            return false;
        }
    }
}