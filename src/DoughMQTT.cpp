#include "DoughMQTT.h"

// ----------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------

DoughMQTT* DoughMQTT::_instance = nullptr;

/**
 * Fetch the DoughMQTT singleton.
 */
DoughMQTT* DoughMQTT::Instance() {
    if (DoughMQTT::_instance == nullptr) {
        DoughMQTT::_instance = new DoughMQTT();
    }
    return DoughMQTT::_instance;
}

DoughMQTT::DoughMQTT() {
    _ui = DoughUI::Instance();
}

// ----------------------------------------------------------------------
// Setup
// ----------------------------------------------------------------------

void DoughMQTT::setup() {
    DoughNetwork* network = DoughNetwork::Instance();

    #ifdef MQTT_DEVICE_ID
    _mqttDeviceId = MQTT_DEVICE_ID;
    #else
    _mqttDeviceId = network->getMacAddress();
    #endif
    _ui->log("MQTT", "ss", "Device ID = ", _mqttDeviceId);

    _mqttClient.begin(MQTT_BROKER, MQTT_PORT, network->client);    
}

void DoughMQTT::onConnect(DoughMQTTConnectHandler callback) {
    _onConnect = callback;
}

void DoughMQTT::onMessage(MQTTClientCallbackSimple callback) {
    _onMessage = callback;
}

// ----------------------------------------------------------------------
// Loop
// ----------------------------------------------------------------------

bool DoughMQTT::isConnected() {
    return _mqttClient.connected();
}

bool DoughMQTT::connect() {
    _ui->log("MQTT" , "sssi", "Broker = ", MQTT_BROKER, ":", MQTT_PORT);
    _mqttClient.connect(_mqttDeviceId, MQTT_USERNAME, MQTT_PASSWORD);
  
    // Check if the connection to the broker was successful.
    if (!_mqttClient.connected()) {
        _ui->log("MQTT", "s", "ERROR - Connection to broker failed");
        return false;
    }

    _mqttClient.onMessage(DoughMQTT::handleMessage);

    if (_onConnect != nullptr) {
        _onConnect(this);
    }

    return true;
}

void DoughMQTT::procesIncomingsMessages() {
    _mqttClient.loop();
}

void DoughMQTT::handleMessage(String &topic, String &payload) {
    DoughUI::Instance()->log("MQTT", "sSsS", "<<< ",  topic, " = ", payload);    

    DoughMQTT *mqtt = DoughMQTT::Instance();
    if (mqtt->_onMessage != nullptr) {
        int pos = topic.lastIndexOf('/');
        if (pos != -1) {
            topic.remove(0, pos+1);
            mqtt->_onMessage(topic, payload);
        }
    }
}

void DoughMQTT::subscribe(const char* key) {
    char topic[200];
    snprintf(topic, sizeof(topic)/sizeof(topic[0]), "%s/%s/%s", MQTT_TOPIC_PREFIX, _mqttDeviceId, key);
    DoughUI::Instance()->log("MQTT", "ss", "Subscribe to ", topic);
    _mqttClient.subscribe(topic);
}

void DoughMQTT::publish(const char* key, const char* payload) {
    char topic[200];
    snprintf(topic, sizeof(topic)/sizeof(topic[0]), "%s/%s/%s", MQTT_TOPIC_PREFIX, _mqttDeviceId, key);
    DoughUI::Instance()->log("MQTT", "ssss", ">>> ", topic, " = ", payload);
    _mqttClient.publish(topic, payload);
}

void DoughMQTT::publish(const char* key, int payload) {
    char buf[16];
    snprintf(buf, 16, "%d", payload);
    publish(key, buf);
}
