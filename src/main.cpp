#include "main.h"

// TOOD: implement the calibration logic
// TODO: don't make sensors instances anymore, now they are handled by App as the only singleton
// TODO: see what more stuff can be moved to the UI code. Maybe state to UI state translation ought to be there as well

DoughBoyState state = CONFIGURING;
Dough::Logger logger("MAIN");

void setup()
{
    Dough::Logger::setup();
    logger.log("s", "Initializing device");
    auto app = Dough::App::Instance();
    app->setup();
    app->ui.onoffButton.onPress(handleOnoffButtonPress);
    app->ui.setupButton.onPress(handleSetupButtonPress);
    logger.log("s", "Initialization completed, starting device");
}

void loop()
{
    auto app = Dough::App::Instance();

    app->ui.processButtonEvents();

    if (!setupNetworkConnection())
    {
        return;
    }
    
    app->mqtt.procesIncomingsMessages();
    
    if (state == CONFIGURING)
    {
        if (app->config.isOk())
        {
            setStateToMeasuring();
        }
    }
    else if (state == MEASURING && !app->config.isOk())
    {
        setStateToConfiguring();
    }
    else if (state == MEASURING)
    {
        app->measure();
    }
    else if (state == CALIBRATING)
    {
        delay(3000);
        setStateToPaused();
    }
    else if (state == PAUSED)
    {
        app->clearHistory();
    }
}

// Check if the device is connected to the WiFi network and the MQTT broker.
// If not, then try to setup the connection.
// Returns true if the connection was established, false otherwise.
bool setupNetworkConnection()
{
    static auto connectionState = CONNECTING_WIFI;

    auto app = Dough::App::Instance();

    if (!app->wifi.isConnected())
    {
        if (connectionState == CONNECTED)
        {
            logger.log("s", "ERROR - Connection to WiFi network lost! Reconnecting ...");
        }
        else
        {
            logger.log("s", "Connecting to the WiFi network ...");
        }
        connectionState = CONNECTING_WIFI;
        app->ui.notifyConnectingToWifi();
        app->wifi.connect();
    }
    if (app->wifi.isConnected() && !app->mqtt.isConnected())
    {
        if (connectionState == CONNECTED)
        {
            logger.log("s", "ERROR - Connection to the MQTT broker lost! Reconnecting ...");
        }
        else
        {
            logger.log("s", "Connecting to the MQTT broker ...");
        }
        connectionState = CONNECTING_MQTT;
        app->ui.notifyConnectingToMQTT();
        app->mqtt.connect();
        delay(1000); // purely costmetic, to make the faster LED blinking noticable
    }
    if (app->wifi.isConnected() && app->mqtt.isConnected())
    {
        if (connectionState != CONNECTED)
        {
            logger.log("s", "Connection to MQTT broker established");
            app->ui.notifyConnected();
            app->ui.clearButtonEvents();
            connectionState = CONNECTED;
            setStateToConfiguring();
        }
    }

    return connectionState == CONNECTED;
}

void handleOnoffButtonPress()
{
    if (state == MEASURING)
    {
        setStateToPaused();
    }
    else if (state == PAUSED)
    {
        setStateToMeasuring();
    }
}

void handleSetupButtonPress()
{
    setStateToCalibrating();
}

void setStateToConfiguring()
{
    auto app = Dough::App::Instance();
    logger.log("s", "Waiting for configuration ...");
    state = CONFIGURING;
    app->ui.notifyWaitingForConfiguration();
    app->mqtt.publish("state", "configuring");
}

void setStateToMeasuring()
{
    auto app = Dough::App::Instance();
    logger.log("s", "Starting measurements");
    state = MEASURING;
    app->ui.notifyMeasurementsActive();
    app->mqtt.publish("state", "measuring");
}

void setStateToPaused()
{
    auto app = Dough::App::Instance();
    logger.log("s", "Pausing measurements");
    state = PAUSED;
    app->ui.notifyMeasurementsPaused();
    app->mqtt.publish("state", "paused");
}

void setStateToCalibrating()
{
    auto app = Dough::App::Instance();
    logger.log("s", "Requested device calibration");
    state = CALIBRATING;
    app->ui.notifyCalibrating();
    app->mqtt.publish("state", "calibrating");
}
