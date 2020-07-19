#include "App/AppStateController.h"

namespace Dough
{
    AppStateController::AppStateController(AppStateControllerPluginBase *plugin) : _logger("STATE"),
                                                                                   _plugin(plugin) {}

    void AppStateController::setup()
    {
        // Trigger a state change event, so the plugin can act upon it.
        _plugin->onStateChange(get(), INITIALIZING);
    }

    AppState AppStateController::get()
    {
        if (_wifiConnected != CONNECTED)
        {
            return _wifiConnected == CONNECTION_LOST ? WIFI_CONNECTION_LOST : CONNECTING_WIFI;
        }
        if (_mqttConnected != CONNECTED)
        {
            return _mqttConnected == CONNECTION_LOST ? MQTT_CONNECTION_LOST : CONNECTING_MQTT;
        }
        if (_operationMode == CONFIGURE)
        {
            return CONFIGURING;
        }
        if (_operationMode == MEASURE)
        {
            return _paused ? PAUSED : MEASURING;
            ;
        }
        if (_operationMode == CALIBRATE)
        {
            return CALIBRATING;
        }
        _logger.log("s", "get(): Unable to determine state!");
        return INVALID;
    }

    void AppStateController::setWiFiConnected(bool connected)
    {
        if (connected)
        {
            _wifiConnected = CONNECTED;
        }
        else
        {
            _wifiConnected = _wifiConnected == CONNECTED ? CONNECTION_LOST : CONNECTING;
            setMQTTConnected(false);
        }
        _updateState();
    }

    void AppStateController::setMQTTConnected(bool connected)
    {
        if (connected)
        {
            // Trigger a connection established state, so the plugin can act
            // upon this specific event. At the end of this method, the status update
            // call will push the system to the next state.
            if (_mqttConnected != CONNECTED)
            {
                _appState = CONNECTION_ESTABLISHED;
                _plugin->onStateChange(get(), _appState);
            }
            _mqttConnected = CONNECTED;
        }
        else
        {
            _mqttConnected = _mqttConnected == CONNECTED ? CONNECTION_LOST : CONNECTING;
        }
        _updateState();
    }

    void AppStateController::startConfiguration()
    {
        _operationMode = CONFIGURE;
        _updateState();
    }

    void AppStateController::startMeasurements()
    {
        _operationMode = MEASURE;
        _updateState();
    }

    void AppStateController::startCalibration()
    {
        _operationMode = CALIBRATE;
        _updateState();
    }

    void AppStateController::pauseMeasurements()
    {
        _paused = true;
        _updateState();
    }

    void AppStateController::resumeMeasurements()
    {
        _paused = false;
        _updateState();
    }

    void AppStateController::_updateState()
    {
        auto oldState = _appState;
        auto newState = get();
        if (newState != oldState)
        {
            _plugin->onStateChange(oldState, newState);
            _appState = newState;
        }
    }
} // namespace Dough
