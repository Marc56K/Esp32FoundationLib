#pragma once
#include "KeyValueStorage.h"
#include <map>

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
            virtual ~ParameterSet();

            virtual void SaveToEEPROM() override;

            void Register(Parameter &parameter);
            void Unregister(Parameter &parameter);

            Parameter *GetParameter(const String &name) const;
            const std::map<String, Parameter *> &GetParameters() const;

            void PrintParameters(HardwareSerial& serial, const bool hiddenParams = false) const;

        private:
            std::map<String, Parameter *> _params;
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

            virtual ~Parameter();
            virtual void SetFromString(const String& value) = 0;
            virtual String ToString() = 0;

            bool IsHidden() const;
            String GetDisplayName() const;

        protected:
            int32_t _keyId;

            Parameter(
                const ParameterType type,
                const String &name,
                ParameterSet &paramSet = DefaultParameterSet);
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

            virtual void SetFromString(const String& value) override;
            virtual String ToString() override;

            operator String();
            StringParameter &operator=(const String &value);
        };

        struct FloatParameter : Parameter
        {
            const float DefaultValue;
            const uint8_t DecimalPlaces;
            const float MinValue;
            const float MaxValue;

            FloatParameter(
                const String &name,
                const float defaultValue = 0.0f,
                const uint8_t decimalPlaces = 2,
                const float minValue = -std::numeric_limits<float>::max(),
                const float maxValue = std::numeric_limits<float>::max(),
                ParameterSet &paramSet = DefaultParameterSet)
                : Parameter(PT_FLOAT, name, paramSet),
                  DefaultValue(defaultValue),
                  DecimalPlaces(decimalPlaces),
                  MinValue(minValue),
                  MaxValue(maxValue)
            {
            }

            virtual void SetFromString(const String& value) override;
            virtual String ToString() override;

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

            virtual void SetFromString(const String& value) override;
            virtual String ToString() override;

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

            virtual void SetFromString(const String& value) override;
            virtual String ToString() override;

            operator bool();
            BooleanParameter &operator=(const bool value);
        };
    }
}