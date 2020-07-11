#include "Data/DataController.h"

// ----------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------

DataController *DataController::_instance = nullptr;

DataController *DataController::Instance()
{
    if (DataController::_instance == nullptr)
    {
        DataController::_instance = new DataController();
    }
    return DataController::_instance;
}

DataController::DataController() : _temperatureMeasurements(
                                       "temperature",
                                       readTemperature,
                                       TEMPERATURE_AVG_LOOKBACK,
                                       TEMPERATURE_SIGNIFICANT_CHANGE,
                                       PUBLISH_INTERVAL),
                                   _humidityMeasurements(
                                       "humidity",
                                       readHumidity,
                                       HUMIDITY_AVG_LOOKBACK,
                                       HUMIDITY_SIGNIFICANT_CHANGE,
                                       PUBLISH_INTERVAL),
                                   _distanceMeasurements(
                                       "distance",
                                       readDistance,
                                       DISTANCE_AVG_LOOKBACK,
                                       DISTANCE_SIGNIFICANT_CHANGE,
                                       PUBLISH_INTERVAL),
                                   _logger("DATA")
{
    _ui = DoughUI::Instance();
    _sensors = DoughSensors::Instance();
    _mqtt = DoughMQTT::Instance();
}

// ----------------------------------------------------------------------
// Setup
// ----------------------------------------------------------------------

void DataController::setup()
{
    _containerHeight = 0.00;
    _containerHeightSet = false;

    DoughMQTT *mqtt = DoughMQTT::Instance();
    mqtt->onConnect(DataController::handleMqttConnect);
    mqtt->onMessage(DataController::handleMqttMessage);
}

void DataController::handleMqttConnect(DoughMQTT *mqtt)
{
    mqtt->subscribe("container_height");
}

void DataController::handleMqttMessage(String &key, String &payload)
{
    if (key.equals("container_height"))
    {
        DataController::Instance()->setContainerHeight(payload.toInt());
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

/**
 * Set the container height in mm. This is the distance between the sensor
 * and the bottom of the container. It is used to determine the height of
 * the starter or dough by subtracting the distance measurement from it.
 */
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

// ----------------------------------------------------------------------
// Loop
// ----------------------------------------------------------------------

void DataController::loop()
{
    if (isConfigured())
    {
        _sample();
    }
}

void DataController::clearHistory()
{
    _temperatureMeasurements.clearHistory();
    _humidityMeasurements.clearHistory();
    _distanceMeasurements.clearHistory();
    _sampleType = SAMPLE_TEMPERATURE;
    _sampleCounter = 0;
}

void DataController::_sample()
{
    auto now = millis();
    auto delta = now - _lastSample;
    auto tick = _lastSample == 0 || delta >= SAMPLE_INTERVAL;

    if (tick)
    {
        _lastSample = now;

        // Quickly dip the LED to indicate that a measurement is started.
        // This is done synchroneously, because we suspend the timer interrupts
        // in the upcoming code.
        _ui->led3.off();
        delay(50);
        _ui->led3.on();

        // Suspend the UI timer interrupts, to not let these interfere
        // with the sensor measurements.
        _ui->suspend();

        // Take a sample.
        switch (_sampleType)
        {
        case SAMPLE_TEMPERATURE:
            _temperatureMeasurements.process();
            _sampleType = SAMPLE_HUMIDITY;
            break;
        case SAMPLE_HUMIDITY:
            _humidityMeasurements.process();
            _sampleType = SAMPLE_DISTANCE;
            break;
        case SAMPLE_DISTANCE:
            _distanceMeasurements.process();
            break;
        }

        _ui->resume();

        _sampleCounter++;
        if (_sampleCounter == SAMPLE_CYCLE_LENGTH)
        {
            _sampleCounter = 0;
            _sampleType = SAMPLE_TEMPERATURE;
        }
    }
}