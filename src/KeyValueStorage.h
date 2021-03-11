#pragma once
#include <Arduino.h>
#include <EEPROM.h>
#include <map>

class KeyValueStorage
{
private:
    struct StorageHeader
    {
        uint32_t hash;
        uint32_t numEntries;
    };

public:
    KeyValueStorage(
        EEPROMClass& eeprom = EEPROM,
        const uint32_t eepromSize = 512);

    ~KeyValueStorage();

    uint32_t Load();
    void Save();
    bool IsModified();

    void Set(const String& key, const String& value);
    bool IsSet(const String& key) const;
    String Get(const String& key) const;    
    const std::map<String, String>& GetEntries() const;
    void PrintEntries(HardwareSerial& serial = Serial) const;

private:
    uint32_t ComputeHash(const void* storage) const;

private:
    EEPROMClass& _eeprom;
    uint32_t _eepromSize;
    bool _isModified;
    std::map<String, String> _entries;
};