#ifndef DOUGH_SENSORS_H    
#define DOUGH_SENSORS_H

#include <DHT.h>
#include "HCSR04.h"
#include "DoughUI.h"
#include "config.h"    

class DoughSensors {
    public:
        static DoughSensors* Instance();
        void setup();
        void readAll();
        void readTemperature();
        int temperature = 0;
        bool temperatureOk = false;
        void readHumidity();
        int humidity = 0;
        bool humidityOk = false;
        void readDistance();
        int distance = 0;
        bool distanceOk = false;

    private:
        DoughSensors();
        static DoughSensors* _instance;
        DoughUI *_ui;
        DHT* _dht;
        HCSR04* _hcsr04;
};

#endif 
