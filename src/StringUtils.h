#pragma once
#include <Arduino.h>

namespace esp32
{
    namespace foundation
    {
        class StringUtils
        {
        public:
            static bool SplitKeyValue(
                const String &input,
                const String &delimiter,
                String &key,
                String &value);
        };
    }
}