#include "main.h"

// TODO: move config to a separate class
// TODO: see if I can give each sensor its own built-in loop schedule for sampling, the DoughData class might be overkill in the latest setup.
// TOOD: implement the calibration logic
// TODO: see what more stuff can be moved to the UI code. Maybe state to UI state translation ought to be there as well
// TODO: make significantChange part of sensor class?

DoughBoyState state = CONFIGURING;
auto logger = DoughLogger("MAIN");

void setup()
{
    TemperatureSensor::Instance()->setup();
    HumiditySensor::Instance()->setup();
    DistanceSensor::Instance()->setup();
    DoughWiFi::Instance()->setup();
    DoughMQTT::Instance()->setup();
    DataController::Instance()->setup();
    auto ui = DoughUI::Instance();
    ui->setup();
    ui->onoffButton.onPress(handleOnoffButtonPress);
    ui->setupButton.onPress(handleSetupButtonPress);
    logger.log("s", "Initialization completed, starting device");
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
        network->connect();
    }
    if (network->isConnected() && !mqtt->isConnected())
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
        mqtt->connect();
    }
    if (network->isConnected() && mqtt->isConnected())
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
    auto ui = DoughUI::Instance();
    logger.log("s", "Waiting for configuration ...");
    state = CONFIGURING;
    ui->led1.on();
    ui->led2.blink()->fast();
    ui->led3.off();
    DoughMQTT::Instance()->publish("state", "configuring");
}

void setStateToMeasuring()
{
    auto ui = DoughUI::Instance();
    logger.log("s", "Starting measurements");
    state = MEASURING;
    ui->led1.on();
    ui->led2.on();
    ui->led3.on();
    DoughMQTT::Instance()->publish("state", "measuring");
}

void setStateToPaused()
{
    auto ui = DoughUI::Instance();
    logger.log("s", "Pausing measurements");
    state = PAUSED;
    ui->led1.on();
    ui->led2.on();
    ui->led3.pulse();
    DoughMQTT::Instance()->publish("state", "paused");
}

void setStateToCalibrating()
{
    auto ui = DoughUI::Instance();
    logger.log("s", "Requested device calibration");
    state = CALIBRATING;
    ui->led1.on();
    ui->led2.blink()->slow();
    ui->led3.off();
    DoughMQTT::Instance()->publish("state", "calibrating");
}
