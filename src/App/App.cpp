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
                     &sensorControllerPlugin,
                     &mqtt, "distance", distanceSensorX,
                     DISTANCE_AVERAGE_STORAGE,
                     DISTANCE_MEASURE_INTERVAL,
                     MINIMUM_PUBLISH_INTERVAL),
                 temperatureSensor(
                     &sensorControllerPlugin,
                     &mqtt, "temperature", temperatureSensorX,
                     TEMPERATURE_AVERAGE_STORAGE,
                     TEMPERATURE_MEASURE_INTERVAL,
                     MINIMUM_PUBLISH_INTERVAL),
                 humiditySensor(
                     &sensorControllerPlugin,
                     &mqtt, "humidity", humiditySensorX,
                     HUMIDITY_AVERAGE_STORAGE,
                     HUMIDITY_MEASURE_INTERVAL,
                     MINIMUM_PUBLISH_INTERVAL),
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

        // Get measurements from the sensors. Suspend the user interface
        // interrupts in the meanwhile, to not disturb the timing-sensitive
        // sensor readings.
        ui.suspend();
        temperatureSensor.loop();
        humiditySensor.loop();
        distanceSensor.loop();
        ui.resume();
    }

    void App::clearHistory()
    {
        temperatureSensor.clearHistory();
        humiditySensor.clearHistory();
        distanceSensor.clearHistory();
    }
} // namespace Dough