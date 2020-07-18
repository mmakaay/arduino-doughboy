#include "main.h"

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
    app->loop();
    
    // if (state == CONFIGURING)
    // {
    //     if (app->config.isOk())
    //     {
    //         setStateToMeasuring();
    //     }
    // }
    // else if (state == MEASURING && !app->config.isOk())
    // {
    //     setStateToConfiguring();
    // }
    // else if (state == MEASURING)
    // {
    //     app->measure();
    // }
    // else if (state == CALIBRATING)
    // {
    //     delay(3000);
    //     setStateToPaused();
    // }
    // else if (state == PAUSED)
    // {
    //     app->clearHistory();
    // }
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
