#include "KeyValueStorage.h"
#include <vector>

#define HASH_SEED 0x2c58ad4d 

namespace esp32
{
    namespace foundation
    {
        KeyValueStorage::KeyValueStorage(
            EEPROMClass &eeprom,
            const uint32_t eepromSize)
            : _eeprom(eeprom),
              _eepromSize(max<uint32_t>(sizeof(StorageHeader), eepromSize)),
              _isLoaded(false),
              _isModified(false),
              _maxKeyId(-1)
        {
        }

        KeyValueStorage::~KeyValueStorage()
        {
        }

        uint32_t KeyValueStorage::Load()
        {
            _maxKeyId = -1;
            _values.clear();
            _keys.clear();

            std::vector<uint8_t> buffer(_eepromSize);
            if (_eeprom.begin(_eepromSize))
            {
                _eeprom.readBytes(0, buffer.data(), buffer.size());
                _eeprom.end();
            }

            uint8_t *buff = buffer.data();
            StorageHeader *header = (StorageHeader *)buff;
            const uint32_t actualHash = ComputeHash(buff);
            if (header->hash == actualHash)
            {
                uint8_t *ptr = buff + sizeof(StorageHeader);
                const uint8_t *endPtr = buff + header->totalSize;
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
            }

            _isModified = false;
            _isLoaded = true;

            return _values.size();
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

        void KeyValueStorage::Save()
        {
            if (_isModified && _eeprom.begin(_eepromSize))
            {
                _isModified = false;

                uint8_t *ptr = _eeprom.getDataPtr();

                StorageHeader *header = (StorageHeader *)ptr;
                header->totalSize = sizeof(StorageHeader);
                ptr += sizeof(StorageHeader);

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
                    if (header->totalSize + sizeof(EntryHeader) + entry.keySize + entry.valueSize > _eepromSize)
                    {
                        break;
                    }
                    
                    _eeprom.writeBytes(header->totalSize, &entry, sizeof(EntryHeader));
                    header->totalSize += sizeof(EntryHeader);
                    _eeprom.writeBytes(header->totalSize, key.first.c_str(), entry.keySize);
                    header->totalSize += entry.keySize;
                    _eeprom.writeBytes(header->totalSize, value.data(), entry.valueSize);
                    header->totalSize += entry.valueSize;
                }

                if (header->totalSize < _eepromSize)
                {
                    memset(_eeprom.getDataPtr() + header->totalSize, 0, _eepromSize - header->totalSize);
                }

                header->hash = ComputeHash(_eeprom.getDataPtr());

                _eeprom.commit();
                _eeprom.end();
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

        const std::map<String, std::vector<uint8_t>> KeyValueStorage::GetEntries() const
        {
            std::map<String, std::vector<uint8_t>> result;
            for (auto key : _keys)
            {
                result[key.first] = _values.at(key.second);
            }
            return result;
        }

        uint32_t KeyValueStorage::ComputeHash(const void *storage) const
        {
            uint32_t result = HASH_SEED;
            const uint32_t *ptr = (uint32_t *)storage;
            for (uint32_t i = sizeof(StorageHeader) / sizeof(uint32_t); i < _eepromSize / sizeof(uint32_t); i++)
            {
                result += ptr[i];
            }
            return result;
        }
    }
}