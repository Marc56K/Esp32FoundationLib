#include "ParameterSet.h"

namespace esp32
{
    namespace foundation
    {
        ParameterSet::ParameterSet(const String& name)
            : KeyValueStorage(name)
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

        void ParameterSet::PrintParameters(HardwareSerial& serial, const bool ignoreHiddenParams) const
        {
            for (auto& param : _params)
            {
                if (!ignoreHiddenParams || !param.second->IsHidden())
                {
                    serial.print(param.first);
                    serial.print(" = ");
                    serial.println(param.second->ToString());
                }
            }
        }

        ParameterSet DefaultParameterSet("params");

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

        bool Parameter::IsHidden() const
        {
            return Name.startsWith(".");
        }

        String Parameter::GetDisplayName() const
        {
            String result;
            for (uint32_t i = 0; i < Name.length(); i++)
            {
                if (i == 0 || Name[i - 1] == '_')
                {
                    result += (char)std::toupper(Name[i]);
                }
                else if (Name[i] == '_')
                {
                    result += " ";
                }
                else
                {
                    if (std::islower(Name[i - 1]) && std::isupper(Name[i]))
                    {
                        result += " ";
                    }
                    result += Name[i];
                }
            }

            return result;
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