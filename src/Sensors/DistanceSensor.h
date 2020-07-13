#ifndef DOUGH_DISTANCE_SENSOR_H
#define DOUGH_DISTANCE_SENSOR_H

#include "Sensors/SensorBase.h"
#include "Sensors/LowLevel/SensorHCSR04.h"
#include "UI/Logger.h"
#include "Data/Measurement.h"
#include "config.h"

namespace Dough
{
    // This class provides access to the distance sensor in the device.
    class DistanceSensor : public SensorBase
    {
    public:
        static DistanceSensor *Instance();
        void setTemperature(int temperature);
        void setHumidity(int humidity);
        virtual void setup();
        virtual Measurement read();
        virtual unsigned int getPrecision();

    private:
        DistanceSensor();
        Logger _logger;
        SensorHCSR04 *_hcsr04;
    };
}

#endif