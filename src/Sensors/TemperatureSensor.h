#ifndef DOUGH_SENSORS_TEMPERATURE_H
#define DOUGH_SENSORS_TEMPERATURE_H

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

    private:
        TemperatureSensor();
        static TemperatureSensor *_instance;
        Logger _logger;
    };
} // namespace Dough

#endif