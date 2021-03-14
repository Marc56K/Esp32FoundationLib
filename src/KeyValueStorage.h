#pragma once
#include <Arduino.h>
#include <EEPROM.h>
#include <map>
#include <vector>

namespace esp32
{
    namespace foundation
    {
        class KeyValueStorage
        {
        private:
            struct StorageHeader
            {           
                uint32_t totalSize;
                uint32_t hash;
            };

            struct EntryHeader
            {
                uint32_t keySize;
                uint32_t valueSize;
            };

        public:
            KeyValueStorage(
                EEPROMClass &eeprom = EEPROM,
                const uint32_t eepromSize = 512);

            ~KeyValueStorage();

            uint32_t Load();
            void Clear();
            void Save();
            bool IsModified();

            int32_t GetKeyId(const String &key) const;

            void Set(const String &key, const void* value, const uint32_t valueSize);
            bool Set(const int32_t keyId, const void* value, const uint32_t valueSize);

            bool IsSet(const String &key) const;
            bool IsSet(const int32_t keyId) const;

            bool Get(const String &key, std::vector<uint8_t>& result) const;
            bool Get(const int32_t keyId, std::vector<uint8_t>& result) const;

            const std::map<String, std::vector<uint8_t>> GetEntries() const;

        private:
            uint32_t ComputeHash(const void *storage) const;

        private:
            EEPROMClass &_eeprom;
            uint32_t _eepromSize;
            bool _isModified;
            int32_t _maxKeyId;
            std::map<String, int32_t> _keys;
            std::map<int32_t, std::vector<uint8_t>> _values;
        };
    }
}