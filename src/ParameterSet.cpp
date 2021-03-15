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

        void ParameterSet::SaveToEEPROM()
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
            KeyValueStorage::SaveToEEPROM();
        }

        void ParameterSet::Register(Parameter &parameter)
        {
            _params[parameter.Name] = &parameter;
        }

        void ParameterSet::Unregister(Parameter &parameter)
        {
            _params.erase(parameter.Name);
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

        /*************/
        /* Parameter */
        /*************/

        Parameter::Parameter(
            const ParameterType type,
            const String &name,
            ParameterSet &paramSet)
            : Type(type),
                Name(name),
                ParamSet(paramSet),
                _keyId(-1)
        {
            ParamSet.Register(*this);
        }

        Parameter::~Parameter()
        {
            ParamSet.Unregister(*this);
        }

        /*******************/
        /* StringParameter */
        /*******************/

        void StringParameter::SetFromString(const String& value)
        {
            *this = value;
        }

        String StringParameter::ToString()
        {
            return (String)(*this);
        }

        StringParameter::operator String()
        {
            ParamSet.LoadFromEEPROM();

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
            ParamSet.LoadFromEEPROM();

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

        /******************/
        /* FloatParameter */
        /******************/

        void FloatParameter::SetFromString(const String& value)
        {
            *this = value.toFloat();
        }

        String FloatParameter::ToString()
        {
            return String((float)*this, DecimalPlaces);
        }

        FloatParameter::operator float()
        {
            ParamSet.LoadFromEEPROM();

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
            ParamSet.LoadFromEEPROM();

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

        /********************/
        /* IntegerParameter */
        /********************/

        void IntegerParameter::SetFromString(const String& value)
        {
            *this = value.toInt();
        }

        String IntegerParameter::ToString()
        {
            return String((int32_t)*this);
        }

        IntegerParameter::operator int32_t()
        {
            ParamSet.LoadFromEEPROM();

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
            ParamSet.LoadFromEEPROM();

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

        /********************/
        /* BooleanParameter */
        /********************/

        void BooleanParameter::SetFromString(const String& value)
        {
            if (value.isEmpty())
            {
                *this = false;
            }
            else
            {
                switch(value[0])
                {
                    case 'f':
                    case 'F':
                    case '0':
                        *this = false;
                        break;
                    default:
                        *this = true;
                        break;
                }
            }      
        }

        String BooleanParameter::ToString()
        {
            return (*this) ? "true" : "false";
        }

        BooleanParameter::operator bool()
        {
            ParamSet.LoadFromEEPROM();

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
            ParamSet.LoadFromEEPROM();

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