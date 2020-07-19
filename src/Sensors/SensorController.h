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
    // from a sensor and when to publish measurements (after significant
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
            virtual void doPublish(SensorController *controller, Measurement last, Measurement average) {};
            virtual void afterPublish(SensorController *controller) {};
    };

    class SensorController
    {
    public:
        // Create a new Measurements object.
        //
        // @param sensor
        //     The Dough::SensorBase implementation to wrap in this controller.
        // @param plugin
        //     The Dough::SensorControllerPluginBase implementation to use.
        //     This plugin is used to decouple network / user interface functionality
        //     from the sensor controller code.
        // @param storageSize
        //     Number of measurements to keep track of for computing an average.
        // @param minimumMeasureTime
        //     The number of seconds after which to read the next measurement
        //     from the sensor.
        // @param minimumPublishTime
        //     The number of seconds after which to forcibly publish measurements
        //     to MQTT, even when no significant changes to measurements were seen.
        //     "Significant change" is defined by the Dough::SensorBase implementation.
        SensorController(
            SensorBase *sensor,
            SensorControllerPluginBase *plugin,
            unsigned int storageSize,
            unsigned int minimumMeasureTime,
            unsigned int minimumPublishTime);
        SensorBase *sensor;
        void setup();

        // Return the name for the contained sensor.
        const char *getSensorName();

        // Read the sensor and publish the results when needed.
        // This method returns true when the last read value was an ok measurement.
        // This means that after this method returns true, getLast() will return
        // an ok Dough::Measurement.
        bool loop();
        
        // Read a measurement from the sensor.
        Measurement readSensor();

        // Returns the last measurement that was done for the sensor.
        // This might be a failed measurement, in which case the ok field
        // of the result will be set to false.
        Measurement getLast();

        // Returns the running average value for the sensor readings so far.
        // When no successful measurements were done at all, then the ok
        // field of the result will be set to false.
        Measurement getAverage();

        // Clear the collected data for the running average, letting the
        // average computation start from a clean slate.
        void clearHistory();

    private:
        SensorControllerPluginBase *_plugin;

        // Members related to the measurement storage.
        Measurement **_storage;
        unsigned int _storageSize;
        int _averageSum = 0;
        unsigned int _averageCount = 0;
        unsigned int _index = 0;
        void _store(Measurement measurement);
        unsigned int _next();

        // Members used for controlling measurements.
        bool _mustMeasure();
        unsigned int _minimumMeasureTime;
        unsigned long _lastMeasuredAt = 0;

        // Members used for controlling publishing of measurements.
        bool _mustPublish();
        unsigned int _minimumPublishTime;
        unsigned long _lastPublishedAt = 0;
        Measurement _lastPublished;
        Measurement _lastPublishedAverage;
    };
} // namespace Dough

#endif
