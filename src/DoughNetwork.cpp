#include "DoughNetwork.h"

// ----------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------

DoughNetwork* DoughNetwork::_instance = nullptr;

/**
 * Fetch the DoughNetwork singleton.
 */
DoughNetwork* DoughNetwork::Instance() {
    if (DoughNetwork::_instance == nullptr) {
        DoughNetwork::_instance = new DoughNetwork();
    }
    return DoughNetwork::_instance;
}

DoughNetwork::DoughNetwork() {
    _ui = DoughUI::Instance();
}

// ----------------------------------------------------------------------
// Setup
// ----------------------------------------------------------------------

void DoughNetwork::_setMacAddress() {
    byte mac[6];
    WiFi.macAddress(mac);
    snprintf(
        _macAddress, sizeof(_macAddress)/sizeof(_macAddress[0]),
        "%x:%x:%x:%x:%x:%x", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
}

void DoughNetwork::setup() {
    _setMacAddress();
    DoughUI::Instance()->log("NETWORK", "ss", "MAC address = ", getMacAddress());  
}

// ----------------------------------------------------------------------
// Loop
// ----------------------------------------------------------------------

bool DoughNetwork::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

bool DoughNetwork::connect() {
    int status = WiFi.status();

    // Check if a device with a WiFi shield is used.
    if (status == WL_NO_SHIELD) {
        _ui->log("NETWORK", "s", "ERROR - Device has no WiFi shield");
        delay(5000);
        return false;
    }
  
    // Check if the WiFi network is already up.
    if (status == WL_CONNECTED) {
        return true;
    }

    // Setup the connection to the WiFi network.
    _ui->log("NETWORK", "ss", "WiFi network = ", WIFI_SSID);
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
    // Check if the connection attempt was successful.
    if (status == WL_CONNECTED) {
        _ui->log("NETWORK", "sa", "IP-Address = ", WiFi.localIP());
        _ui->log("NETWORK", "sis", "Signal strength = ", WiFi.RSSI(), " dBm");        
        return true;
    } else {
        _ui->log("NETWORK", "sis", "ERROR - WiFi connection failed (reason: ", WiFi.reasonCode(), ")");
        return false;
    }
}

char* DoughNetwork::getMacAddress() {
    return _macAddress;
}
