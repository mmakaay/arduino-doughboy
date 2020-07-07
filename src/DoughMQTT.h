#ifndef DOUGH_MQTT_H
#define DOUGH_MQTT_H

#include <MQTT.h>
#include <MQTTClient.h>
#include "DoughNetwork.h"
#include "DoughUI.h"
#include "config.h"

class DoughMQTT;

typedef void (*DoughMQTTConnectHandler)(DoughMQTT* mqtt);
typedef void (*DoughMQTTMessageHandler)(String &key, String &value);

class DoughMQTT {
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

    private:
        DoughMQTT();
        static DoughMQTT* _instance;
        MQTTClient _mqttClient;
        DoughUI* _ui;
        DoughMQTTConnectHandler _onConnect = nullptr;
        MQTTClientCallbackSimple _onMessage = nullptr;
        static void handleMessage(String &topic, String &payload);
        char *_mqttDeviceId;
};

#endif
