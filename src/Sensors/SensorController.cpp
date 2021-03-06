#include "Sensors/SensorController.h"
#include "UI/UI.h"

namespace Dough
{
    SensorController::SensorController(
        SensorBase *sensor,
        SensorControllerPluginBase *plugin,
        unsigned int storageSize,
        unsigned int minimumMeasureTime,
        unsigned int minimumPublishTime) : sensor(sensor),
                                           _plugin(plugin),
                                           _storageSize(storageSize),
                                           _minimumMeasureTime(minimumMeasureTime),
                                           _minimumPublishTime(minimumPublishTime) {}

    const char *SensorController::getSensorName()
    {
        return sensor->getName();
    }

    unsigned int SensorController::getPrecision()
    {
        return sensor->getPrecision();
    }

    void SensorController::setup()
    {
        sensor->setup();

        // Initialize the storage for holding measurements. This storage is used
        // as a circular buffer, and it helps in computing an over time average
        // value for the collected measurements. The bigger the storage size, the
        // more values will be included for the average computation.
        _storage = new Measurement *[_storageSize];
        for (unsigned int i = 0; i < _storageSize; i++)
        {
            _storage[i] = new Measurement;
        }
        clearHistory();
    }

    bool SensorController::loop()
    {
        if (_mustMeasure())
        {
            _plugin->beforeMeasure(this);
            _lastMeasuredAt = millis();
            _store(readSensor());
            _plugin->afterMeasure(this);
        }
        
        if (_mustPublish())
        {
            _plugin->beforePublish(this);
            auto last = getLast();
            auto average = getAverage();
            _lastPublishedAt = millis();
            average.copyTo(&_lastPublishedAverage);
            last.copyTo(&_lastPublished);
            _plugin->doPublish(this, last, average);
            _plugin->afterPublish(this);
            return true;
        }

        return false;
    }

    bool SensorController::_mustMeasure()
    {
        // When no measurement was done yet, then do one now.
        if (_lastMeasuredAt == 0)
        {
            return true;
        }

        // When enough time has passed since the last measurement,
        // then start another measurement.
        auto now = millis();
        auto delta = now - _lastMeasuredAt;
        return delta >= (_minimumMeasureTime * 1000);
    }

    bool SensorController::_mustPublish()
    {
        Measurement lastMeasurement = getLast();

        // When the measurement failed, then there's no need to publish it.
        if (lastMeasurement.ok == false)
        {
            return false;
        }

        // When no data was published before, then this is a great time to do so.
        if (_lastPublished.ok == false)
        {
            return true;
        }

        // If the value did not change, only publish when the minimum publishing
        // time has passed.
        if (_lastPublished.value == lastMeasurement.value)
        {
            auto now = millis();
            auto delta = now - _lastPublishedAt;
            return _lastPublishedAt == 0 || delta >= (_minimumPublishTime * 1000);
        }

        auto precision = sensor->getPrecision();

        // When there is a significant change in the sensor value, then publish.
        if (abs(_lastPublished.value - lastMeasurement.value) >= precision)
        {
            return true;
        }

        auto average = getAverage();

        // When there is a significant change in the average value, then publish.
        if (average.ok && abs(_lastPublishedAverage.value - average.value) >= precision)
        {
            return true;
        }

        // When the value changed less than the significant change, but it reached
        // the current average value, then publish it, since we might have reached
        // a stable value.
        if (average.ok && average.value == lastMeasurement.value)
        {
            return true;
        }

        // Well, we're out of options. No reason to publish the data right now.
        return false;
    }

    void SensorController::_store(Measurement measurement)
    {
        measurement.copyTo(_storage[_next()]);

        if (measurement.ok)
        {
            _averageCount++;
            _averageSum += measurement.value;
        }
    }

    unsigned int SensorController::_next()
    {
        _index++;

        // Wrap around at the end of the circular buffer.
        if (_index == _storageSize)
        {
            _index = 0;
        }

        // If the new position contains an ok value, update the running totals.
        if (_storage[_index]->ok)
        {
            _averageSum -= _storage[_index]->value;
            _averageCount--;
        }

        _storage[_index]->clear();

        return _index;
    }

    Measurement SensorController::readSensor()
    {
        return sensor->read();
    }

    Measurement SensorController::getLast()
    {
        return *_storage[_index];
    }

    Measurement SensorController::getAverage()
    {
        return _averageCount > 0
                   ? Measurement::Value(round(_averageSum / _averageCount))
                   : Measurement::Failed();
    }

    void SensorController::clearHistory()
    {
        _averageCount = 0;
        _averageSum = 0;
        for (unsigned int i = 0; i < _storageSize; i++)
        {
            _storage[i]->clear();
        }
    }
} // namespace Dough