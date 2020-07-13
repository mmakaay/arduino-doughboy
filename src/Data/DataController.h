#ifndef DOUGH_DATA_DATACONTROLLER_H
#define DOUGH_DATA_DATACONTROLLER_H

// These definitions describes what measurements are performed in sequence.
// One measurement is done every SAMPLE_INTERVAL microseconds.
// We always start with a temperature measurement, then a humidity measurement,
// and finally a number of distance measurements.
// The SAMPLE_CYCLE_LENGTH defines the total number of samples in this sequence.
#define SAMPLE_INTERVAL 1000
#define SAMPLE_CYCLE_LENGTH 30 // 1 temperature + 1 humidity + 28 distance samples

// Two different values are published per sensor: a recent value and an average
// value. These definition define the number of recent measurements to include
// in the average computation.
#define TEMPERATURE_AVG_LOOKBACK 6       // making this a 3 minute average
#define HUMIDITY_AVG_LOOKBACK 6          // making this a 3 minute average
#define DISTANCE_AVG_LOOKBACK 28 * 2 * 3 // making this a 3 minute average

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
        unsigned long _lastSample = 0;
        DoughSampleType _sampleType = SAMPLE_TEMPERATURE;
        int _sampleCounter = 0;
        int _containerHeight;
        bool _containerHeightSet;
        void _sample();
    };
}

#endif