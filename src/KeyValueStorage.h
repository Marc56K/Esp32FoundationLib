#pragma once
#include <Arduino.h>
#include <EEPROM.h>
#include <map>

namespace esp32
{
    namespace foundation
    {
        class KeyValueStorage
        {
        private:
            struct StorageHeader
            {           
                uint32_t size;
                uint32_t hash;
            };

        public:
            KeyValueStorage(
                EEPROMClass &eeprom = EEPROM,
                const uint32_t eepromSize = 512);

            ~KeyValueStorage();

            uint32_t Load();
            void Save();
            bool IsModified();

            int32_t GetKeyId(const String &key) const;

            void Set(const String &key, const String &value);
            bool Set(const int32_t keyId, const String &value);

            bool IsSet(const String &key) const;
            bool IsSet(const int32_t keyId) const;

            String Get(const String &key) const;
            String Get(const int32_t keyId) const;

            const std::map<String, String> GetEntries() const;
            void PrintEntries(HardwareSerial &serial = Serial) const;

        private:
            uint32_t ComputeHash(const void *storage) const;

        private:
            EEPROMClass &_eeprom;
            uint32_t _eepromSize;
            bool _isModified;
            int32_t _maxKeyId;
            std::map<String, int32_t> _keys;
            std::map<int32_t, String> _values;
        };
    }
}