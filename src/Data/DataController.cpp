#include "Data/DataController.h"

// ----------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------

DataController *DataController::_instance = nullptr;

/**
 * Fetch the DoughData singleton.
 */
DataController *DataController::Instance()
{
    if (DataController::_instance == nullptr)
    {
        DataController::_instance = new DataController();
    }
    return DataController::_instance;
}

DataController::DataController() : _temperatureMeasurements(TEMPERATURE_AVG_LOOKBACK),
                                   _humidityMeasurements(HUMIDITY_AVG_LOOKBACK),
                                   _distanceMeasurements(DISTANCE_AVG_LOOKBACK),
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
        _publish();
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
        Measurement m;
        switch (_sampleType)
        {
        case SAMPLE_TEMPERATURE:
            m = _sensors->readTemperature();
            _temperatureMeasurements.add(m);
            if (_temperatureLastPublished.ok && m.ok && _temperatureLastPublished.value != m.value)
            {
                if (m.value == _temperatureMeasurements.getAverage().value ||
                    abs(_temperatureLastPublished.value - m.value) > TEMPERATURE_SIGNIFICANT_CHANGE) {
                    _publishTemperature();
                }
            }
            else if (_temperatureLastPublished.ok == false && m.ok) {
                _publishTemperature();
            }
            _sampleType = SAMPLE_HUMIDITY;
            break;
        case SAMPLE_HUMIDITY:
            m = _sensors->readHumidity();
            _humidityMeasurements.add(m);
            if (_humidityLastPublished.ok && m.ok && _humidityLastPublished.value != m.value)
            {
                if (m.value == _humidityMeasurements.getAverage().value ||
                    abs(_humidityLastPublished.value - m.value) > HUMIDITY_SIGNIFICANT_CHANGE) {
                    _publishHumidity();
                }
            }
            else if (_humidityLastPublished.ok == false && m.ok) {
                _publishHumidity();
            }
            _sampleType = SAMPLE_DISTANCE;
            break;
        case SAMPLE_DISTANCE:
            m = _sensors->readDistance();
            _distanceMeasurements.add(m);
            if (_distanceLastPublished.ok && m.ok && _distanceLastPublished.value != m.value)
            {
                if (m.value == _distanceMeasurements.getAverage().value ||
                    abs(_distanceLastPublished.value - m.value) > DISTANCE_SIGNIFICANT_CHANGE) {
                    _publishDistance();
                }
            }
            else if (_distanceLastPublished.ok == false && m.ok) {
                _publishDistance();
            }
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

void DataController::_publish()
{
    if (_lastPublish == 0 || millis() - _lastPublish > PUBLISH_INTERVAL)
    {
        _lastPublish = millis();
        _publishTemperature();
        _publishHumidity();
        _publishDistance();
        _ui->led1.dip()->fast();
    }
}

void DataController::_publishTemperature()
{
    auto m = _temperatureMeasurements.getLast();
    _temperatureLastPublished.ok = m.ok;
    _temperatureLastPublished.value = m.value;
    _mqtt->publish("temperature", m);
    _mqtt->publish("temperature/average", _temperatureMeasurements.getAverage());
}

void DataController::_publishHumidity()
{
    auto m = _humidityMeasurements.getLast();
    _humidityLastPublished.ok = m.ok;
    _humidityLastPublished.value = m.value;
    _mqtt->publish("humidity", _humidityMeasurements.getLast());
    _mqtt->publish("humidity/average", _humidityMeasurements.getAverage());
}

void DataController::_publishDistance()
{
    auto m = _distanceMeasurements.getLast();
    _distanceLastPublished.ok = m.ok;
    _distanceLastPublished.value = m.value;
    _mqtt->publish("distance", _distanceMeasurements.getLast());
    _mqtt->publish("distance/average", _distanceMeasurements.getAverage());
}