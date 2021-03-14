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

        void ParameterSet::Register(Parameter* parameter)
        {
            _params.insert(parameter);
        }

        void ParameterSet::Unregister(Parameter* parameter)
        {
            _params.erase(parameter);
        }

        ParameterSet DefaultParameterSet;
    }
}