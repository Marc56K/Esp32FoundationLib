#include "SerialCLI.h"
#include "StringUtils.h"

#define MAX_BUFFER_SIZE 1024

namespace esp32
{
    namespace foundation
    {
        SerialCLI::SerialCLI(HardwareSerial &serial)
            : _serial(serial)
        {
            _buffer.reserve(128);
        }

        SerialCLI::~SerialCLI()
        {
        }

        void SerialCLI::PrintCommands()
        {
            _serial.println(F("*************************************************************"));
            _serial.println(F("USAGE:"));
            for (auto& handler : _handlers)
            {
                const String& name = handler.first;
                const String& desc = std::get<1>(handler.second);
                _serial.print(F("  "));
                _serial.print(name);
                if (!desc.isEmpty())
                {
                    for (uint8_t i = name.length(); i < 10; i++) _serial.print(" ");
                    _serial.print(F(" : "));
                    _serial.println(desc);
                }
            }
            _serial.println(F("*************************************************************"));
        }

        void SerialCLI::On(
            const String &cmd,
            std::function<bool()> handler,
            const String &description)
        {
            _handlers[cmd] = SerialInputHandler([handler](const String& arg) -> bool
            {
                return handler();
            }, description);
        }

        void SerialCLI::On(
            const String &cmd,
            std::function<bool(const String &arg)> handler,
            const String &description)
        {
            _handlers[cmd] = SerialInputHandler(handler, description);
        }

        void SerialCLI::On(
            const String &cmd,
            std::function<bool(const String &arg0, const String &arg1)> handler,
            const String &description)
        {
            _handlers[cmd] = SerialInputHandler([handler](const String& arg) -> bool
            {
                String arg0, arg1;
                StringUtils::SplitKeyValue(arg, " ", arg0, arg1);
                return handler(arg0, arg1);
            }, description);
        }

        void SerialCLI::Update()
        {
            String cmd, arg;
            if (ReadCommand(cmd, arg))
            {
                auto handlerIt = _handlers.find(cmd);
                if (handlerIt != _handlers.end())
                {
                    auto handler = std::get<0>(handlerIt->second);
                    if (handler(arg))
                    {
                        _serial.println(F("done"));
                        return;
                    }
                }
                else
                {
                    _serial.print(F("Unknown command: '"));
                    _serial.print(cmd);
                    _serial.println(F("'"));
                }
                PrintCommands();
            }
        }

        bool SerialCLI::ReadCommand(String &cmd, String &arg)
        {
            cmd = "";
            arg = "";
            while (_serial.available())
            {
                const char c = _serial.read();
                if (c == '\r' || c == '\n')
                {
                    _serial.println();
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
                else if (c == 0x8) // backspace
                {
                    if (!_buffer.isEmpty())
                    {
                        _serial.print(c);
                        _serial.print(' ');
                        _serial.print(c);
                        _buffer.remove(_buffer.length() - 1, 1);
                    }
                }
                else if ((c >= 0x20 && c <= 0x7E) || (c >= 0xA0 && c <= 0xFF)) // printable chars
                {
                    _serial.print(c);
                    _buffer += c;
                }
            }

            return false;
        }
    }
}