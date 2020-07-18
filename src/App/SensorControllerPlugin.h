#ifndef DOUGH_SENSORCONTROLLER_PLUGINS_H
#define DOUGH_SENSORCONTROLLER_PLUGINS_H

#include <Arduino.h>
#include "UI/Logger.h"
#include "Sensors/SensorController.h"
#include "Network/MQTT.h"
#include "UI/UI.h"

namespace Dough
{
    // This class is a plugin for the Dough::SensorController. It takes care
    // of notifying events via the device UI and publishing data via the MQTT broker.
    // This decouples the sensor controller for the network and user interface specifics.
    class SensorControllerPlugin : public SensorControllerPluginBase
    {
    public:
        SensorControllerPlugin(MQTT *mqtt, UI *ui);
        virtual void beforeMeasure(SensorController *controller);
        virtual void afterMeasure(SensorController *controller);
        virtual void beforePublish(SensorController *controller);
        virtual void doPublish(SensorController *controller, Measurement last, Measurement average);

    private:
        MQTT *_mqtt;
        UI *_ui;
    };
} // namespace Dough

#endif