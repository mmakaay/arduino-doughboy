#include "Network/MQTT.h"

namespace Dough
{

    // ----------------------------------------------------------------------
    // Constructor
    // ----------------------------------------------------------------------

    MQTT *MQTT::_instance = nullptr;

    MQTT *MQTT::Instance()
    {
        if (MQTT::_instance == nullptr)
        {
            MQTT::_instance = new MQTT();
        }
        return MQTT::_instance;
    }

    MQTT::MQTT() : _logger("MQTT") {}

    // ----------------------------------------------------------------------
    // Setup
    // ----------------------------------------------------------------------

    void MQTT::setup()
    {
        WiFi *network = WiFi::Instance();

#ifdef MQTT_DEVICE_ID
        _mqttDeviceId = MQTT_DEVICE_ID;
#else
        _mqttDeviceId = network->getMacAddress();
#endif
        _logger.log("ss", "Device ID = ", _mqttDeviceId);

        _mqttClient.begin(MQTT_BROKER, MQTT_PORT, network->client);
    }

    void MQTT::onConnect(MQTTConnectHandler callback)
    {
        _onConnect = callback;
    }

    void MQTT::onMessage(MQTTClientCallbackSimple callback)
    {
        _onMessage = callback;
    }

    // ----------------------------------------------------------------------
    // Loop
    // ----------------------------------------------------------------------

    bool MQTT::isConnected()
    {
        return _mqttClient.connected();
    }

    bool MQTT::connect()
    {
        _logger.log("sssi", "Broker = ", MQTT_BROKER, ":", MQTT_PORT);
        _mqttClient.connect(_mqttDeviceId, MQTT_USERNAME, MQTT_PASSWORD);

        // Check if the connection to the broker was successful.
        if (!_mqttClient.connected())
        {
            _logger.log("s", "ERROR - Connection to broker failed");
            return false;
        }

        _mqttClient.onMessage(MQTT::handleMessage);

        if (_onConnect != nullptr)
        {
            _onConnect(this);
        }

        return true;
    }

    void MQTT::procesIncomingsMessages()
    {
        _mqttClient.loop();
    }

    void MQTT::handleMessage(String &topic, String &payload)
    {
        MQTT::Instance()->_logger.log("sSsS", "<<< ", topic, " = ", payload);

        MQTT *mqtt = MQTT::Instance();
        if (mqtt->_onMessage != nullptr)
        {
            int pos = topic.lastIndexOf('/');
            if (pos != -1)
            {
                topic.remove(0, pos + 1);
                mqtt->_onMessage(topic, payload);
            }
        }
    }

    void MQTT::subscribe(const char *key)
    {
        char topic[200];
        snprintf(topic, sizeof(topic) / sizeof(topic[0]), "%s/%s/%s", MQTT_TOPIC_PREFIX, _mqttDeviceId, key);
        _logger.log("ss", "Subscribe to ", topic);
        _mqttClient.subscribe(topic);
    }

    void MQTT::publish(const char *key, const char *payload)
    {
        char topic[200];
        snprintf(topic, sizeof(topic) / sizeof(topic[0]), "%s/%s/%s", MQTT_TOPIC_PREFIX, _mqttDeviceId, key);
        _logger.log("ssss", ">>> ", topic, " = ", payload);
        _mqttClient.publish(topic, payload);
    }

    void MQTT::publish(const char *key, int payload)
    {
        char buf[16];
        snprintf(buf, 16, "%d", payload);
        publish(key, buf);
    }

    void MQTT::publish(const char *key, Measurement measurement)
    {
        if (measurement.ok)
        {
            publish(key, measurement.value);
        }
        else
        {
            publish(key, "null");
        }
    }

}