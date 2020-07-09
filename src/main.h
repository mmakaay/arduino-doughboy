#ifndef DOUGHBOY_H
#define DOUGHBOY_H

#include <Arduino.h>
#include "Network/DoughWiFi.h"
#include "Network/DoughMQTT.h"
#include "Sensors/DoughSensors.h"
#include "Data/DataController.h"
#include "UI/DoughButton.h"
#include "UI/DoughUI.h"
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
