#pragma once
#include "KeyValueStorage.h"
#include <set>

namespace esp32
{
    namespace foundation
    {
        struct Parameter;

        class ParameterSet : public KeyValueStorage
        {
        public:
            ParameterSet(
                EEPROMClass &eeprom = EEPROM,
                const uint32_t eepromSize = 512);
            ~ParameterSet();

            void Init();

            void Register(Parameter *parameter);
            void Unregister(Parameter *parameter);

        private:
            std::set<Parameter *> _params;
        };

        extern ParameterSet DefaultParameterSet;

        enum ParameterType
        {
            PT_STRING = 0,
            PT_FLOAT,
            PT_INTEGER,
            PT_BOOLEAN
        };

        struct Parameter
        {
            const ParameterType Type;
            const String Name;

            ParameterSet &ParamSet;

        protected:
            int32_t _keyId;

            Parameter(
                const ParameterType type,
                const String &name,
                ParameterSet &paramSet = DefaultParameterSet)
                : Type(type),
                  Name(name),
                  ParamSet(paramSet),
                  _keyId(-1)
            {
            }
        };

        struct StringParameter : Parameter
        {
            const String DefaultValue;

            StringParameter(
                const String &name,
                const String &defaultValue = "",
                ParameterSet &paramSet = DefaultParameterSet)
                : Parameter(PT_STRING, name, paramSet),
                  DefaultValue(defaultValue)
            {
            }

            operator String();
            StringParameter &operator=(const String &value);
        };

        struct FloatParameter : Parameter
        {
            const float DefaultValue;
            const float MinValue;
            const float MaxValue;

            FloatParameter(
                const String &name,
                const float defaultValue = 0.0f,
                const float minValue = -std::numeric_limits<float>::max(),
                const float maxValue = std::numeric_limits<float>::max(),
                ParameterSet &paramSet = DefaultParameterSet)
                : Parameter(PT_FLOAT, name, paramSet),
                  DefaultValue(defaultValue),
                  MinValue(minValue),
                  MaxValue(maxValue)
            {
            }

            operator float();
            FloatParameter &operator=(const float value);
        };

        struct IntegerParameter : Parameter
        {
            const int32_t DefaultValue;
            const int32_t MinValue;
            const int32_t MaxValue;

            IntegerParameter(
                const String &name,
                const int32_t defaultValue = 0,
                const int32_t minValue = -std::numeric_limits<int32_t>::max(),
                const int32_t maxValue = std::numeric_limits<int32_t>::max(),
                ParameterSet &paramSet = DefaultParameterSet)
                : Parameter(PT_INTEGER, name, paramSet),
                  DefaultValue(defaultValue),
                  MinValue(minValue),
                  MaxValue(maxValue)
            {
            }

            operator int32_t();
            IntegerParameter &operator=(const int32_t value);
        };

        struct BooleanParameter : Parameter
        {
            const bool DefaultValue;

            BooleanParameter(
                const String &name,
                const bool defaultValue = false,
                ParameterSet &paramSet = DefaultParameterSet)
                : Parameter(PT_BOOLEAN, name, paramSet),
                  DefaultValue(defaultValue)
            {
            }

            operator bool();
            BooleanParameter &operator=(const bool value);
        };
    }
}