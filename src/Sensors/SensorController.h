#ifndef DOUGH_SENSORCONTROLLER_H
#define DOUGH_SENSORCONTROLLER_H

#include <Arduino.h>
#include "Sensors/HighLevel/SensorBase.h"
#include "Data/Measurement.h"
#include "Network/MQTT.h"

namespace Dough
{
    // This class is used to store measurements for a sensor and to keep
    // track of running totals for handling average computations.
    // It also provides functionality to decide when to read a measurement
    // from a sensor and when to publish measurements  (after significant
    // changes occur or when the last publish was too long ago).
    class SensorController;

    // This class can be derived from to create a plugin module for the
    // Dough::SensorController. Methods from this interface will be called
    // by the Dough::SensorController at appropriate times.
    class SensorControllerPluginBase
    {
        public:
            virtual void beforeMeasure(SensorController *controller) {};
            virtual void afterMeasure(SensorController *controller) {};
            virtual void beforePublish(SensorController *controller) {};
            virtual void doPublish(SensorController *controller) {};
            virtual void afterPublish(SensorController *controller) {};
    };

    class SensorController
    {
    public:
        // Create a new Measurements object.
        //
        // @param plugin
        //     The Dough::SensorControllerPluginBase to use.
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
        //     is taken using the contained sensor object.
        // @param minimumPublishTime
        //     The number of seconds after which to forcibly publish measurements
        //     to MQTT, even when no significant changes to measurements were seen.
        // @param onPublish
        //     A callback function that is called right before a measurement
        //     is published.
        SensorController(
            SensorControllerPluginBase *plugin,
            MQTT *mqtt,
            const char *mqttKey,
            SensorBase &sensor,
            unsigned int storageSize,
            unsigned int minimumMeasureTime,
            unsigned int minimumPublishTime);
        void setup();
        void loop();
        Measurement getLast();
        Measurement getAverage();
        void clearHistory();

    private:
        SensorControllerPluginBase *_plugin;
        MQTT *_mqtt;
        const char *_mqttKey;
        char *_mqttAverageKey;
        SensorBase &_sensor;
        Measurement **_storage;
        unsigned int _storageSize;
        int _averageSum = 0;
        unsigned int _averageCount = 0;
        unsigned int _index = 0;
        bool _mustMeasure();
        void _measure();
        unsigned int _minimumMeasureTime;
        unsigned long _lastMeasuredAt = 0;
        bool _mustPublish();
        void _publish();
        unsigned int _minimumPublishTime;
        unsigned long _lastPublishedAt = 0;
        Measurement _lastPublished;
        Measurement _lastPublishedAverage;
        void _store(Measurement measurement);
        unsigned int _next();
    };
} // namespace Dough

#endif
