#include "Network/WiFi.h"

namespace Dough
{
    WiFi::WiFi() : _logger("WIFI") {}

    // ----------------------------------------------------------------------
    // Setup
    // ----------------------------------------------------------------------

    void WiFi::_storeMacAddress()
    {
        byte mac[6];
        ::WiFi.macAddress(mac);
        snprintf(
            _macAddress, sizeof(_macAddress) / sizeof(_macAddress[0]),
            "%x:%x:%x:%x:%x:%x", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
    }

    void WiFi::setup()
    {
        _storeMacAddress();
        _logger.log("ss", "MAC address = ", getMacAddress());
    }

    char *WiFi::getMacAddress()
    {
        return _macAddress;
    }

    bool WiFi::isConnected()
    {
        return ::WiFi.status() == WL_CONNECTED;
    }

    bool WiFi::connect()
    {
        int status = ::WiFi.status();

        // Check if a device with a WiFi shield is used.
        if (status == WL_NO_SHIELD)
        {
            _logger.log("s", "ERROR - Device has no WiFi shield");
            delay(5000);
            return false;
        }

        // Check if the WiFi network is already up.
        if (status == WL_CONNECTED)
        {
            return true;
        }

        // Setup the connection to the WiFi network.
        _logger.log("ss", "WiFi network = ", WIFI_SSID);
        status = ::WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

        // Check if the connection attempt was successful.
        if (status == WL_CONNECTED)
        {
            char buf[50]; // this will also fit IPv6 addresses
            auto printer = StringPrinter(buf, 50);
            printer.print(::WiFi.localIP());
            _logger.log("ss", "IP-Address = ", buf);
            _logger.log("sis", "Signal strength = ", ::WiFi.RSSI(), " dBm");
            return true;
        }
        else
        {
            _logger.log("sis", "ERROR - WiFi connection failed (reason: ", ::WiFi.reasonCode(), ")");
            return false;
        }
    }
}