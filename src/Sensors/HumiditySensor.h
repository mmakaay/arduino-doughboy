#ifndef DOUGH_SENSORS_HUMIDITY_H
#define DOUGH_SENSORS_HUMIDITY_H

#include "Sensors/SensorBase.h"
#include "Sensors/LowLevel/SensorDHT11.h"
#include "UI/DoughLogger.h"
#include "Data/Measurement.h"
#include "Sensors/DistanceSensor.h"
#include "config.h"

/**
 * This class provides access to the humidity sensor in the device.
 */
class HumiditySensor : public SensorBase
{
public:
    static HumiditySensor *Instance();
    virtual void setup();
    virtual Measurement read();

private:
    HumiditySensor();
    static HumiditySensor *_instance;
    DoughLogger _logger;
};

#endif