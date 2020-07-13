#include "Data/DataController.h"

namespace Dough
{
    // ----------------------------------------------------------------------
    // Constructor
    // ----------------------------------------------------------------------

    DataController *DataController::Instance()
    {
        static DataController *_instance = new DataController();
        return _instance;
    }

    DataController::DataController() : _temperatureMeasurements(
                                           "temperature",
                                           TemperatureSensor::Instance(),
                                           TEMPERATURE_AVERAGE_STORAGE,
                                           TEMPERATURE_MEASURE_INTERVAL,
                                           PUBLISH_INTERVAL),
                                       _humidityMeasurements(
                                           "humidity",
                                           HumiditySensor::Instance(),
                                           HUMIDITY_AVERAGE_STORAGE,
                                           HUMIDITY_MEASURE_INTERVAL,
                                           PUBLISH_INTERVAL),
                                       _distanceMeasurements(
                                           "distance",
                                           DistanceSensor::Instance(),
                                           DISTANCE_AVERAGE_STORAGE,
                                           DISTANCE_MEASURE_INTERVAL,
                                           PUBLISH_INTERVAL),
                                       _logger("DATA")
    {
        _ui = UI::Instance();
        _mqtt = MQTT::Instance();
    }

    // ----------------------------------------------------------------------
    // Setup
    // ----------------------------------------------------------------------

    void DataController::setup()
    {
        _containerHeight = 0.00;
        _containerHeightSet = false;

        MQTT *mqtt = MQTT::Instance();
        mqtt->onConnect(DataController::handleMqttConnect);
        mqtt->onMessage(DataController::handleMqttMessage);

        _temperatureMeasurements.setup();
        _humidityMeasurements.setup();
        _distanceMeasurements.setup();
    }

    void DataController::handleMqttConnect(MQTT *mqtt)
    {
        mqtt->subscribe("container_height");
        mqtt->subscribe("temperature_offset");
    }

    void DataController::handleMqttMessage(String &key, String &payload)
    {
        if (key.equals("container_height"))
        {
            DataController::Instance()->setContainerHeight(payload.toInt());
        }
        if (key.equals("temperature_offset"))
        {
            DataController::Instance()->setTemperatureOffset(payload.toInt());
        }
        else
        {
            DataController::Instance()->_logger.log("sS", "ERROR - Unhandled MQTT message, key = ", key);
        }
    }

    bool DataController::isConfigured()
    {
        return _containerHeightSet;
    }

    // Set the container height in mm. This is the distance between the sensor
    // and the bottom of the container. It is used to determine the height of
    // the starter or dough by subtracting the distance measurement from it.
    void DataController::setContainerHeight(int height)
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

    // Set the temperature offset in °C, to calibrate the temperature
    // as measured by the device. On my device, the temperature raises about
    // 5 degrees in the first few minutes, due to the circuitry warming up.
    // I will have to fix that in the hardware as well, because 5 degrees is
    // just redicoulous, but there might always be a diff that needs to be
    // compensated for.
    void DataController::setTemperatureOffset(int offset)
    {
        // Just a little safety measure. Devices should never need more than
        // 10 degrees correction, right?
        if (offset < -10 || offset > 10) {
            _logger.log("sisis", "ERROR - Temperature offset ", offset,
                        "°C is too large (allowed range: -10°C up to +10°C)");
            return;
        }
        _logger.log("sis", "Set temperature offset to ", offset, "°C");
        _temperatureOffset = offset;
    }

    // ----------------------------------------------------------------------
    // Loop
    // ----------------------------------------------------------------------

    void DataController::loop()
    {
        if (isConfigured())
        {
            _temperatureMeasurements.loop();
            _humidityMeasurements.loop();
            _distanceMeasurements.loop();
        }
    }

    void DataController::clearHistory()
    {
        _temperatureMeasurements.clearHistory();
        _humidityMeasurements.clearHistory();
        _distanceMeasurements.clearHistory();
    }
}