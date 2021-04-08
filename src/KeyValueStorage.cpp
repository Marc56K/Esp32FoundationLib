#include "KeyValueStorage.h"
#include <vector>

namespace esp32
{
    namespace foundation
    {
        KeyValueStorage::KeyValueStorage(const String& name) : 
            _name(name),
            _isLoaded(false),
            _isModified(false),
            _maxKeyId(-1)
        {
        }

        KeyValueStorage::~KeyValueStorage()
        {
        }

        bool KeyValueStorage::Load()
        {
            if (!IsLoaded())
            {
                Reload();
                return true;
            }
            return false;
        }

        uint32_t KeyValueStorage::Reload()
        {
            _maxKeyId = -1;
            _values.clear();
            _keys.clear();

            std::vector<uint8_t> buffer;            
            if (_pref.begin(_name.c_str(), true))
            {
                const uint32_t size = _pref.getUInt("size", 0);
                buffer.resize(size);
                if (size > 0)
                {
                    _pref.getBytes("data", buffer.data(), buffer.size());
                }
                _pref.end();
            }

            uint8_t *buff = buffer.data();
            uint8_t *ptr = buff;
            const uint8_t *endPtr = buff + buffer.size();
            while (ptr < endPtr)
            {
                const EntryHeader* entry = (EntryHeader*)ptr;
                ptr += sizeof(EntryHeader);

                if (ptr > endPtr) break;

                // check null terminatin of key
                if (entry->keySize == 0 || ptr[entry->keySize - 1] != 0) break;

                const String key = (char *)ptr;
                ptr += entry->keySize;

                if (ptr > endPtr) break;

                const uint8_t *value = (uint8_t *)ptr;
                ptr += entry->valueSize;

                if (ptr > endPtr) break;

                if (entry->keySize > 0 && entry->valueSize > 0)
                {
                    Set(key, value, entry->valueSize);
                }
            }

            _isModified = false;
            _isLoaded = true;

            return _values.size();
        }

        void KeyValueStorage::Save()
        {
            if (_isModified && _pref.begin(_name.c_str(), false))
            {
                _isModified = false;

                std::vector<uint8_t> buffer;
                buffer.reserve(512);
                for (auto &key : _keys)
                {
                    const auto &value = _values.at(key.second);
                    if (value.empty())
                    {
                        continue;
                    }

                    EntryHeader entry;
                    entry.keySize = key.first.length() + 1;
                    entry.valueSize = value.size();

                    uint32_t idx = buffer.size();
                    buffer.resize(buffer.size() + sizeof(EntryHeader) + entry.keySize + entry.valueSize);
                    memcpy(&buffer[idx], &entry, sizeof(EntryHeader));
                    idx += sizeof(EntryHeader);
                    memcpy(&buffer[idx], key.first.c_str(), entry.keySize);
                    idx += entry.keySize;
                    memcpy(&buffer[idx], value.data(), entry.valueSize);
                    idx += entry.valueSize;
                }

                _pref.putUInt("size", (uint32_t)buffer.size());
                _pref.putBytes("data", buffer.data(), buffer.size());

                _pref.end();
            }
        }

        void KeyValueStorage::Clear()
        {
            if (!_keys.empty())
            {
                _isModified = true;
                _keys.clear();
                _values.clear();
            }
        }

        bool KeyValueStorage::IsModified()
        {
            return _isModified;
        }

        bool KeyValueStorage::IsLoaded()
        {
            return _isLoaded;
        }

        int32_t KeyValueStorage::GetKeyId(const String &key) const
        {
            auto it = _keys.find(key);
            if (it != _keys.end())
            {
                return it->second;
            }
            return -1;
        }

        bool KeyValueStorage::IsSet(const String &key) const
        {
            return _keys.find(key) != _keys.end();
        }

        bool KeyValueStorage::IsSet(const int32_t keyId) const
        {
            return _values.find(keyId) != _values.end();
        }

        void KeyValueStorage::Unset(const String &key)
        {
            const int32_t keyId = GetKeyId(key);
            if (keyId > -1)
            {
                _keys.erase(key);
                _values.erase(keyId);
                _isModified = true;
            }
        }

        void KeyValueStorage::Unset(const int32_t keyId)
        {
            auto it = _values.find(keyId);
            if (it != _values.end())
            {
                _isModified = true;
                _values.erase(it);
                for (auto keyIt = _keys.begin(); keyIt != _keys.end(); ++keyIt)
                {
                    if (keyIt->second == keyId)
                    {
                        _keys.erase(keyIt);
                        return;
                    }
                }
            }
        }

        int32_t KeyValueStorage::Set(const String &key, const void *value, const uint32_t valueSize)
        {
            auto it = _keys.find(key);
            if (it != _keys.end())
            {
                const int32_t keyId = it->second;
                Set(keyId, value, valueSize);
                return keyId;
            }
            else
            {
                std::vector<uint8_t> buff(valueSize);
                memcpy(buff.data(), value, valueSize);

                _maxKeyId++;
                _keys[key] = _maxKeyId;
                _values[_maxKeyId] = buff;
                _isModified = true;
                return _maxKeyId;
            }
        }

        bool KeyValueStorage::Set(const int32_t keyId, const void *value, const uint32_t valueSize)
        {
            auto it = _values.find(keyId);
            if (it != _values.end())
            {
                auto &buff = it->second;
                if (buff.size() != valueSize || memcmp(value, buff.data(), valueSize) != 0)
                {
                    buff.resize(valueSize);
                    memcpy(buff.data(), value, valueSize);
                    _isModified = true;
                }
                return true;
            }
            return false;
        }

        bool KeyValueStorage::Get(const String &key, std::vector<uint8_t> &result) const
        {
            return Get(GetKeyId(key), result);
        }

        bool KeyValueStorage::Get(const int32_t keyId, std::vector<uint8_t> &result) const
        {
            auto it = _values.find(keyId);
            if (it != _values.end())
            {
                result = it->second;
                return true;
            }
            return false;
        }

        const std::map<String, int32_t>& KeyValueStorage::GetKeys() const
        {
            return _keys;
        }

        const std::map<int32_t, std::vector<uint8_t>>& KeyValueStorage::GetValues() const
        {
            return _values;
        }
    }
}