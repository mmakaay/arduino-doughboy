#ifndef DOUGH_STATEMACHINE_H
#define DOUGH_STATEMACHINE_H

#include <Arduino.h>
#include "UI/Logger.h"

namespace Dough
{
    typedef enum
    {
        INITIALIZING,
        WIFI_CONNECTION_LOST,
        CONNECTING_WIFI,
        MQTT_CONNECTION_LOST,
        CONNECTING_MQTT,
        CONNECTION_ESTABLISHED,
        CONFIGURING,
        MEASURING,
        CALIBRATING,
        PAUSED,
        INVALID
    } AppState;

    typedef enum
    {
        CONNECTING,
        CONNECTED,
        CONNECTION_LOST
    } ConnectionState;

    typedef enum
    {
        CONFIGURE,
        MEASURE,
        CALIBRATE
    } AppOperationMode;

    // This class defines an interface that can be implemented to create a plugin
    // for the Dough::AppState class. This plugin can act upon state changes.
    class AppStateControllerPluginBase
    {
    public:
        virtual void onStateChange(AppState oldState, AppState newState);
    };

    class AppStateController
    {
    public:
        AppStateController(AppStateControllerPluginBase *plugin);
        void setup();
        void setWiFiConnected(bool connected);
        void setMQTTConnected(bool connected);
        void startConfiguration();
        void startMeasurements();
        void startCalibration();
        void pauseDevice();
        void resumeDevice();
        AppState get();

    private:
        Logger _logger;
        AppStateControllerPluginBase *_plugin;
        AppState _appState = INITIALIZING;
        ConnectionState _wifiConnected = CONNECTING;
        ConnectionState _mqttConnected = CONNECTING;
        AppOperationMode _operationMode = CONFIGURE;
        bool _paused = false;
        void _updateState();
    };
} // namespace Dough

#endif