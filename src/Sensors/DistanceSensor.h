#ifndef DOUGH_SENSORS_DISTANCE_H
#define DOUGH_SENSORS_DISTANCE_H

#include "Sensors/SensorBase.h"
#include "Sensors/LowLevel/SensorHCSR04.h"
#include "UI/DoughLogger.h"
#include "Data/Measurement.h"
#include "config.h"

/**
 * This class provides access to the distance sensor in the device.
 */
class DistanceSensor : public SensorBase
{
public:
    static DistanceSensor *Instance();
    void setup();
    void setTemperature(int temperature);
    void setHumidity(int humidity);
    Measurement read();

private:
    DistanceSensor();
    static DistanceSensor *_instance;
    DoughLogger _logger;
    SensorHCSR04 *_hcsr04;
};

#endif
