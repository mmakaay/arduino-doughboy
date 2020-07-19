#ifndef DOUGH_APP_H
#define DOUGH_APP_H

#include <Arduino.h>
#include "App/AppStateController.h"
#include "App/AppStateControllerPlugin.h"
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
        AppStateControllerPlugin statePlugin;
        AppStateController state;
        SensorControllerPlugin sensorControllerPlugin;
        SensorController distanceSensor;
        SensorController temperatureSensor;
        SensorController humiditySensor;

        void setup();
        void loop();
                
    private:
        App();
        Logger _logger;
    };
}

#endif