#ifndef DOUGH_SENSORS_BASE_H
#define DOUGH_SENSORS_BASE_H

#include "Data/Measurement.h"

namespace Dough
{
    // This interface is implemented by all sensors.
    class SensorBase
    {
    public:
        virtual void setup();
        virtual Measurement read();
        virtual unsigned int getPrecision();
    };
}

#endif