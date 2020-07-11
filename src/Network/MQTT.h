#ifndef DOUGH_MQTT_H
#define DOUGH_MQTT_H

#include <MQTT.h>
#include <MQTTClient.h>
#include "Network/DoughWiFi.h"
#include "Data/Measurement.h"
#include "UI/DoughLogger.h"
#include "config.h"

namespace Dough
{
    /**
     * This class encapsulates the connection to the MQTT broker.
     * MQTT is used to publish measurements and to store configuration data.
     */
    class MQTT;

    typedef void (*MQTTConnectHandler)(MQTT *mqtt);
    typedef void (*MQTTMessageHandler)(String &key, String &value);

    class MQTT
    {
    public:
        static MQTT *Instance();
        void setup();
        void onConnect(MQTTConnectHandler callback);
        void onMessage(MQTTMessageHandler callback);
        bool isConnected();
        bool connect();
        void subscribe(const char *key);
        void procesIncomingsMessages();
        void publish(const char *key, const char *payload);
        void publish(const char *key, int payload);
        void publish(const char *key, Measurement measurement);

    private:
        MQTT();
        static MQTT *_instance;
        MQTTClient _mqttClient;
        DoughLogger _logger;
        MQTTConnectHandler _onConnect = nullptr;
        MQTTClientCallbackSimple _onMessage = nullptr;
        static void handleMessage(String &topic, String &payload);
        char *_mqttDeviceId;
    };

} // namespace Dough

#endif
