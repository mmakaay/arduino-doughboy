#include "App/App.h"

namespace Dough
{
    App *App::Instance()
    {
        static App *_instance = new App();
        return _instance;
    }

    App::App() : config(),
                 wifi(),
                 mqtt(
                     &wifi,
                     mqttOnConnectCallback,
                     mqttOnMessageCallback),
                 temperatureSensor(
                     &mqtt,
                     "temperature",
                     TemperatureSensor::Instance(),
                     TEMPERATURE_AVERAGE_STORAGE,
                     TEMPERATURE_MEASURE_INTERVAL,
                     MINIMUM_PUBLISH_INTERVAL),
                 humiditySensor(
                     &mqtt,
                     "humidity",
                     HumiditySensor::Instance(),
                     HUMIDITY_AVERAGE_STORAGE,
                     HUMIDITY_MEASURE_INTERVAL,
                     MINIMUM_PUBLISH_INTERVAL),
                 distanceSensor(
                     &mqtt,
                     "distance",
                     DistanceSensor::Instance(),
                     DISTANCE_AVERAGE_STORAGE,
                     DISTANCE_MEASURE_INTERVAL,
                     MINIMUM_PUBLISH_INTERVAL),
                 _logger("APP") {}

    void App::setup()
    {
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
            temperatureSensor.loop();
            humiditySensor.loop();
            distanceSensor.loop();
        }
    }

    void App::clearHistory()
    {
        temperatureSensor.clearHistory();
        humiditySensor.clearHistory();
        distanceSensor.clearHistory();
    }
} // namespace Dough

Dough::Logger callbackLogger("CALLBACK");

void mqttOnConnectCallback(Dough::MQTT *mqtt)
{
    callbackLogger.log("s", "MQTT connection establish, subscribing to topics");
    mqtt->subscribe("container_height");
    mqtt->subscribe("temperature_offset");
}

void mqttOnMessageCallback(String &topic, String &payload)
{
    callbackLogger.log("s", "MQTT message received");
}