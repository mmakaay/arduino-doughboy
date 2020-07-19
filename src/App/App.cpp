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
                 distanceSensor(),
                 distanceController(
                     &distanceSensor, &sensorControllerPlugin,
                     DISTANCE_AVERAGE_STORAGE, DISTANCE_MEASURE_INTERVAL, MINIMUM_PUBLISH_INTERVAL),
                 temperatureController(
                     new TemperatureSensor(), &sensorControllerPlugin,
                     TEMPERATURE_AVERAGE_STORAGE, TEMPERATURE_MEASURE_INTERVAL, MINIMUM_PUBLISH_INTERVAL),
                 humidityController(
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
        temperatureController.setup();
        humidityController.setup();
        distanceController.setup();
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
                if (temperatureController.loop())
                    distanceSensor.setTemperature(temperatureController.getLast().value);
                if (humidityController.loop())
                    distanceSensor.setHumidity(humidityController.getLast().value);
                distanceController.loop();
            }
            else
            {
                state.startConfiguration();
            }
            break;
        case CALIBRATING:
            temperatureController.loop();
            humidityController.loop();
            distanceController.loop();

            delay(2000);
            state.pauseDevice();
            state.startMeasurements();
            break;
        case PAUSED:
            temperatureController.clearHistory();
            humidityController.clearHistory();
            distanceController.clearHistory();
            break;
        default:
            // NOOP
            break;
        }
    }
} // namespace Dough