#include "Data/DataController.h"
#include "Data/Measurement.h"

typedef Measurement *MeasurementPtr;

Measurements::Measurements(unsigned int avgLookback)
{
    _storageSize = avgLookback;
    _storage = new MeasurementPtr[avgLookback];
    for (unsigned int i = 0; i < avgLookback; i++)
    {
        _storage[i] = nullptr;
    }
}

void Measurements::add(Measurement *measurement)
{
    auto index = _next();
    _storage[index] = measurement;
    if (measurement->ok)
    {
        _averageCount++;
        _averageSum += measurement->value;
    }
}

unsigned int Measurements::_next()
{
    _index++;
    if (_index == _storageSize)
    {
        _index = 0;
    }
    if (_storage[_index] != nullptr && _storage[_index]->ok)
    {
        _averageSum -= _storage[_index]->value;
        _averageCount--;
    }
    return _index;
}

Measurement *Measurements::getLast()
{
    return _storage[_index];
}

Measurement *Measurements::getAverage()
{
    auto result = new Measurement();
    if (_averageCount > 0)
    {
        result->ok = true;
        result->value = round(_averageSum / _averageCount);
    }
    return result;
}

void Measurements::clearHistory()
{
    _averageCount = 0;
    _averageSum = 0;
    for (unsigned int i = 0; i < _storageSize; i++)
    {
        _storage[i]->ok = false;
        _storage[i]->value = 0;
    }
}
