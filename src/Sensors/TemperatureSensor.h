#ifndef DOUGH_TEMPERATURE_SENSOR_H
#define DOUGH_TEMPERATURE_SENSOR_H

#include "Sensors/SensorBase.h"
#include "Sensors/LowLevel/SensorDHT11.h"
#include "UI/Logger.h"
#include "Data/Measurement.h"
#include "Sensors/DistanceSensor.h"
#include "config.h"

namespace Dough
{
    // This class provides access to the temperature sensor in the device.
    class TemperatureSensor : public SensorBase
    {
    public:
        static TemperatureSensor *Instance();
        virtual void setup();
        virtual Measurement read();
        virtual unsigned int getPrecision();

    private:
        TemperatureSensor();
        Logger _logger;
    };
}

#endif