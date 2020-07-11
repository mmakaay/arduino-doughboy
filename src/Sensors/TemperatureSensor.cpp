#include "TemperatureSensor.h"

// ----------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------

TemperatureSensor *TemperatureSensor::_instance = nullptr;

TemperatureSensor *TemperatureSensor::Instance()
{
    if (TemperatureSensor::_instance == nullptr)
    {
        TemperatureSensor::_instance = new TemperatureSensor();
    }
    return TemperatureSensor::_instance;
}

TemperatureSensor::TemperatureSensor() : _logger("TEMPERATURE") {}

// ----------------------------------------------------------------------
// setup
// ----------------------------------------------------------------------

void TemperatureSensor::setup()
{
    SensorDHT11::Instance()->begin();
}

// ----------------------------------------------------------------------
// loop
// ----------------------------------------------------------------------

Measurement TemperatureSensor::read()
{
    float t = SensorDHT11::Instance()->readTemperature();
    if (isnan(t))
    {
        _logger.log("s", "ERROR - Temperature measurement failed");
        return Measurement::Failed();
    }
    else
    {
        _logger.log("sis", "Temperature = ", int(t), "°C");
        DistanceSensor::Instance()->setTemperature(int(t));
        return Measurement::Value(int(t));
    }
}