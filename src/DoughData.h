#ifndef DOUGH_DATA_H
#define DOUGH_DATA_H

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
#define TEMPERATURE_AVG_LOOKBACK 10       // making this a 5 minute average
#define HUMIDITY_AVG_LOOKBACK 10          // making this a 5 minute average
#define DISTANCE_AVG_LOOKBACK 28 * 2 * 5  // making this a 5 minute average

// The minimal interval at which to publish measurements to the MQTT broker.
// When significant changes occur in the measurements, then these will be published
// to the MQTT broker at all times, independent from this interval.
#define PUBLISH_INTERVAL 4000

#include <Arduino.h>
#include "DoughSensors.h"
#include "DoughNetwork.h"
#include "DoughMQTT.h"
#include "DoughUI.h"

typedef enum {
    SAMPLE_TEMPERATURE,
    SAMPLE_HUMIDITY,
    SAMPLE_DISTANCE
} DoughSampleType;

/**
 * The DoughDataMeasurement struct represents a single measurement.
 */
struct DoughDataMeasurement {
    public:
        int value = 0;
        bool ok = false;
};

/**
 * The DoughDataMeasurements class is used to store measurements for a sensor
 * and to keep track of running totals for handling average computations.
 */
class DoughDataMeasurements {
    public:
        DoughDataMeasurements(int avgLookback);
        void registerValue(int value);
        void registerFailed();
        DoughDataMeasurement getLast();
        DoughDataMeasurement getAverage();
        void clearHistory();
    private:
        DoughDataMeasurement* _storage;
        unsigned int _storageSize;
        int _averageSum = 0;
        unsigned int _averageCount = 0;
        unsigned int _index = 0;
        DoughDataMeasurement* _next();
};

/**
 * The DoughData class is responsible for holding the device configuration,
 * collecting measurements from sensors, gathering the statistics on these data,
 * and publishing results to the MQTT broker.
 */
class DoughData {
    public:
        static DoughData* Instance();
	    void setup();
        void loop();
        void clearHistory();
        void setContainerHeight(int height);
        bool isConfigured();
        static void handleMqttConnect(DoughMQTT *mqtt);
        static void handleMqttMessage(String &key, String &value);
    
    private:
        DoughData();
        static DoughData* _instance;
        DoughSensors * _sensors;
        unsigned long _lastSample = 0;
        DoughSampleType _sampleType = SAMPLE_TEMPERATURE;
        int _sampleCounter = 0;
        int _containerHeight;
        bool _containerHeightSet;
        void _sample();
        void _publish();
        DoughDataMeasurements _temperatureMeasurements;
        DoughDataMeasurements _humidityMeasurements;
        DoughDataMeasurements _distanceMeasurements;
};

#endif
