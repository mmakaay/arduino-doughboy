// The digital pin to which the DATA pin of the DHT11
// temperature/humidity sensor is connected.
#define DHT11_DATA_PIN        10

// The digital pins to which the TRIG and ECHO pins of
// the HCSR04 distance sensor are connected.
#define HCSR04_TRIG_PIN       4
#define HCSR04_ECHO_PIN       5

// The digital pins to which the three LEDs are connected.
#define LED1_PIN              8
#define LED2_PIN              7
#define LED3_PIN              6

// The digital pins to which the push buttons are connected. 
#define ONOFF_BUTTON_PIN      2
#define SETUP_BUTTON_PIN      3

// The network configuration and possibly overrides for the above
// definitions are stored in a separate header file, which is
// not stored in the repository. Before compiling this code,
// rename or copy the file config_local.example.h to config_local.h
// and update the settings in that file.
#include "config_local.h"
