#include "KeyValueStorage.h"
#include <vector>

#define HASH_SEED 100

KeyValueStorage::KeyValueStorage(
    EEPROMClass &eeprom,
    const uint32_t eepromSize)
    : _eeprom(eeprom),
      _eepromSize(max<uint32_t>(sizeof(StorageHeader), eepromSize)),
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
    uint8_t *ptr = buffer.data();
    StorageHeader *header = (StorageHeader *)ptr;
    const uint32_t actualHash = ComputeHash(buffer.data());
    if (header->hash == actualHash)
    {
        ptr += sizeof(StorageHeader);
        for (uint32_t i = 0; i < header->numEntries; i++)
        {
            const String key = (char *)ptr;
            ptr += key.length() + 1;

            const String value = (char *)ptr;
            ptr += value.length() + 1;

            if (key.length() > 0 && value.length() > 0)
            {
                Set(key, value);
            }
        }
    }

    _isModified = false;

    return _values.size();
}

void KeyValueStorage::Save()
{
    if (_isModified && _eeprom.begin(_eepromSize))
    {
        _isModified = false;

        StorageHeader *header = (StorageHeader *)_eeprom.getDataPtr();
        header->numEntries = (uint32_t)_values.size();
        uint32_t pos = sizeof(StorageHeader);
        for (auto &key : _keys)
        {
            _eeprom.writeString(pos, key.first);
            pos += key.first.length() + 1;
            const String &value = _values.at(key.second);
            _eeprom.writeString(pos, value);
            pos += value.length() + 1;
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

int32_t KeyValueStorage::GetKeyId(const String &key) const
{
    auto it = _keys.find(key);
    if (it != _keys.end())
    {
        return it->second;
    }
    return -1;
}

void KeyValueStorage::Set(const String &key, const String &value)
{
    auto it = _keys.find(key);
    if (it != _keys.end())
    {
        const int32_t keyId = it->second;
        const String &oldValue = _values[keyId];
        if (value != oldValue)
        {
            _values[keyId] = value;
            _isModified = true;
        }
    }
    else
    {
        _maxKeyId++;
        _keys[key] = _maxKeyId;
        _values[_maxKeyId] = value;
        _isModified = true;
    }
}

bool KeyValueStorage::Set(const int32_t keyId, const String &value)
{
    auto it = _values.find(keyId);
    if (it != _values.end())
    {
        if (it->second != value)
        {
            _values[keyId] = value;
            _isModified = true;
        }
        return true;
    }
    return false;
}

bool KeyValueStorage::IsSet(const String &key) const
{
    return _keys.find(key) != _keys.end();
}

bool KeyValueStorage::IsSet(const int32_t keyId) const
{
    return _values.find(keyId) != _values.end();
}

String KeyValueStorage::Get(const String &key) const
{
    return Get(GetKeyId(key));
}

String KeyValueStorage::Get(const int32_t keyId) const
{
    auto it = _values.find(keyId);
    if (it != _values.end())
    {
        return it->second;
    }
    return "";
}

const std::map<String, String> KeyValueStorage::GetEntries() const
{
    std::map<String, String> result;
    for (auto key : _keys)
    {
        result[key.first] = _values.at(key.second);
    }
    return result;
}

void KeyValueStorage::PrintEntries(HardwareSerial &serial) const
{
    for (auto key : _keys)
    {
        serial.println(key.first + "=" + _values.at(key.second));
    }
}

uint32_t KeyValueStorage::ComputeHash(const void *storage) const
{
    uint32_t result = HASH_SEED;
    const uint32_t *ptr = (uint32_t *)storage;
    for (uint32_t i = 1; i < _eepromSize / sizeof(uint32_t); i++)
    {
        result += ptr[i];
    }
    return result;
}