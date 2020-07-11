#ifndef DOUGH_MQTT_H
#define DOUGH_MQTT_H

#include <MQTT.h>
#include <MQTTClient.h>
#include "Network/DoughWiFi.h"
#include "Data/Measurement.h"
#include "UI/DoughLogger.h"
#include "config.h"

/**
 * This class encapsulates the connection to the MQTT broker.
 * MQTT is used to publish measurements and to store configuration data.
 */
class DoughMQTT;

typedef void (*DoughMQTTConnectHandler)(DoughMQTT* mqtt);
typedef void (*DoughMQTTMessageHandler)(String &key, String &value);

class DoughMQTT
{
public:
    static DoughMQTT* Instance();
    void setup();
    void onConnect(DoughMQTTConnectHandler callback);
    void onMessage(DoughMQTTMessageHandler callback);
    bool isConnected();
    bool connect();
    void subscribe(const char* key);
    void procesIncomingsMessages();
    void publish(const char* key, const char* payload);
    void publish(const char* key, int payload);
    void publish(const char* key, Measurement measurement);

private:
    DoughMQTT();
    static DoughMQTT* _instance;
    MQTTClient _mqttClient;
    DoughLogger _logger;
    DoughMQTTConnectHandler _onConnect = nullptr;
    MQTTClientCallbackSimple _onMessage = nullptr;
    static void handleMessage(String &topic, String &payload);
    char* _mqttDeviceId;
};

#endif
