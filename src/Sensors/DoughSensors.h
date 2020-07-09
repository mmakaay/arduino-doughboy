#ifndef DOUGH_SENSORS_H    
#define DOUGH_SENSORS_H

#include <DHT.h>
#include "Sensors/HCSR04.h"
#include "UI/DoughUI.h"
#include "Data/Measurement.h"
#include "config.h"    

class DoughSensors {
    public:
        static DoughSensors* Instance();
        void setup();
        Measurement* readTemperature();
        Measurement* readHumidity();
        Measurement* readDistance();

    private:
        DoughSensors();
        static DoughSensors* _instance;
        DoughUI *_ui;
        DHT* _dht;
        HCSR04* _hcsr04;
};

#endif 
