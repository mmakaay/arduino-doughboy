#ifndef DOUGH_MEASUREMENTS_H
#define DOUGH_MEASUREMENTS_H

#include <Arduino.h>
#include "Sensors/SensorBase.h"
#include "Data/Measurement.h"
#include "Network/MQTT.h"

namespace Dough
{
    typedef void (*SensorControllerCallback)();
    
    // This class is used to store measurements for a sensor and to keep
    // track of running totals for handling average computations.
    // It also provides functionality to decide when to read a measurement
    // from a sensor and when to publish measurements to MQTT (after significant
    // changes occur or when the last publish was too long ago).
    class SensorController
    {
    public:
        // Create a new Measurements object.
        //
        // @param mqtt
        //     The Dough::MQTT object, which is connected to the MQTT broker.
        // @param mqttKey
        //     The key to use when publishing sensor values to MQTT.
        //     The full key will be "<prefix>/<mqttKey>" for measurement values
        //     and "<prefix>/<mqttKey>/average" for average values.
        // @param sensor
        //     The sensor to read, implements SensorBase.
        // @param storageSize
        //     Number of measurements to keep track of for computing an average.
        // @param minimumMeasureTime
        //     The number of seconds after which to read the next measurement
        //     from the sensor.
        // @param onMeasure
        //     A callback function that is called right before a measurement
        //     is taken.
        // @param minimumPublishTime
        //     The number of seconds after which to forcibly publish measurements
        //     to MQTT, even when no significant changes to measurements were seen.
        // @param onPublish
        //     A callback function that is called right before a measurement
        //     is published.
        SensorController(
            MQTT *mqtt,
            const char *mqttKey,
            SensorBase *sensor,
            unsigned int storageSize,
            unsigned int minimumMeasureTime,
            SensorControllerCallback onMeasure,
            unsigned int minimumPublishTime,
            SensorControllerCallback onPublish);
        void setup();
        void loop();
        Measurement getLast();
        Measurement getAverage();
        void clearHistory();

    private:
        MQTT *_mqtt;
        const char *_mqttKey;
        char *_mqttAverageKey;
        SensorBase *_sensor;
        Measurement **_storage;
        unsigned int _storageSize;
        int _averageSum = 0;
        unsigned int _averageCount = 0;
        unsigned int _index = 0;
        bool _mustMeasure();
        SensorControllerCallback _onMeasure;
        void _measure();
        unsigned int _minimumMeasureTime; 
        unsigned long _lastMeasuredAt = 0;
        bool _mustPublish();
        SensorControllerCallback _onPublish ;
        void _publish();
        unsigned int _minimumPublishTime;
        unsigned long _lastPublishedAt = 0;
        Measurement _lastPublished;
        Measurement _lastPublishedAverage;
        void _store(Measurement measurement);
        unsigned int _next();
    };
}

#endif
