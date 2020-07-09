#include "Data/DataController.h"
#include "Data/Measurements.h"

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
                                   _distanceMeasurements(DISTANCE_AVG_LOOKBACK) {}

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
        DoughUI::Instance()->log("DATA", "sS", "ERROR - Unhandled MQTT message, key = ", key);
    }
}

/**
 * Check if configuration has been taken care of. Some configuration is
 * required before measurements can be processed.
 */
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
        DoughUI::Instance()->log("DATA", "sisis",
                                 "ERROR - Container height ", height,
                                 "mm is less than the minimum measuring distance of ",
                                 HCSR04_MIN_MM, "mm");
        return;
    }
    if (height >= HCSR04_MAX_MM)
    {
        DoughUI::Instance()->log("DATA", "sisis",
                                 "ERROR - Container height ", height,
                                 "mm is more than the maximum measuring distance of ",
                                 HCSR04_MAX_MM, "mm");
        return;
    }
    DoughUI::Instance()->log("DATA", "sis", "Set container height to ", height, "mm");
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
        DoughUI *ui = DoughUI::Instance();
        _lastSample = now;
        DoughSensors *sensors = DoughSensors::Instance();

        // Quickly dip the LED to indicate that a measurement is started.
        // This is done synchroneously, because we suspend the timer interrupts
        // in the upcoming code.
        ui->led3.off();
        delay(50);
        ui->led3.on();

        // Suspend the UI timer interrupts, to not let these interfere
        // with the sensor measurements.
        ui->suspend();

        // Take a sample.
        switch (_sampleType)
        {
        case SAMPLE_TEMPERATURE:
            _temperatureMeasurements.add(sensors->readTemperature());
            _sampleType = SAMPLE_HUMIDITY;
            break;
        case SAMPLE_HUMIDITY:
            _humidityMeasurements.add(sensors->readHumidity());
            _sampleType = SAMPLE_DISTANCE;
            break;
        case SAMPLE_DISTANCE:
            _distanceMeasurements.add(sensors->readDistance());
            break;
        }

        ui->resume();

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
    static unsigned long lastSample = 0;
    if (lastSample == 0 || millis() - lastSample > PUBLISH_INTERVAL)
    {
        lastSample = millis();

        DoughUI *ui = DoughUI::Instance();
        DoughMQTT *mqtt = DoughMQTT::Instance();

        ui->log("DATA", "s", "Publish temperature");
        auto m = _temperatureMeasurements.getLast();
        if (m->ok)
        {
            mqtt->publish("temperature", m->value);
        }
        else
        {
            mqtt->publish("temperature", "null");
        }

        ui->log("DATA", "s", "Publish temperature average");
        m = _temperatureMeasurements.getAverage();
        if (m->ok)
        {
            mqtt->publish("temperature/average", m->value);
        }
        else
        {
            mqtt->publish("temperature/average", "null");
        }

        ui->log("DATA", "s", "Publish humidity");
        m = _humidityMeasurements.getLast();
        if (m->ok)
        {
            mqtt->publish("humidity", m->value);
        }
        else
        {
            mqtt->publish("humidity", "null");
        }

        m = _humidityMeasurements.getAverage();
        if (m->ok)
        {
            mqtt->publish("humidity/average", m->value);
        }
        else
        {
            mqtt->publish("humidity/average", "null");
        }

        m = _distanceMeasurements.getLast();
        if (m->ok)
        {
            mqtt->publish("distance", m->value);
        }
        else
        {
            mqtt->publish("distance", "null");
        }

        m = _distanceMeasurements.getAverage();
        if (m->ok)
        {
            mqtt->publish("distance/average", m->value);
        }
        else
        {
            mqtt->publish("distance/average", "null");
        }

        ui->led1.dip()->fast();
    }
}
