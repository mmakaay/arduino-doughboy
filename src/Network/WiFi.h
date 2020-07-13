#ifndef DOUGH_WIFI_H
#define DOUGH_WIFI_H

#include <WiFiNINA.h>
#include "UI/Logger.h"
#include "config.h"

namespace Dough
{
    // This class encapsulates the connection to the WiFi network.
    class WiFi
    {
    public:
        WiFi();
        char *getMacAddress();
        void setup();
        void loop();
        bool isConnected();
        bool connect();
        WiFiClient client;

    private:
        void _storeMacAddress();
        char _macAddress[18]; // max MAC address length + 1
        Logger _logger;
    };
}

#endif