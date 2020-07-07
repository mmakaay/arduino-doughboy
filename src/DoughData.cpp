#include "DoughData.h"

// ----------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------

DoughData* DoughData::_instance = nullptr;

/**
 * Fetch the DoughData singleton.
 */
DoughData* DoughData::Instance() {
    if (DoughData::_instance == nullptr) {
        DoughData::_instance = new DoughData();
    }
    return DoughData::_instance;
}

DoughData::DoughData() : _temperatureMeasurements(TEMPERATURE_AVG_LOOKBACK),
                         _humidityMeasurements(HUMIDITY_AVG_LOOKBACK),
                         _distanceMeasurements(DISTANCE_AVG_LOOKBACK) {}

// ----------------------------------------------------------------------
// Measurements storage
// ----------------------------------------------------------------------

DoughDataMeasurements::DoughDataMeasurements(int avgLookback) {
    _storageSize = avgLookback;
    _storage = new DoughDataMeasurement[avgLookback];
    for (int i = 0; i < avgLookback; i++) {
        _storage[i] = DoughDataMeasurement();
    }
}

void DoughDataMeasurements::registerValue(int value) {
    auto measurement = _next();
    _averageCount++;
    _averageSum += value;
    measurement->ok = true;
    measurement->value = value;
}

void DoughDataMeasurements::registerFailed() {
    auto measurement = _next();
    measurement->ok = false;
    measurement->value = 0;
}

DoughDataMeasurement* DoughDataMeasurements::_next() {
    _index++;
    if (_index == _storageSize) {
        _index = 0;
    }
    if (_storage[_index].ok) {
        _averageSum -= _storage[_index].value;
        _averageCount--;
    }
    return &(_storage[_index]);
}

DoughDataMeasurement DoughDataMeasurements::getLast() {
    return _storage[_index];
}

DoughDataMeasurement DoughDataMeasurements::getAverage() {
    DoughDataMeasurement result;
    if (_averageCount > 0) {
        result.ok = true;
        result.value = round(_averageSum / _averageCount);
    }
    return result;
}

void DoughDataMeasurements::clearHistory() {
    _averageCount = 0;
    _averageSum = 0;
    for (unsigned int i = 0; i < _storageSize; i++) {
        _storage[i].ok = false;
        _storage[i].value = 0;
    }
}

// ----------------------------------------------------------------------
// Setup
// ----------------------------------------------------------------------

void DoughData::setup() {
    _containerHeight = 0.00;
    _containerHeightSet = false;

    DoughMQTT *mqtt = DoughMQTT::Instance();
    mqtt->onConnect(DoughData::handleMqttConnect);
    mqtt->onMessage(DoughData::handleMqttMessage);    
}

void DoughData::handleMqttConnect(DoughMQTT* mqtt) {
    mqtt->subscribe("container_height");  
}

void DoughData::handleMqttMessage(String &key, String &payload) {
    if (key.equals("container_height")) {
        DoughData::Instance()->setContainerHeight(payload.toInt());
    } else {
        DoughUI::Instance()->log("DATA", "sS", "ERROR - Unhandled MQTT message, key = ", key);
    }
}

/**
 * Check if configuration has been taken care of. Some configuration is
 * required before measurements can be processed.
 */
bool DoughData::isConfigured() {
    return _containerHeightSet;
}

/**
 * Set the container height in mm. This is the distance between the sensor
 * and the bottom of the container. It is used to determine the height of
 * the starter or dough by subtracting the distance measurement from it.
 */
void DoughData::setContainerHeight(int height) {
    _containerHeightSet = false;
    if (height <= HCSR04_MIN_MM) {
        DoughUI::Instance()->log("DATA", "sisis",
            "ERROR - Container height ", height,
            "mm is less than the minimum measuring distance of ",
            HCSR04_MIN_MM, "mm");
        return;
    }
    if (height >= HCSR04_MAX_MM) {
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

void DoughData::loop() {
    if (isConfigured()) {
        _sample();
        _publish();
    }
}

void DoughData::clearHistory() {
    _temperatureMeasurements.clearHistory();
    _humidityMeasurements.clearHistory();
    _distanceMeasurements.clearHistory();
    _sampleType = SAMPLE_TEMPERATURE;
    _sampleCounter = 0;
}

void DoughData::_sample() {
    auto now = millis();
    auto delta = now - _lastSample;
    auto tick = _lastSample == 0 || delta >= SAMPLE_INTERVAL;

    if (tick) {
        _lastSample = now;        
        DoughUI* ui = DoughUI::Instance();
        DoughSensors* sensors = DoughSensors::Instance();

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
        switch (_sampleType) {
            case SAMPLE_TEMPERATURE:
                sensors->readTemperature();
                if (sensors->temperatureOk) {
                    _temperatureMeasurements.registerValue(sensors->temperature);
                } else {
                    _temperatureMeasurements.registerFailed();
                }
                _sampleType = SAMPLE_HUMIDITY;
                break;
            case SAMPLE_HUMIDITY:
                sensors->readHumidity();
                if (sensors->humidityOk) {
                    _humidityMeasurements.registerValue(sensors->humidity);
                } else {
                    _humidityMeasurements.registerFailed();
                }
                _sampleType = SAMPLE_DISTANCE;
                break;
            case SAMPLE_DISTANCE:
                sensors->readDistance();
                if (sensors->distanceOk) {
                    _distanceMeasurements.registerValue(sensors->distance);
                } else {
                    _distanceMeasurements.registerFailed();
                }
                break;
        }
        
        ui->resume();

        _sampleCounter++;
        if (_sampleCounter == SAMPLE_CYCLE_LENGTH) {
            _sampleCounter = 0;
            _sampleType = SAMPLE_TEMPERATURE;
        }
    }
}

void DoughData::_publish() {
    static unsigned long lastSample = 0;    
    if (lastSample == 0 || millis() - lastSample > PUBLISH_INTERVAL) {
        lastSample = millis();
        
        DoughUI* ui = DoughUI::Instance();
        DoughMQTT* mqtt = DoughMQTT::Instance();
        
        auto m = _temperatureMeasurements.getLast();
        if (m.ok) {
            mqtt->publish("temperature", m.value);
        } else {
            mqtt->publish("temperature", "null");              
        }

        m = _temperatureMeasurements.getAverage();
        if (m.ok) {
            mqtt->publish("temperature/average", m.value);
        } else {
            mqtt->publish("temperature/average", "null");              
        }
        
        m = _humidityMeasurements.getLast();
        if (m.ok) {
            mqtt->publish("humidity", m.value);
        } else {
            mqtt->publish("humidity", "null");              
        }

        m = _humidityMeasurements.getAverage();
        if (m.ok) {
            mqtt->publish("humidity/average", m.value);
        } else {
            mqtt->publish("humidity/average", "null");              
        }
        
        m = _distanceMeasurements.getLast();
        if (m.ok) {
            mqtt->publish("distance", m.value);
        } else {
            mqtt->publish("distance", "null");              
        }

        m = _distanceMeasurements.getAverage();
        if (m.ok) {
            mqtt->publish("distance/average", m.value);
        } else {
            mqtt->publish("distance/average", "null");              
        }
        
        ui->led1.dip()->fast();        
    }
}
