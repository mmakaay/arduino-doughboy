#include "Data/SensorController.h"
#include "UI/UI.h"

namespace Dough
{
    SensorController::SensorController(
        MQTT *mqtt,
        const char *mqttKey,
        SensorBase *sensor,
        unsigned int storageSize,
        unsigned int minimumMeasureTime,
        SensorControllerCallback onMeasure,
        unsigned int minimumPublishTime,
        SensorControllerCallback onPublish)
    {
        _mqtt = mqtt;
        _mqttKey = mqttKey;
        _sensor = sensor;
        _storageSize = storageSize;
        _minimumMeasureTime = minimumMeasureTime;
        _onMeasure = onMeasure;
        _minimumPublishTime = minimumPublishTime;
        _onPublish = onPublish;
    }

    void SensorController::setup()
    {
        _sensor->setup();

        // Format the key to use for publishing the average (i.e. "<mqttKey>/average").
        auto lenAverageKey = strlen(_mqttKey) + 9; // +9 for the "/average\0" suffix
        _mqttAverageKey = new char[lenAverageKey];
        snprintf(_mqttAverageKey, lenAverageKey, "%s/average", _mqttKey);

        // Initialize the storage for holding the measurements.
        _storage = new Measurement *[_storageSize];
        for (unsigned int i = 0; i < _storageSize; i++)
        {
            _storage[i] = new Measurement;
        }
        clearHistory();
    }

    void SensorController::loop()
    {
        if (_mustMeasure())
        {
            _onMeasure();
            _measure();
        }
        if (_mustPublish())
        {
            _onPublish();
            _publish();
        }
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

    void SensorController::_measure()
    {
        _lastMeasuredAt = millis();
        
        _store(_sensor->read());
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

        auto precision = _sensor->getPrecision();

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

    void SensorController::_publish()
    {
        auto average = getAverage();
        auto last = getLast();

        _mqtt->publish(_mqttKey, last);
        _mqtt->publish(_mqttAverageKey, average);

        _lastPublishedAt = millis();
        average.copyTo(&_lastPublishedAverage);
        last.copyTo(&_lastPublished);
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
}