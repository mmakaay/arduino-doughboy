#include "App/App.h"

namespace Dough
{
    App *App::Instance()
    {
        static App *_instance = new App();
        return _instance;
    }

    App::App() : config(),
                 ui(onoffButtonInterruptCallback, setupButtonInterruptCallback),
                 wifi(),
                 mqtt(&wifi, mqttOnConnectCallback, mqttOnMessageCallback),
                 temperatureSensor(
                     &mqtt, "temperature", TemperatureSensor::Instance(),
                     TEMPERATURE_AVERAGE_STORAGE,
                     TEMPERATURE_MEASURE_INTERVAL, sensorOnMeasureCallback,
                     MINIMUM_PUBLISH_INTERVAL, sensorOnPublishCallback),
                 humiditySensor(
                     &mqtt, "humidity", HumiditySensor::Instance(),
                     HUMIDITY_AVERAGE_STORAGE,
                     HUMIDITY_MEASURE_INTERVAL, sensorOnMeasureCallback,
                     MINIMUM_PUBLISH_INTERVAL, sensorOnPublishCallback),
                 distanceSensor(
                     &mqtt, "distance", DistanceSensor::Instance(),
                     DISTANCE_AVERAGE_STORAGE,
                     DISTANCE_MEASURE_INTERVAL, sensorOnMeasureCallback,
                     MINIMUM_PUBLISH_INTERVAL, sensorOnPublishCallback),
                 _logger("APP") {}

    void App::setup()
    {
        ui.setup();
        wifi.setup();
        mqtt.setup();
        temperatureSensor.setup();
        humiditySensor.setup();
        distanceSensor.setup();
    }

    void App::measure()
    {
        if (config.isOk())
        {
            // Get measurements from the sensors. Suspend the user interface
            // interrupts in the meanwhile, to not disturb the timing-sensitive
            // sensor readings.
            ui.suspend();
            temperatureSensor.loop();
            humiditySensor.loop();
            distanceSensor.loop();
            ui.resume();
        }
    }

    void App::clearHistory()
    {
        temperatureSensor.clearHistory();
        humiditySensor.clearHistory();
        distanceSensor.clearHistory();
    }
} // namespace Dough