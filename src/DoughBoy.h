#ifndef DOUGHBOY_H
#define DOUGHBOY_H

#include <Arduino.h>
#include "DoughNetwork.h"
#include "DoughMQTT.h"
#include "DoughSensors.h"
#include "DoughData.h"
#include "DoughButton.h"
#include "DoughUI.h"
#include "config.h"

typedef enum {
    CONNECTING_WIFI,
    CONNECTING_MQTT,
    CONNECTED
} DoughBoyConnectionState;

typedef enum {
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
