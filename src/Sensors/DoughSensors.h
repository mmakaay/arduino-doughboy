#ifndef DOUGH_SENSORS_H    
#define DOUGH_SENSORS_H

#include <DHT.h>
#include "Sensors/HCSR04.h"
#include "UI/DoughLogger.h"
#include "Data/Measurement.h"
#include "config.h"    

/**
 * This class provides access to the sensors in the device.
 */
class DoughSensors {
    public:
        static DoughSensors* Instance();
        void setup();
        Measurement readTemperature();
        Measurement readHumidity();
        Measurement readDistance();

    private:
        DoughSensors();
        static DoughSensors* _instance;
        DoughLogger _logger;
        DHT* _dht;
        HCSR04* _hcsr04;
};

#endif 
