#ifndef DOUGH_DATA_MEASUREMENTS_H
#define DOUGH_DATA_MEASUREMENTS_H

#include <Arduino.h>
#include "Data/Measurement.h"

/**
 * This class is used to store measurements for a sensor and to keep
 * track of running totals for handling average computations.
 */
class Measurements
{
public:
    Measurements(unsigned int avgLookback);
    void add(Measurement measurement);
    Measurement getLast();
    Measurement getAverage();
    void clearHistory();

private:
    Measurement** _storage;
    unsigned int _storageSize;
    int _averageSum = 0;
    unsigned int _averageCount = 0;
    unsigned int _index = 0;
    unsigned int _next();
};

#endif
