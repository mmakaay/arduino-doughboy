#include "App/SensorControllerPlugin.h"

namespace Dough
{
    SensorControllerPlugin::SensorControllerPlugin(MQTT *mqtt, UI *ui) : _mqtt(mqtt), _ui(ui)
    {
    }
    
    void SensorControllerPlugin::doPublish(SensorController *controller) {
        Serial.println(">>>>>>>>>> YO PLUGIN HERE! <<<<<<<<<<<");
    }

    void SensorControllerPlugin::beforeMeasure(SensorController *controller)
    {
        _ui->notifySensorActivity();        
    }

    void SensorControllerPlugin::beforePublish(SensorController *controller)
    {
        _ui->notifyNetworkActivity();
    }
}