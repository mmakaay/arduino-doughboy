#ifndef DOUGH_APPSTATECONTROLLERPLUGIN_H
#define DOUGH_APPSTATECONTROLLERPLUGIN_H

#include <Arduino.h>
#include "App/AppStateController.h"
#include "Network/MQTT.h"
#include "UI/Logger.h"
#include "UI/UI.h"

namespace Dough
{
    class AppStateControllerPlugin : public AppStateControllerPluginBase
    {
    public:
        AppStateControllerPlugin(UI *ui, MQTT *mqtt);
        virtual void onStateChange(AppState oldState, AppState newState);
    private:
        UI *_ui;
        MQTT *_mqtt;
        Logger _logger;
    };
} // namespace Dough

#endif