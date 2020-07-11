#ifndef DOUGH_NETWORK_H
#define DOUGH_NETWORK_H

#include <WiFiNINA.h>
#include "UI/DoughLogger.h"
#include "config.h"

/**
 * This class encapsulates the connection to the WiFi network.
 */
class DoughWiFi
{
public:
    static DoughWiFi *Instance();
    char *getMacAddress();
    void setup();
    void loop();
    bool isConnected();
    bool connect();
    WiFiClient client;

private:
    DoughWiFi();
    static DoughWiFi *_instance;
    void _setMacAddress();
    char _macAddress[18]; // max MAC address length + 1
    DoughLogger _logger;
};

#endif
