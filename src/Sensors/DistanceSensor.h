#ifndef DOUGH_SENSORS_DISTANCE_H
#define DOUGH_SENSORS_DISTANCE_H

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
        virtual void setup();
        virtual Measurement read();
        void setTemperature(int temperature);
        void setHumidity(int humidity);

    private:
        DistanceSensor();
        static DistanceSensor *_instance;
        Logger _logger;
        SensorHCSR04 *_hcsr04;
    };
}

#endif