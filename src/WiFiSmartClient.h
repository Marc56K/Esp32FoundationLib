#pragma once
#include <string>
#include <vector>
#include <WiFi.h>

class WiFiSmartClient
{
public:
    WiFiSmartClient(const char* wifiSSID, const char* wifiKey, const char* hostName);
    ~WiFiSmartClient();

    bool Connect(const uint32_t timeoutInMillis);
    void Disconnect();
    bool Connected();

private:
    std::string _wifiSSID;
    std::string _wifiKey;
    std::string _hostName;
    std::vector<WiFiEventId_t> _eventIds;
};