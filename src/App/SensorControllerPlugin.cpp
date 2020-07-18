#include "App/SensorControllerPlugin.h"

namespace Dough
{
    SensorControllerPlugin::SensorControllerPlugin(MQTT *mqtt, UI *ui) : _mqtt(mqtt), _ui(ui) {}
    
    void SensorControllerPlugin::beforeMeasure(SensorController *controller)
    {
        _ui->notifySensorActivity();

        // Suspend the user interface interrupts, to not disturb the timing-sensitive
        // sensor readings.
        _ui->suspend();
    }

    void SensorControllerPlugin::afterMeasure(SensorController *controller)
    {
        _ui->resume();
    }

    void SensorControllerPlugin::beforePublish(SensorController *controller)
    {
        _ui->notifyNetworkActivity();
    }

    void SensorControllerPlugin::doPublish(SensorController *controller, Measurement last, Measurement average)
    {
        const char *sensorName = controller->getSensorName();
        static char mqttAverageKey[50];
        snprintf(mqttAverageKey, 50, "%s/average", sensorName);
        _mqtt->publish(sensorName, last);
        _mqtt->publish(mqttAverageKey, average);
    }
}