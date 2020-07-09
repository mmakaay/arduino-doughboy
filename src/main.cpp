#include "main.h"

// TOOD: implement the calibration logic
// TODO: use different timings for temperature, humidity and distance measurements. Temp/Humidity together takes about 500ms, which slows down stuff.
// TODO: make the measuring more loop-y, giving back control to the main loop more often for better UI responsiveness
// TODO: see what more stuff can be moved to the UI code. Maybe state to UI state translation ought to be there as well
// TODO: use longer term averages for data

DoughBoyState state = CONFIGURING;

void setup()
{
    DoughSensors::Instance()->setup();
    DoughWiFi::Instance()->setup();
    DoughMQTT::Instance()->setup();
    DataController::Instance()->setup();
    auto ui = DoughUI::Instance();
    ui->setup();
    ui->onoffButton.onPress(handleOnoffButtonPress);
    ui->setupButton.onPress(handleSetupButtonPress);
    ui->log("MAIN", "s", "Initialization completed, starting device");
}

void loop()
{
    auto ui = DoughUI::Instance();
    auto data = DataController::Instance();
    auto mqtt = DoughMQTT::Instance();

    ui->processButtonEvents();

    if (!setupNetworkConnection())
    {
        return;
    }

    mqtt->procesIncomingsMessages();

    if (state == CONFIGURING && data->isConfigured())
    {
        setStateToMeasuring();
    }
    else if (state == MEASURING && !data->isConfigured())
    {
        setStateToConfiguring();
    }
    else if (state == MEASURING)
    {
        DataController::Instance()->loop();
    }
    else if (state == CALIBRATING)
    {
        delay(3000);
        setStateToPaused();
    }
    else if (state == PAUSED)
    {
        DataController::Instance()->clearHistory();
    }
}

/**
 * Check if the device is connected to the WiFi network and the MQTT broker.
 * If not, then try to setup the connection.
 * Returns true if the connection was established, false otherwise.
 */
bool setupNetworkConnection()
{
    static auto connectionState = CONNECTING_WIFI;

    auto ui = DoughUI::Instance();
    auto network = DoughWiFi::Instance();
    auto mqtt = DoughMQTT::Instance();

    if (!network->isConnected())
    {
        if (connectionState == CONNECTED)
        {
            ui->log("MAIN", "s", "ERROR - Connection to WiFi network lost! Reconnecting ...");
        }
        else
        {
            ui->log("MAIN", "s", "Connecting to the WiFi network ...");
        }
        connectionState = CONNECTING_WIFI;
        ui->led1.blink()->slow();
        ui->led2.off();
        ui->led3.off();
        network->connect();
    }
    if (network->isConnected() && !mqtt->isConnected())
    {
        if (connectionState == CONNECTED)
        {
            ui->log("MAIN", "s", "ERROR - Connection to the MQTT broker lost! Reconnecting ...");
        }
        else
        {
            ui->log("MAIN", "s", "Connecting to the MQTT broker ...");
        }
        connectionState = CONNECTING_MQTT;
        ui->led1.blink()->fast();
        ui->led2.off();
        ui->led3.off();
        mqtt->connect();
    }
    if (network->isConnected() && mqtt->isConnected())
    {
        if (connectionState != CONNECTED)
        {
            ui->log("MAIN", "s", "Connection to MQTT broker established");
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
    auto ui = DoughUI::Instance();
    ui->log("MAIN", "s", "Waiting for configuration ...");
    state = CONFIGURING;
    ui->led1.on();
    ui->led2.blink()->fast();
    ui->led3.off();
    DoughMQTT::Instance()->publish("state", "configuring");
}

void setStateToMeasuring()
{
    auto ui = DoughUI::Instance();
    ui->log("MAIN", "s", "Starting measurements");
    state = MEASURING;
    ui->led1.on();
    ui->led2.on();
    ui->led3.on();
    DoughMQTT::Instance()->publish("state", "measuring");
}

void setStateToPaused()
{
    auto ui = DoughUI::Instance();
    ui->log("MAIN", "s", "Pausing measurements");
    state = PAUSED;
    ui->led1.on();
    ui->led2.on();
    ui->led3.pulse();
    DoughMQTT::Instance()->publish("state", "paused");
}

void setStateToCalibrating()
{
    auto ui = DoughUI::Instance();
    ui->log("MAIN", "s", "Requested device calibration");
    state = CALIBRATING;
    ui->led1.on();
    ui->led2.blink()->slow();
    ui->led3.off();
    DoughMQTT::Instance()->publish("state", "calibrating");
}
