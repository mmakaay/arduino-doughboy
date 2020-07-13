#ifndef DOUGH_CONFIG_H
#define DOUGH_CONFIG_H

#include <Arduino.h>
//#include "App/App.h"
#include "Network/MQTT.h"
#include "UI/Logger.h"
#include "Sensors/LowLevel/SensorHCSR04.h"

namespace Dough
{
    // This class is responsible for handling the device configuration.
    // Configuration parameters are stored as persistent data in the
    // MQTT broker. The device itself has no permanent storage peripheral.
    // Therefore, this class hooks into MQTT.
    class Configuration
    {
    public:
        Configuration();
        void setup();
        static void handleMqttConnect(MQTT *mqtt);
        static void handleMqttMessage(String &key, String &value);
        void setContainerHeight(int height);
        unsigned int getContainerHeight();
        void setTemperatureOffset(int offset);
        int getTemperatureOffset();
        bool isOk();

    private:
        MQTT *_mqtt;
        Logger _logger;
        unsigned int _containerHeight;
        bool _containerHeightSet;
        int _temperatureOffset = 0;
    };
}

#endif