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
                 statePlugin(&ui, &mqtt),
                 state(&statePlugin),
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
                 _logger("APP")
    {
        ui.onoffButton.onPress(handleOnoffButtonPress);
        ui.setupButton.onPress(handleSetupButtonPress);
    }

    void App::setup()
    {
        Dough::Logger::setup();
        state.setup();
        ui.setup();
        wifi.setup();
        mqtt.setup();
        temperatureSensor.setup();
        humiditySensor.setup();
        distanceSensor.setup();
    }

    void App::loop()
    {
        if (!wifi.isConnected())
        {
            state.setWiFiConnected(false);
            state.setWiFiConnected(wifi.connect());
            return;
        }
        if (!mqtt.isConnected())
        {
            state.setMQTTConnected(false);
            state.setMQTTConnected(mqtt.connect());
            return;
        }

        ui.processButtonEvents();
        mqtt.procesIncomingsMessages();

        switch (state.get())
        {
        case CONFIGURING:
            if (config.isOk())
            {
                state.startMeasurements();
            }
            break;
        case MEASURING:
            if (config.isOk())
            {
                temperatureSensor.loop();
                humiditySensor.loop();
                distanceSensor.loop();
            }
            else
            {
                state.startConfiguration();
            }
            break;
        case CALIBRATING:
            delay(2000);
            state.pauseMeasurements();
            state.startMeasurements();
            break;
        case PAUSED:
            temperatureSensor.clearHistory();
            humiditySensor.clearHistory();
            distanceSensor.clearHistory();
            break;
        default:
            // NOOP
            break;
        }
    }
} // namespace Dough