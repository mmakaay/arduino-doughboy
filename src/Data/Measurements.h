#ifndef DOUGH_DATA_MEASUREMENTS_H
#define DOUGH_DATA_MEASUREMENTS_H

#include <Arduino.h>
#include "Sensors/SensorBase.h"
#include "Data/Measurement.h"
#include "Network/DoughMQTT.h"

/**
 * This class is used to store measurements for a sensor and to keep
 * track of running totals for handling average computations.
 * It also provides functionality to decide when to publish measurements
 * to MQTT (after significant changes occur or when the last publish
 * was too long ago).
 */
class Measurements
{
public:
    /**
     * Create a new Measurements object.
     * 
     * @param sensor
     *     The sensor to read, implements SensorBase.
     * @param storageSize
     *     Number of measurements to keep track of for computing an average.
     * @param significantChange
     *     Number that describes how much a measurement value needs to change,
     *     before it is considered significant and must be published to MQTT.
     * @param minimumPublishTime
     *     The number of seconds after which to forcibly publish measurements
     *     to MQTT, even when no significant changes to measurements were seen.
     */
    Measurements(
        const char *mqttKey,
        SensorBase *sensor,
        unsigned int storageSize,
        unsigned int significantChange,
        unsigned int minimumPublishTime);
    void setup();
    void process();
    Measurement getLast();
    Measurement getAverage();
    void clearHistory();

private:
    DoughMQTT *_mqtt;
    const char *_mqttKey;
    char *_mqttAverageKey;
    SensorBase *_sensor;
    Measurement **_storage;
    unsigned int _storageSize;
    unsigned int _significantChange;
    unsigned int _minimumPublishTime;
    int _averageSum = 0;
    unsigned int _averageCount = 0;
    unsigned int _index = 0;
    unsigned long _lastPublishedAt = 0;
    Measurement _lastPublished;
    Measurement _lastPublishedAverage;
    bool _mustPublish();
    void _publish();
    void _store(Measurement measurement);
    unsigned int _next();
};

#endif
