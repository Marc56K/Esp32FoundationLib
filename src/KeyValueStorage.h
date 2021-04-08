#pragma once
#include <Arduino.h>
#include <EEPROM.h>
#include <map>
#include <vector>
#include <Preferences.h>

namespace esp32
{
    namespace foundation
    {
        class KeyValueStorage
        {
        protected:
            struct EntryHeader
            {
                uint32_t keySize;
                uint32_t valueSize;
            };

        public:
            KeyValueStorage(const String& name);

            virtual ~KeyValueStorage();
            
            virtual bool Load();
            virtual uint32_t Reload();
            virtual void Save();
            virtual void Clear();
            bool IsModified();
            bool IsLoaded();

            int32_t GetKeyId(const String &key) const;

            bool IsSet(const String &key) const;
            bool IsSet(const int32_t keyId) const;

            void Unset(const String &key);
            void Unset(const int32_t keyId);

            int32_t Set(const String &key, const void* value, const uint32_t valueSize);
            bool Set(const int32_t keyId, const void* value, const uint32_t valueSize);

            bool Get(const String &key, std::vector<uint8_t>& result) const;
            bool Get(const int32_t keyId, std::vector<uint8_t>& result) const;

            template <typename T>
            int32_t Set(const String &key, const T& value)
            {
                return Set(key, &value, sizeof(T));
            };

            template <typename T>
            int32_t Set(const int32_t keyId, const T& value)
            {
                return Set(keyId, &value, sizeof(T));
            };

            template <typename T>
            bool Get(const String &key, T& result) const
            {
                return Get<T>(GetKeyId(key), result);
            }

            template <typename T>
            bool Get(const int32_t keyId, T& result) const
            {
                auto it = _values.find(keyId);
                if (it != _values.end() && it->second.size() == sizeof(T))
                {
                    memcpy(&result, it->second.data(), sizeof(T));
                    return true;
                }
                return false;
            }

            const std::map<String, int32_t>& GetKeys() const;
            const std::map<int32_t, std::vector<uint8_t>>& GetValues() const;

        protected:
            uint32_t ComputeHash(const void *storage) const;

        protected:
            String _name;
            Preferences _pref;
            bool _isLoaded;
            bool _isModified;
            int32_t _maxKeyId;
            std::map<String, int32_t> _keys;
            std::map<int32_t, std::vector<uint8_t>> _values;
        };
    }
}