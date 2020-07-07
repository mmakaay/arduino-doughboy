// WPA2 WiFi connection configuration.
#define WIFI_SSID         "<SSID network name>"
#define WIFI_PASSWORD     "<network password>"

// MQTT broker configuration.
#define MQTT_BROKER       "<IP or hostname>"
#define MQTT_PORT         1883
#define MQTT_USERNAME     "<mqtt username>"
#define MQTT_PASSWORD     "<mqtt password>"

// The prefix to use for the MQTT publishing topic.
#define MQTT_TOPIC_PREFIX "sensors/doughboy"

// Define this one to not use the WiFi MAC address as the device ID
// in the publish topics (sensors/doughboy/<MQTT_DEVICE_ID>/...)
//#define MQTT_DEVICE_ID    "1"

// Define this one to wait for USB serial to come up.
// This can be useful during development, when you want all
// serial messages to appear in the serial monitor.
// Without this, some of the initial serial messages might
// be missing from the output.
//#define LOG_WAIT_SERIAL