#include "KeyValueStorage.h"
#include <vector>

#define HASH_SEED 100

KeyValueStorage::KeyValueStorage(
    EEPROMClass &eeprom,
    const uint32_t eepromSize)
    : _eeprom(eeprom),
      _eepromSize(max<uint32_t>(sizeof(StorageHeader), eepromSize)),
      _isModified(false)
{
}

KeyValueStorage::~KeyValueStorage()
{
}

uint32_t KeyValueStorage::Load()
{
    _isModified = false;
    _entries.clear();

    std::vector<uint8_t> buffer(_eepromSize);
    if (_eeprom.begin(_eepromSize))
    {
        _eeprom.readBytes(0, buffer.data(), buffer.size());
        _eeprom.end();
    }
    uint8_t *ptr = buffer.data();
    StorageHeader* header = (StorageHeader*)ptr;
    const uint32_t actualHash = ComputeHash( buffer.data());
    if (header->hash == actualHash)
    {
        ptr += sizeof(StorageHeader);
        for (uint32_t i = 0; i < header->numEntries; i++)
        {
            const String key = (char*)ptr;
            ptr += key.length() + 1;

            const String value = (char*)ptr;
            ptr += value.length() + 1;

            if (key.length() > 0 && value.length() > 0)
            {
                _entries[key] = value;
            }
        }
    }

    return _entries.size();
}

void KeyValueStorage::Save()
{
    if (_isModified && _eeprom.begin(_eepromSize))
    {
        _isModified = false;

        StorageHeader* header = (StorageHeader*)_eeprom.getDataPtr();
        header->numEntries = (uint32_t)_entries.size();
        uint32_t pos = sizeof(StorageHeader);
        for (auto& entry : _entries)
        {
            _eeprom.writeString(pos, entry.first);
            pos += entry.first.length() + 1;
            _eeprom.writeString(pos, entry.second);
            pos += entry.second.length() + 1;
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

void KeyValueStorage::Set(const String &key, const String &value)
{
    auto it = _entries.find(key);
    if (it != _entries.end())
    {
        if (value.isEmpty())
        {
            _entries.erase(it);
            _isModified = true;
        }
        else if (it->second != value)
        {
            _entries[key] = value;
            _isModified = true;
        }
    }
    else if (!value.isEmpty())
    {
        _entries[key] = value;
        _isModified = true;
    }
}

bool KeyValueStorage::IsSet(const String &key) const
{
    return _entries.find(key) != _entries.end();
}

String KeyValueStorage::Get(const String &key) const
{
    auto it = _entries.find(key);
    if (it != _entries.end())
    {
        return it->second;
    }
    return "";
}

const std::map<String, String> &KeyValueStorage::GetEntries() const
{
    return _entries;
}

void KeyValueStorage::PrintEntries(HardwareSerial& serial) const
{
    for (auto entry : _entries)
    {
        serial.println(entry.first + "=" + entry.second);
    }
}

uint32_t KeyValueStorage::ComputeHash(const void* storage) const
{
    uint32_t result = HASH_SEED;
    const uint32_t* ptr = (uint32_t*)storage;
    for (uint32_t i = 1; i < _eepromSize / sizeof(uint32_t); i++)
    {
        result += ptr[i];
    }
    return result;
}