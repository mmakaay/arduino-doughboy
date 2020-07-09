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

Measurement* DoughSensors::readTemperature() {
    float t = _dht->readTemperature();
    if (isnan(t)) {
        _ui->log("SENSORS", "s", "ERROR - Temperature measurement failed");
        return Measurement::Failed();
    } else {
        _hcsr04->setTemperature(int(t));
        auto m = new Measurement(true, int(t));
        _ui->log("SENSORS", "sisi", "Temperature = ", int(t), "°C ", m->value);
        return m;
    }
}

Measurement* DoughSensors::readHumidity() {
    int h = _dht->readHumidity();
    if (h == 0) {
        _ui->log("SENSORS", "s", "ERROR - Humidity measurement failed");
        return Measurement::Failed();
    } else {
        _hcsr04->setHumidity(h);
        _ui->log("SENSORS", "sis", "Humidity = ", h, "%");
        return Measurement::Ok(h);
    }
}

Measurement* DoughSensors::readDistance() {
    int d = _hcsr04->readDistance();
    if (d == -1) {
        _ui->log("SENSORS", "s", "ERROR - Distance measurement failed");    
        return Measurement::Failed();
    } else {
        _ui->log("SENSORS", "sis", "Distance = ", d, "mm"); 
         return Measurement::Ok(d);
    }     
}
