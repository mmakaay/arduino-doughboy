#ifndef DOUGHBOY_H
#define DOUGHBOY_H

#include <Arduino.h>
#include "App/App.h"
#include "Sensors/HighLevel/TemperatureSensor.h"
#include "Sensors/HighLevel/HumiditySensor.h"
#include "Sensors/HighLevel/DistanceSensor.h"
#include "Network/WiFi.h"
#include "Network/MQTT.h"
#include "UI/Button.h"
#include "UI/UI.h"
#include "config.h"

typedef enum
{
    CONFIGURING,
    MEASURING,
    PAUSED,
    CALIBRATING
} DoughBoyState;

void handleOnoffButtonPress();
void handleSetupButtonPress();

void setStateToConfiguring();
void setStateToMeasuring();
void setStateToPaused();
void setStateToCalibrating();

#endif
