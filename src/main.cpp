#include "main.h"

// TOOD: implement the calibration logic
// TODO: see what more stuff can be moved to the UI code. Maybe state to UI state translation ought to be there as well

DoughBoyState state = CONFIGURING;
Dough::Logger logger("MAIN");

void setup()
{
    Dough::Logger::setup();
    logger.log("s", "Initializing device");
    Dough::App::Instance()->setup();
    auto ui = Dough::UI::Instance();
    ui->setup();
    ui->onoffButton.onPress(handleOnoffButtonPress);
    ui->setupButton.onPress(handleSetupButtonPress);
    logger.log("s", "Initialization completed, starting device");
}

void loop()
{
    auto app = Dough::App::Instance();
    auto ui = Dough::UI::Instance();

    ui->processButtonEvents();

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
    auto ui = Dough::UI::Instance();

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
        ui->led1.blink()->slow();
        ui->led2.off();
        ui->led3.off();
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
        ui->led1.blink()->fast();
        ui->led2.off();
        ui->led3.off();
        app->mqtt.connect();
        delay(1000); // purely costmetic, to make the faster LED blinking noticable
    }
    if (app->wifi.isConnected() && app->mqtt.isConnected())
    {
        if (connectionState != CONNECTED)
        {
            logger.log("s", "Connection to MQTT broker established");
            ui->led1.on();
            ui->led2.off();
            ui->led3.off();
            ui->clearButtonEvents();
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
    auto ui = Dough::UI::Instance();
    logger.log("s", "Waiting for configuration ...");
    state = CONFIGURING;
    ui->led1.on();
    ui->led2.blink()->fast();
    ui->led3.off();
    Dough::App::Instance()->mqtt.publish("state", "configuring");
}

void setStateToMeasuring()
{
    auto ui = Dough::UI::Instance();
    logger.log("s", "Starting measurements");
    state = MEASURING;
    ui->led1.on();
    ui->led2.on();
    ui->led3.on();
    Dough::App::Instance()->mqtt.publish("state", "measuring");
}

void setStateToPaused()
{
    auto ui = Dough::UI::Instance();
    logger.log("s", "Pausing measurements");
    state = PAUSED;
    ui->led1.on();
    ui->led2.on();
    ui->led3.pulse();
    Dough::App::Instance()->mqtt.publish("state", "paused");
}

void setStateToCalibrating()
{
    auto ui = Dough::UI::Instance();
    logger.log("s", "Requested device calibration");
    state = CALIBRATING;
    ui->led1.on();
    ui->led2.blink()->slow();
    ui->led3.off();
    Dough::App::Instance()->mqtt.publish("state", "calibrating");
}
