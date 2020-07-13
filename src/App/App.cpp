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
                 mqtt(&wifi, mqttOnConnect, mqttOnMessage),
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
                     MINIMUM_PUBLISH_INTERVAL)
    {
    }

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

void mqttOnConnect(Dough::MQTT *mqtt)
{
    Serial.println("Subscribe to required incoming MQTT topics");
    mqtt->subscribe("container_height");
    mqtt->subscribe("temperature_offset");    
}

void mqttOnMessage(String &topic, String &payload)
{
    Serial.println("Handle message");
}