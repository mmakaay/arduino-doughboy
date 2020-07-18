#ifndef DOUGH_APP_H
#define DOUGH_APP_H

#include <Arduino.h>
#include "UI/UI.h"
#include "App/Configuration.h"
#include "App/callbacks.h"
#include "Sensors/SensorController.h"
#include "App/SensorControllerPlugin.h"
#include "Sensors/HighLevel/TemperatureSensor.h"
#include "Sensors/HighLevel/HumiditySensor.h"
#include "Sensors/HighLevel/DistanceSensor.h"
#include "config.h"

namespace Dough
{
    class App
    {
    public:
        static App *Instance();
        Configuration config;
        UI ui;
        WiFi wifi;
        MQTT mqtt;
        SensorControllerPlugin sensorControllerPlugin;
        DistanceSensor distanceSensorX;
        SensorController distanceSensor;
        TemperatureSensor temperatureSensorX;
        SensorController temperatureSensor;
        HumiditySensor humiditySensorX;
        SensorController humiditySensor;

        void setup();
        void measure();
        void clearHistory();
                
    private:
        App();
        Logger _logger;
    };
}

#endif