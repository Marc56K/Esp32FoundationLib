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

        void ParameterSet::Save()
        {
            std::vector<String> garbage;
            for (auto &key : _keys)
            {
                if (_params.find(key.first) == _params.end())
                {
                    garbage.push_back(key.first);
                }
            }
            for (auto &name : garbage)
            {
                Unset(name);
            }
            KeyValueStorage::Save();
        }

        void ParameterSet::Register(Parameter *parameter)
        {
            if (parameter != nullptr)
            {
                _params[parameter->Name] = parameter;
            }
        }

        void ParameterSet::Unregister(Parameter *parameter)
        {
            if (parameter != nullptr)
            {
                _params.erase(parameter->Name);
            }
        }

        Parameter *ParameterSet::GetParameter(const String &name) const
        {
            auto it = _params.find(name);
            if (it != _params.end())
            {
                return it->second;
            }
            return nullptr;
        }

        const std::map<String, Parameter *> &ParameterSet::GetParameters() const
        {
            return _params;
        }

        ParameterSet DefaultParameterSet;

        String StringParameter::ToString()
        {
            return (String)(*this);
        }

        StringParameter::operator String()
        {
            ParamSet.Load();

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
            ParamSet.Load();

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

        String FloatParameter::ToString()
        {
            return String((float)*this, DecimalPlaces);
        }

        FloatParameter::operator float()
        {
            ParamSet.Load();

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
            ParamSet.Load();

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

        String IntegerParameter::ToString()
        {
            return String((int32_t)*this);
        }

        IntegerParameter::operator int32_t()
        {
            ParamSet.Load();

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
            ParamSet.Load();

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

        String BooleanParameter::ToString()
        {
            return (*this) ? "true" : "false";
        }

        BooleanParameter::operator bool()
        {
            ParamSet.Load();

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
            ParamSet.Load();

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