// The speed to use for the serial terminal logging.
#define LOG_BAUDRATE 9600

// Define this one to wait for USB serial to come up.
// This can be useful during development, when you want all
// serial messages to appear in the serial monitor.
// Without this, some of the initial serial messages might
// be missing from the output.
#undef LOG_WAIT_SERIAL

// The digital pins to which the push buttons are connected.
#define ONOFF_BUTTON_PIN 2
#define SETUP_BUTTON_PIN 3

// The digital pins to which the TRIG and ECHO pins of
// the HCSR04 distance sensor are connected.
#define HCSR04_TRIG_PIN 4
#define HCSR04_ECHO_PIN 5

// The digital pin to which the DATA pin of the DHT11
// temperature/humidity sensor is connected.
#define DHT11_DATA_PIN 10

// The digital pins to which the three LEDs are connected.
#define LED1_PIN 8
#define LED2_PIN 7
#define LED3_PIN 6 // PWM pin, so pulsing during "device paused" is possible

// Two different values are published per sensor: a recent value and an average
// value. These definitions define how often measurements must be taken and
// how many values must be stored to compute an average.
#define TEMPERATURE_MEASURE_INTERVAL 30 // measure twice per minute
#define TEMPERATURE_AVERAGE_STORAGE 6   // making this a 3 minute average
#define HUMIDITY_MEASURE_INTERVAL 30    // measure twice per minute
#define HUMIDITY_AVERAGE_STORAGE 6      // making this a 3 minute average
#define DISTANCE_MEASURE_INTERVAL 1     // measure every second
#define DISTANCE_AVERAGE_STORAGE 180    // making this a 3 minute average

// The minimum interval in seconds at which to publish measurements to MQTT.
// This can be seen as keep-alive functionality, allowing the receiving
// side to consider the device off-line when no new data have come in
// after this interval.
#define MINIMUM_PUBLISH_INTERVAL 300

// The network configuration and possibly overrides for the above
// definitions are stored in a separate header file, which is
// not stored in the repository. Before compiling this code,
// rename or copy the file config_local.example.h to config_local.h
// and update the settings in that file.
#include "config_local.h"
