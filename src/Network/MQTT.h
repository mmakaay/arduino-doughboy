#ifndef DOUGH_MQTT_H
#define DOUGH_MQTT_H

#include <MQTT.h>
#include <MQTTClient.h>
#include "Network/WiFi.h"
#include "Data/Measurement.h"
#include "UI/Logger.h"
#include "config.h"

namespace Dough
{
    // This class encapsulates the connection to the MQTT broker.
    // MQTT is used to publish measurements and to store configuration data.
    class MQTT;

    typedef void (*MQTTConnectHandler)(MQTT *mqtt);
    typedef void (*MQTTMessageHandler)(String &key, String &value);

    class MQTT
    {
    public:
        MQTT(WiFi *wifi, MQTTConnectHandler onConnect, MQTTMessageHandler onMessage);
        void setup();
        bool isConnected();
        bool connect();
        void subscribe(const char *key);
        void procesIncomingsMessages();
        void publish(const char *key, const char *payload);
        void publish(const char *key, int payload);
        void publish(const char *key, Measurement measurement);

    private:
        WiFi *_wifi;
        Logger _logger;
        MQTTClient _mqttClient;
        MQTTConnectHandler _onConnect;
        MQTTClientCallbackSimple _onMessage;
        char *_mqttDeviceId;
    };
}

#endif
