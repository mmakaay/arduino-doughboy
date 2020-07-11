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
// value. These definition define the number of measurements to include in the
// average computation.
#define TEMPERATURE_AVG_LOOKBACK 10      // making this a 5 minute average
#define HUMIDITY_AVG_LOOKBACK 10         // making this a 5 minute average
#define DISTANCE_AVG_LOOKBACK 28 * 2 * 5 // making this a 5 minute average

// The minimal interval at which to publish measurements to the MQTT broker.
// When significant changes occur in the measurements, then these will be published
// to the MQTT broker at all times, independent from this interval.
#define PUBLISH_INTERVAL 4000

#include <Arduino.h>
#include "Data/Measurements.h"
#include "Sensors/DoughSensors.h"
#include "Network/DoughWiFi.h"
#include "Network/DoughMQTT.h"
#include "UI/DoughUI.h"
#include "UI/DoughLogger.h"

typedef enum
{
    SAMPLE_TEMPERATURE,
    SAMPLE_HUMIDITY,
    SAMPLE_DISTANCE
} DoughSampleType;

/**
 * This class is responsible for handling all things "data".
 * It holds the device configuration, collects measurements from sensors,
 * gathers statistics on these data, and publishing results to the MQTT broker.
 */
class DataController
{
public:
    static DataController* Instance();
    void setup();
    void loop();
    void clearHistory();
    void setContainerHeight(int height);
    bool isConfigured();    
    static void handleMqttConnect(DoughMQTT* mqtt);
    static void handleMqttMessage(String &key, String &value);

private:
    DataController();
    static DataController* _instance;
    Measurements _temperatureMeasurements;
    Measurements _humidityMeasurements;
    Measurements _distanceMeasurements;
    DoughLogger _logger;
    DoughUI* _ui;
    DoughSensors* _sensors;
    DoughMQTT* _mqtt;
    unsigned long _lastSample = 0;
    unsigned long _lastPublish = 0;
    DoughSampleType _sampleType = SAMPLE_TEMPERATURE;
    int _sampleCounter = 0;
    int _containerHeight;
    bool _containerHeightSet;
    void _sample();
    void _publish();
};

#endif
