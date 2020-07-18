#ifndef DOUGH_HUMIDITY_SENSOR_H
#define DOUGH_HUMIDITY_SENSOR_H

#include "Sensors/HighLevel/SensorBase.h"
#include "Sensors/LowLevel/SensorDHT11.h"
#include "UI/Logger.h"
#include "Data/Measurement.h"
#include "config.h"

namespace Dough
{
    // This class provides access to the humidity sensor in the device.
    class HumiditySensor : public SensorBase
    {
    public:
        HumiditySensor();
        virtual void setup();
        virtual Measurement read();
        virtual unsigned int getPrecision();

    private:
        Logger _logger;
    };
}

#endif