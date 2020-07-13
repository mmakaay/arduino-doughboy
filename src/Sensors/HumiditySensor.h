#ifndef DOUGH_HUMIDITY_SENSOR_H
#define DOUGH_HUMIDITY_SENSOR_H

#include "Sensors/SensorBase.h"
#include "Sensors/LowLevel/SensorDHT11.h"
#include "UI/Logger.h"
#include "Data/Measurement.h"
#include "Sensors/DistanceSensor.h"
#include "config.h"

namespace Dough
{
    // This class provides access to the humidity sensor in the device.
    class HumiditySensor : public SensorBase
    {
    public:
        static HumiditySensor *Instance();
        virtual void setup();
        virtual Measurement read();
        virtual unsigned int getPrecision();

    private:
        HumiditySensor();
        Logger _logger;
    };
}

#endif