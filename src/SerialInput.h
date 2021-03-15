#pragma once
#include <Arduino.h>
#include <map>
#include <tuple>
#include <functional>

namespace esp32
{
    namespace foundation
    {
        typedef std::tuple<std::function<bool(const String&)>, String> SerialInputHandler;

        class SerialInput
        {
        public:
            SerialInput(HardwareSerial &serial = Serial);
            ~SerialInput();

            void PrintCommands();

            void On(
                const String& cmd, 
                std::function<bool()> handler, 
                const String& description = "");
            void On(
                const String& cmd, 
                std::function<bool(const String& arg)> handler, 
                const String& description = "");
            void On(
                const String& cmd, 
                std::function<bool(const String& arg0, const String& arg1)> handler, 
                const String& description = "");

            void Update();

            bool ReadCommand(String &cmd, String &arg);            

        private:
            HardwareSerial &_serial;
            String _buffer;
            std::map<String, SerialInputHandler> _handlers;
        };
    }
}