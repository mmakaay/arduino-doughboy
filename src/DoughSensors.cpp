#include "DoughSensors.h"
   
// ----------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------

DoughSensors* DoughSensors::_instance = nullptr;

/**
 * Fetch the DoughSensors singleton.
 */
DoughSensors* DoughSensors::Instance() {
    if (DoughSensors::_instance == nullptr) {
        DoughSensors::_instance = new DoughSensors();
    }
    return DoughSensors::_instance;
}

DoughSensors::DoughSensors() {
    _ui = DoughUI::Instance();
    _dht = new DHT(DHT11_DATA_PIN, DHT11);
    _hcsr04 = new HCSR04(HCSR04_TRIG_PIN, HCSR04_ECHO_PIN);
    temperature = 0;
    humidity = 0;
    distance = 0;
}

// ----------------------------------------------------------------------
// setup 
// ----------------------------------------------------------------------

void DoughSensors::setup() {
    _dht->begin();
    _hcsr04->begin();
}

// ----------------------------------------------------------------------
// loop
// ----------------------------------------------------------------------

void DoughSensors::readTemperature() {
    float t = _dht->readTemperature();
    if (isnan(t)) {
        _ui->log("SENSORS", "s", "ERROR - Temperature measurement failed");
        temperatureOk = false;
    } else {
        temperature = int(t);
        temperatureOk = true;
        _hcsr04->setTemperature(t);
    }
    _ui->log("SENSORS", "siss", "Temperature = ", temperature, "°C ", (temperatureOk ? "[OK]" : "[ERR]"));
}

void DoughSensors::readHumidity() {
    int h = _dht->readHumidity();
    if (h == 0) {
        _ui->log("SENSORS", "s", "ERROR - Humidity measurement failed");
        humidityOk = false;
    } else {
        humidity = h;
        humidityOk = true;
        _hcsr04->setHumidity(h);
    }
    _ui->log("SENSORS", "siss", "Humidity = ", humidity, "% ", (humidityOk ? "[OK]" : "[ERR]"));
}

void DoughSensors::readDistance() {
    int d = _hcsr04->readDistance();
    if (d == -1) {
        _ui->log("SENSORS", "s", "ERROR - Distance measurement failed");    
        distanceOk = false;
    } else {
        distanceOk = true;
        distance = d;        
    }
    _ui->log("SENSORS", "siss", "Distance = ", distance, "mm ", (distanceOk? "[OK]" : "[ERR]"));  
}
