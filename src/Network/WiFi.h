#ifndef DOUGH_NETWORK_H
#define DOUGH_NETWORK_H

#include <WiFiNINA.h>
#include "UI/Logger.h"
#include "config.h"

namespace Dough
{
    // This class encapsulates the connection to the WiFi network.
    class WiFi
    {
    public:
        static WiFi *Instance();
        char *getMacAddress();
        void setup();
        void loop();
        bool isConnected();
        bool connect();
        WiFiClient client;

    private:
        WiFi();
        static WiFi *_instance;
        void _setMacAddress();
        char _macAddress[18]; // max MAC address length + 1
        Logger _logger;
    };
}

#endif