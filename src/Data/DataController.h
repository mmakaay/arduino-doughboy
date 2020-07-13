#ifndef DOUGH_DATA_DATACONTROLLER_H
#define DOUGH_DATA_DATACONTROLLER_H

// The minimal interval in seconds at which to forcibly publish measurements to the
// MQTT broker. When significant changes occur in the measurements, then these will
// be published to the MQTT broker at all times, independent from this interval.
#define PUBLISH_INTERVAL 300

#include <Arduino.h>
#include "Data/SensorController.h"
#include "Sensors/TemperatureSensor.h"
#include "Sensors/HumiditySensor.h"
#include "Sensors/DistanceSensor.h"
#include "Network/WiFi.h"
#include "Network/MQTT.h"
#include "UI/UI.h"
#include "UI/Logger.h"

namespace Dough
{
    typedef enum
    {
        SAMPLE_TEMPERATURE,
        SAMPLE_HUMIDITY,
        SAMPLE_DISTANCE
    } DoughSampleType;

    // This class is responsible for handling all things "data".
    // It holds the device configuration, collects measurements from sensors,
    // gathers statistics on these data, and publishing results to the MQTT broker.
    class DataController
    {
    public:
        static DataController *Instance();
        void setup();
        void loop();
        void clearHistory();
        void setContainerHeight(int height);
        void setTemperatureOffset(int offset);
        bool isConfigured();
        static void handleMqttConnect(MQTT *mqtt);
        static void handleMqttMessage(String &key, String &value);

    private:
        DataController();
        UI *_ui;
        MQTT *_mqtt;
        SensorController _temperatureMeasurements;
        SensorController _humidityMeasurements;
        SensorController _distanceMeasurements;
        Logger _logger;
        unsigned int _containerHeight;
        bool _containerHeightSet;
        int _temperatureOffset = 0;
        void _sample();
    };
}

#endif