#ifndef DOUGH_SENSOR_BASE_H
#define DOUGH_SENSOR_BASE_H

#include "Data/Measurement.h"

namespace Dough
{
    // This interface is implemented by all high level sensors.
    class SensorBase
    {
    public:
        virtual void setup();
        virtual Measurement read();
        virtual unsigned int getPrecision();
    };
}

#endif