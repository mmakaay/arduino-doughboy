#ifndef DOUGH_SENSORS_BASE_H
#define DOUGH_SENSORS_BASE_H

#include "Data/Measurement.h"

/**
 * This interface is implemented by all sensors.
 */
class SensorBase
{
public:
    virtual void setup();
    virtual Measurement read();
};

#endif