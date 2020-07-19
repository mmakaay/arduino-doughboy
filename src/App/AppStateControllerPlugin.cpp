#include "App/AppStateControllerPlugin.h"

namespace Dough
{
    AppStateControllerPlugin::AppStateControllerPlugin(UI *ui, MQTT *mqtt) : _ui(ui),
                                                                             _mqtt(mqtt),
                                                                             _logger("STATE") {}

    void AppStateControllerPlugin::onStateChange(AppState oldState, AppState newState)
    {
        switch (newState)
        {
        case INITIALIZING:
            _logger.log("s", "Initializing the device");
            break;
        case CONNECTING_WIFI:
            _logger.log("s", "Connecting to the WiFi network");
            _ui->notifyConnectingToWifi();
            break;
        case WIFI_CONNECTION_LOST:
            _logger.log("s", "ERROR - Connection to WiFi network lost! Reconnecting");
            _ui->notifyConnectingToWifi();
            break;
        case CONNECTING_MQTT:
            _logger.log("s", "Connecting to the MQTT broker");
            _ui->notifyConnectingToMQTT();
            break;
        case MQTT_CONNECTION_LOST:
            _logger.log("s", "ERROR - Connection to the MQTT broker lost! Reconnecting");
            _ui->notifyConnectingToMQTT();
            break;
        case CONNECTION_ESTABLISHED:
            _logger.log("s", "Connection to MQTT broker established");
            _ui->notifyConnected();
            _ui->clearButtonEvents();
            _mqtt->publish("state", "connected");
            break;
        case CONFIGURING:
            _logger.log("s", "Waiting for configuration");
            _ui->notifyWaitingForConfiguration();
            _mqtt->publish("state", "configuring");
            break;
        case MEASURING:
            _logger.log("s", "Starting measurements");
            _ui->notifyMeasurementsActive();
            _mqtt->publish("state", "measuring");
            break;
        case PAUSED:
            _logger.log("s", "Pausing measurements");
            _ui->notifyMeasurementsPaused();
            _mqtt->publish("state", "paused");
            break;
        case CALIBRATING:
            _logger.log("s", "Requested device calibration");
            _ui->notifyCalibrating();
            _mqtt->publish("state", "calibrating");
            break;
        default:
            _logger.log("sisi", "Unhandled state transfer: ", oldState, " -> ", newState);
        }
    }
} // namespace Dough
