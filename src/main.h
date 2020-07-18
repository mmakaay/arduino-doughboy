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
    CONNECTING_WIFI,
    CONNECTING_MQTT,
    CONNECTED
} DoughBoyConnectionState;

typedef enum
{
    CONFIGURING,
    MEASURING,
    PAUSED,
    CALIBRATING
} DoughBoyState;

bool setupNetworkConnection();
void handleMqttMessage(String &topic, String &payload);
void handleOnoffButtonPress();
void handleSetupButtonPress();
void setStateToConfiguring();
void setStateToMeasuring();
void setStateToPaused();
void setStateToCalibrating();

#endif
