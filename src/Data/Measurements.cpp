#include "Data/Measurements.h"
#include "UI/DoughUI.h"

Measurements::Measurements(unsigned int avgLookback)
{
    _storageSize = avgLookback;

    _storage = new Measurement*[avgLookback];
    for (unsigned int i = 0; i < avgLookback; i++) {
        _storage[i] = new Measurement;
    }
    clearHistory();
}

void Measurements::add(Measurement measurement)
{
    unsigned int index = _next();
    _storage[index]->ok = measurement.ok;
    _storage[index]->value = measurement.value;
    
    if (measurement.ok)
    {
        _averageCount++;
        _averageSum += measurement.value;
    }
}

unsigned int Measurements::_next()
{
    _index++;
    if (_index == _storageSize)
    {
        _index = 0;
    }
    if (_storage[_index]->ok)
    {
        _averageSum -= _storage[_index]->value;
        _averageCount--;
    }
    _storage[_index]->ok = false;
    _storage[_index]->value = 0;
    return _index;
}

Measurement Measurements::getLast()
{
    return *_storage[_index];
}

Measurement Measurements::getAverage()
{
    Measurement result;
    if (_averageCount > 0)
    {
        result.ok = true;
        result.value = round(_averageSum / _averageCount);
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
