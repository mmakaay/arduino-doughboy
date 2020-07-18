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
                 sensorControllerPlugin(&mqtt, &ui),
                 distanceSensor(
                     new DistanceSensor(), &sensorControllerPlugin,
                     DISTANCE_AVERAGE_STORAGE, DISTANCE_MEASURE_INTERVAL, MINIMUM_PUBLISH_INTERVAL),
                 temperatureSensor(
                     new TemperatureSensor(), &sensorControllerPlugin,
                     TEMPERATURE_AVERAGE_STORAGE, TEMPERATURE_MEASURE_INTERVAL, MINIMUM_PUBLISH_INTERVAL),
                 humiditySensor(
                     new HumiditySensor(), &sensorControllerPlugin,
                     HUMIDITY_AVERAGE_STORAGE, HUMIDITY_MEASURE_INTERVAL, MINIMUM_PUBLISH_INTERVAL),
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
        if (!config.isOk())
        {
            return;
        }

        // Get measurements from the sensors.
        temperatureSensor.loop();
        humiditySensor.loop();
        distanceSensor.loop();
    }

    void App::clearHistory()
    {
        temperatureSensor.clearHistory();
        humiditySensor.clearHistory();
        distanceSensor.clearHistory();
    }
} // namespace Dough