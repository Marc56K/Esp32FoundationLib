#include "ParameterSet.h"

namespace esp32
{
    namespace foundation
    {
        ParameterSet::ParameterSet(
            EEPROMClass &eeprom,
            const uint32_t eepromSize)
            : KeyValueStorage(eeprom, eepromSize)
        {
        }

        ParameterSet::~ParameterSet()
        {
        }

        void ParameterSet::Init()
        {
            if (!IsLoaded())
            {
                Load();
            }
        }

        void ParameterSet::Register(Parameter *parameter)
        {
            _params.insert(parameter);
        }

        void ParameterSet::Unregister(Parameter *parameter)
        {
            _params.erase(parameter);
        }

        ParameterSet DefaultParameterSet;

        StringParameter::operator String()
        {
            ParamSet.Init();

            if (_keyId < 0)
                _keyId = ParamSet.GetKeyId(Name);

            if (_keyId < 0)
                return DefaultValue;

            std::vector<uint8_t> val;
            ParamSet.Get(_keyId, val);
            return String((char *)val.data());
        }

        StringParameter &StringParameter::operator=(const String &value)
        {
            ParamSet.Init();

            if (_keyId < 0)
            {
                _keyId = ParamSet.Set(Name, value.c_str(), value.length() + 1);
            }
            else
            {
                ParamSet.Set(_keyId, value.c_str(), value.length() + 1);
            }
            return *this;
        }

        FloatParameter::operator float()
        {
            ParamSet.Init();

            if (_keyId < 0)
                _keyId = ParamSet.GetKeyId(Name);

            if (_keyId < 0)
                return DefaultValue;

            auto result = DefaultValue;
            ParamSet.Get(_keyId, result);

            return result;
        }

        FloatParameter &FloatParameter::operator=(const float value)
        {
            ParamSet.Init();

            const auto val = constrain(value, MinValue, MaxValue);
            if (_keyId < 0)
            {
                _keyId = ParamSet.Set(Name, val);
            }
            else
            {
                ParamSet.Set(_keyId, val);
            }
            return *this;
        }

        IntegerParameter::operator int32_t()
        {
            ParamSet.Init();

            if (_keyId < 0)
                _keyId = ParamSet.GetKeyId(Name);

            if (_keyId < 0)
                return DefaultValue;

            auto result = DefaultValue;
            ParamSet.Get(_keyId, result);

            return result;
        }

        IntegerParameter &IntegerParameter::operator=(const int32_t value)
        {
            ParamSet.Init();

            const auto val = constrain(value, MinValue, MaxValue);
            if (_keyId < 0)
            {
                _keyId = ParamSet.Set(Name, val);
            }
            else
            {
                ParamSet.Set(_keyId, val);
            }
            return *this;
        }

        BooleanParameter::operator bool()
        {
            ParamSet.Init();

            if (_keyId < 0)
                _keyId = ParamSet.GetKeyId(Name);

            if (_keyId < 0)
                return DefaultValue;

            auto result = DefaultValue;
            ParamSet.Get(_keyId, result);

            return result;
        }

        BooleanParameter &BooleanParameter::operator=(const bool value)
        {
            ParamSet.Init();

            if (_keyId < 0)
            {
                _keyId = ParamSet.Set(Name, value);
            }
            else
            {
                ParamSet.Set(_keyId, value);
            }
            return *this;
        }
    }
}