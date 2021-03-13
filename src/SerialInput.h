#pragma once
#include <Arduino.h>

namespace esp32
{
    namespace foundation
    {
        class SerialInput
        {
        public:
            SerialInput(HardwareSerial &serial = Serial);
            ~SerialInput();

            bool ReadCommand(String &cmd, String &arg);

        private:
            HardwareSerial &_serial;
            String _buffer;
        };
    }
}