#include "Network/MQTT.h"

namespace Dough
{
    MQTT::MQTT(
        WiFi *wifi,
        MQTTConnectHandler onConnect,
        MQTTMessageHandler onMessage) : _logger("MQTT")
    {
        _wifi = wifi;
        _onConnect = onConnect;
        _onMessage = onMessage;
    }

    void MQTT::setup()
    {
#ifdef MQTT_DEVICE_ID
        _mqttDeviceId = MQTT_DEVICE_ID;
#else
        _mqttDeviceId = _wifi->getMacAddress();
#endif
        _logger.log("ss", "Device ID = ", _mqttDeviceId);

        _mqttClient.begin(MQTT_BROKER, MQTT_PORT, _wifi->client);
    }

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
        _logger.log("s", "Connection to broker successful");

        _mqttClient.onMessage(_onMessage);

        if (_onConnect != nullptr)
        {
            _onConnect(this);
        }

        return true;
    }

    void MQTT::procesIncomingsMessages()
    {
        // Calling loop() on the wrapped MQTT client, will fetch the
        // incoming messages and distribute them to the onMessage callback.
        _mqttClient.loop();
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
        _logger.log("ssss", "Send message: ", topic, " = ", payload);
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