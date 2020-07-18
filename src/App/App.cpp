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
                 state(&ui),
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

    void App::loop()
    {
        if (_setupNetworkConnection())
        {
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
            }
        }
    }

    // Check if the device is connected to the WiFi network and the MQTT broker.
    // If not, then try to setup the connection.
    // Returns true if the connection was established, false otherwise.
    bool App::_setupNetworkConnection()
    {
        static bool connected = false;

        if (!wifi.isConnected())
        {
            state.setWiFiConnected(false);
            if (connected)
            {
                _logger.log("s", "ERROR - Connection to WiFi network lost! Reconnecting ...");
            }
            else
            {
                _logger.log("s", "Connecting to the WiFi network ...");
            }
            connected = false;
            ui.notifyConnectingToWifi();
            state.setWiFiConnected(wifi.connect());
        }
        if (wifi.isConnected() && !mqtt.isConnected())
        {
            state.setWiFiConnected(true);
            state.setMQTTConnected(false);
            if (connected)
            {
                _logger.log("s", "ERROR - Connection to the MQTT broker lost! Reconnecting ...");
            }
            else
            {
                _logger.log("s", "Connecting to the MQTT broker ...");
            }
            connected = false;
            ui.notifyConnectingToMQTT();
            state.setMQTTConnected(mqtt.connect());
        }
        if (wifi.isConnected() && mqtt.isConnected())
        {
            state.setWiFiConnected(true);
            state.setMQTTConnected(true);
            if (!connected)
            {
                _logger.log("s", "Connection to MQTT broker established");
                ui.notifyConnected();
                ui.clearButtonEvents();
                connected = true;
            }
        }

        return connected;
    }

    void App::clearHistory()
    {
        temperatureSensor.clearHistory();
        humiditySensor.clearHistory();
        distanceSensor.clearHistory();
    }
} // namespace Dough