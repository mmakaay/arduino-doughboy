#include "App/App.h"

namespace Dough
{
    App *App::Instance()
    {
        static App *_instance = new App();
        return _instance;
    }

    App::App() : config(),
                 ui(),
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
                     TEMPERATURE_MEASURE_INTERVAL, sensorOnMeasureCallback,
                     MINIMUM_PUBLISH_INTERVAL, sensorOnPublishCallback),
                 humiditySensor(
                     &mqtt,
                     "humidity",
                     HumiditySensor::Instance(),
                     HUMIDITY_AVERAGE_STORAGE,
                     HUMIDITY_MEASURE_INTERVAL, sensorOnMeasureCallback,
                     MINIMUM_PUBLISH_INTERVAL, sensorOnPublishCallback),
                 distanceSensor(
                     &mqtt,
                     "distance",
                     DistanceSensor::Instance(),
                     DISTANCE_AVERAGE_STORAGE,
                     DISTANCE_MEASURE_INTERVAL, sensorOnMeasureCallback,
                     MINIMUM_PUBLISH_INTERVAL, sensorOnPublishCallback),
                 _logger("APP") {}

    void App::setup()
    {
        ui.setup();
        ui.onoffButton.onInterrupt(::onoffButtonInterruptCallback);
        ui.setupButton.onInterrupt(::setupButtonInterruptCallback);

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

Dough::Logger callbackLogger("CALLBACK");

void mqttOnConnectCallback(Dough::MQTT *mqtt)
{
    callbackLogger.log("s", "MQTT connection establish, subscribing to topics");
    mqtt->subscribe("container_height");
    mqtt->subscribe("temperature_offset");
}

void mqttOnMessageCallback(String &topic, String &payload)
{
    callbackLogger.log("ssss", "MQTT message received: ", topic.c_str(), " = ", payload.c_str());

    if (topic.endsWith("/container_height"))
    {
        Dough::App::Instance()->config.setContainerHeight(payload.toInt());
    }
    else if (topic.endsWith("/temperature_offset"))
    {
        Dough::App::Instance()->config.setTemperatureOffset(payload.toInt());
    }
    else
    {
        callbackLogger.log("ss", "ERROR - Unhandled MQTT message, topic = ", topic.c_str());
    }
}

void onoffButtonInterruptCallback()
{
    Dough::App::Instance()->ui.onoffButton.handleButtonState();
}

void setupButtonInterruptCallback()
{
    Dough::App::Instance()->ui.setupButton.handleButtonState();
}

void sensorOnMeasureCallback()
{
    Dough::App::Instance()->ui.notifySensorActivity();
}

void sensorOnPublishCallback()
{
    Dough::App::Instance()->ui.notifyNetworkActivity();
}

// This callback is called when the TC4 timer from the UI code hits an overflow
// interrupt. It is defined outside the Dough namespace, because TC4_Handler is
// a hard-coded root namespace function name.
void TC4_Handler()
{
    Dough::App::Instance()->ui.updateLEDs();
    REG_TC4_INTFLAG = TC_INTFLAG_OVF; // Clear the OVF interrupt flag.
}