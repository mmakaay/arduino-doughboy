#include "App/Configuration.h"

namespace Dough
{
    Configuration::Configuration() : _logger("CONFIG")
    {
//        _mqtt = MQTT::Instance();
    }

    void Configuration::setup()
    {
        _containerHeight = 0.00;
        _containerHeightSet = false;
        _temperatureOffset = 0;

        // MQTT *mqtt = MQTT::Instance();
        // mqtt->onConnect(Configuration::handleMqttConnect);
        // mqtt->onMessage(Configuration::handleMqttMessage);
    }

    // void Configuration::handleMqttConnect(MQTT *mqtt)
    // {
    //     mqtt->subscribe("container_height");
    //     mqtt->subscribe("temperature_offset");
    // }

    // void Configuration::handleMqttMessage(String &key, String &payload)
    // {
    //     if (key.equals("container_height"))
    //     {
    //         App::Instance()->config.setContainerHeight(payload.toInt());
    //     }
    //     if (key.equals("temperature_offset"))
    //     {
    //         App::Instance()->config.setTemperatureOffset(payload.toInt());
    //     }
    //     else
    //     {
    //         App::Instance()->config._logger.log("sS", "ERROR - Unhandled MQTT message, key = ", key);
    //     }
    // }

    bool Configuration::isOk()
    {
        return _containerHeightSet;
    }

    // Set the container height in mm. This is the distance between the sensor
    // and the bottom of the container. It is used to determine the height of
    // the starter or dough by subtracting the distance measurement from it.
    void Configuration::setContainerHeight(int height)
    {
        _containerHeightSet = false;
        if (height <= HCSR04_MIN_MM)
        {
            _logger.log("sisis", "ERROR - Container height ", height,
                        "mm is less than the minimum measuring distance of ",
                        HCSR04_MIN_MM, "mm");
            return;
        }
        if (height >= HCSR04_MAX_MM)
        {
            _logger.log("sisis", "ERROR - Container height ", height,
                        "mm is more than the maximum measuring distance of ",
                        HCSR04_MAX_MM, "mm");
            return;
        }
        _logger.log("sis", "Set container height to ", height, "mm");
        _containerHeight = height;
        _containerHeightSet = true;
    }

    unsigned int Configuration::getContainerHeight()
    {
        return _containerHeight;
    }

    // Set the temperature offset in °C, to calibrate the temperature
    // as measured by the device. On my device, the temperature raises about
    // 5 degrees in the first few minutes, due to the circuitry warming up.
    // I will have to fix that in the hardware as well, because 5 degrees is
    // just redicoulous, but there might always be a diff that needs to be
    // compensated for.
    void Configuration::setTemperatureOffset(int offset)
    {
        // Just a little safety measure. Devices should never need more than
        // 10 degrees correction, right?
        if (offset < -10 || offset > 10)
        {
            _logger.log("sisis", "ERROR - Temperature offset ", offset,
                        "°C is too large (allowed range: -10°C up to +10°C)");
            return;
        }
        _logger.log("sis", "Set temperature offset to ", offset, "°C");
        _temperatureOffset = offset;
    }

    int Configuration::getTemperatureOffset()
    {
        return _temperatureOffset;
    }
}