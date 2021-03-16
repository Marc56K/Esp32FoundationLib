#pragma once
#include <Arduino.h>
#include "ParameterSet.h"

namespace esp32
{
    namespace foundation
    {
        class HtmlTemplates
        {
        public:
            static String HtmlSpecialChars(const String &str);

            static String TextInputField(
                const String &id, 
                const String &label, 
                const String &value, 
                const bool password);

            static String CheckboxInputField(
                const String &id, 
                const String &label, 
                const bool &checked);

            static String NumberInputField(
                const String &id, 
                const String &label, 
                const String &value, 
                const String &minValue, 
                const String &maxValue);

            static String ParameterInputField(
                Parameter& parameter,
                const String &label);
        };
    }
}