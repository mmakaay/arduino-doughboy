#ifndef DOUGH_APP_H
#define DOUGH_APP_H

#include <Arduino.h>
#include "App/Configuration.h"
#include "Data/SensorController.h"
#include "Sensors/TemperatureSensor.h"
#include "Sensors/HumiditySensor.h"
#include "Sensors/DistanceSensor.h"
#include "config.h"

namespace Dough
{
    class App
    {
    public:
        static App *Instance();
        Configuration config;
        WiFi wifi;
        MQTT mqtt;
        SensorController temperatureSensor;
        SensorController humiditySensor;
        SensorController distanceSensor;

        void setup();
        void measure();
        void clearHistory();
                
    private:
        App();
        Logger _logger;
    };
}

// Callback functions that need to live in the global namespace.
void mqttOnConnectCallback(Dough::MQTT* mqtt);
void mqttOnMessageCallback(String &topic, String &payload);

#endif