#ifndef DOUGH_APP_CALLBACKS_H
#define DOUGH_APP_CALLBACKS_H

#include "Network/MQTT.h"
#include "UI/Logger.h"
#include "App/App.h"
#include "Sensors/SensorController.h"

// This header file defines various callback functions that
// live in the global namespace. All callbacks are bundled here
// to have a clear overview of them.

// Callbacks from the Dough::MQTT module.
void mqttOnConnectCallback(Dough::MQTT *mqtt);
void mqttOnMessageCallback(String &topic, String &payload);

// Callbacks from the Dough::UI module.
void onoffButtonInterruptCallback();
void setupButtonInterruptCallback();

#endif