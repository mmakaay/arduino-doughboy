#ifndef DOUGH_NETWORK_H
#define DOUGH_NETWORK_H

#include <WiFiNINA.h>
#include "UI/DoughUI.h"
#include "config.h"

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
    DoughUI *_ui;
};

#endif
