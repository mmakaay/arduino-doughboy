#include "App/callbacks.h"

Dough::Logger callbackLogger("CALLBACK");

void mqttOnConnectCallback(Dough::MQTT *mqtt)
{
    callbackLogger.log("s", "MQTT connection establish, subscribing to topics");
    mqtt->subscribe("container_height");
    mqtt->subscribe("temperature_offset");
}

void mqttOnMessageCallback(String &topic, String &payload)
{
    callbackLogger.log("ssss", "MQTT message received: ", topic.c_str(), " = ", payload.c_str());

    if (topic.endsWith("/container_height"))
    {
        Dough::App::Instance()->config.setContainerHeight(payload.toInt());
    }
    else if (topic.endsWith("/temperature_offset"))
    {
        Dough::App::Instance()->config.setTemperatureOffset(payload.toInt());
    }
    else
    {
        callbackLogger.log("ss", "ERROR - Unhandled MQTT message, topic = ", topic.c_str());
    }
}

void onoffButtonInterruptCallback()
{
    Dough::App::Instance()->ui.onoffButton.handleButtonState();
}

void setupButtonInterruptCallback()
{
    Dough::App::Instance()->ui.setupButton.handleButtonState();
}

// This callback is called when the TC4 timer from the UI code hits an overflow
// interrupt. It is defined outside the Dough namespace, because TC4_Handler is
// a hard-coded root namespace function name.
void TC4_Handler()
{
    Dough::App::Instance()->ui.updateLEDs();
    REG_TC4_INTFLAG = TC_INTFLAG_OVF; // Clear the OVF interrupt flag.
}