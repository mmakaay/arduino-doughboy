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
        if (!_setupNetworking())
        {
            return;
        }

        ui.processButtonEvents();
        mqtt.procesIncomingsMessages();

        switch (state.get())
        {
        case CONFIGURING:
            _doConfigure();
            break;
        case MEASURING:
            _doMeasure();
            break;
        case CALIBRATING:
            _doCalibrate();
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

    bool App::_setupNetworking()
    {
        if (!wifi.isConnected())
        {
            state.setWiFiConnected(false);
            state.setWiFiConnected(wifi.connect());
            return false;
        }
        if (!mqtt.isConnected())
        {
            state.setMQTTConnected(false);
            state.setMQTTConnected(mqtt.connect());
            return false;
        }
        return true;
    }

    void App::_doConfigure()
    {
        if (config.isOk())
        {
            state.startMeasurements();
        }
    }

    void App::_doMeasure()
    {
        if (config.isOk())
        {
            if (temperatureController.loop())
                distanceSensor.setTemperature(temperatureController.getLast().value);
            if (humidityController.loop())
                distanceSensor.setHumidity(humidityController.getLast().value);
            if (distanceController.loop())
                mqtt.publish("height", config.getContainerHeight() - distanceController.getLast().value);
        }
        else
        {
            state.startConfiguration();
        }
    }

    void App::_doCalibrate()
    {
        int count = 0;
        int runningTotal = 0;
        unsigned int firstValue = 0;
        unsigned int precision = distanceController.getPrecision();

        for (;;)
        {
            // Read a sensor value. When this fails, then restart the calibration.
            auto m = distanceController.readSensor();
            if (!m.ok)
            {
                _logger.log("s", "Sensor reading failed, restarting calibration");
                return;
            }
            _logger.log("sisis", "Calibration reading ", count, ": ", m.value, "mm");

            count++;
            runningTotal += m.value;

            // If this is the first value that is read, then use that one to compare
            // the upcoming measurements against.
            if (count == 1)
            {
                firstValue = m.value;
            }
            // Otherwise, check if the new value is within the precision range of the
            // distance sensor.
            else if (abs(firstValue - m.value) > precision)
            {
                _logger.log("s", "New reading exceeds sensor precision, restarting calibration");
                return;
            }

            // After reading 10 values in a row that are within the sensor precision,
            // then we have a winner.
            if (count == 10)
            {
                break;
            }
        }

        _logger.log("sis", "Calibration completed, container height: ", firstValue, "mm");
        unsigned int container_height = int(runningTotal / 10);
        config.setContainerHeight(container_height);
        mqtt.publish("container_height", container_height, true);

        state.pauseDevice();
        state.startMeasurements();
    }
} // namespace Dough