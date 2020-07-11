#include "DoughUI.h"

// ----------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------

DoughUI *DoughUI::_instance = nullptr;

DoughUI *DoughUI::Instance()
{
    if (DoughUI::_instance == nullptr)
    {
        DoughUI::_instance = new DoughUI();
    }
    return DoughUI::_instance;
}

DoughUI::DoughUI() : onoffButton(ONOFF_BUTTON_PIN),
                     setupButton(SETUP_BUTTON_PIN),
                     ledBuiltin(LED_BUILTIN),
                     led1(LED1_PIN),
                     led2(LED2_PIN),
                     led3(LED3_PIN) {}

// ----------------------------------------------------------------------
// Setup
// ----------------------------------------------------------------------

void DoughUI::setup()
{
    // Setup the serial port, used for logging.
    Serial.begin(LOG_BAUDRATE);
#ifdef LOG_WAIT_SERIAL
    while (!Serial)
    {
        // wait for serial port to connect. Needed for native USB.
    }
#endif

    // Setup the buttons.
    onoffButton.setup();
    onoffButton.onInterrupt(DoughUI::onoffButtonISR);
    setupButton.setup();
    setupButton.onInterrupt(DoughUI::setupButtonISR);

    // Setup the LEDs.
    ledBuiltin.setup();
    led1.setup();
    led2.setup();
    led3.setup();

    // Setup a timer interrupt that is used to update the
    // user interface (a.k.a. "LEDs") in parallel to other activities.
    // This allows for example to have a flashing LED, during the
    // wifi connection setup.
    _setupTimerInterrupt();

    // Notify the user that we're on a roll!
    _flash_all_leds();
}

void DoughUI::onoffButtonISR()
{
    DoughUI::Instance()->onoffButton.handleButtonState();
}

void DoughUI::setupButtonISR()
{
    DoughUI::Instance()->setupButton.handleButtonState();
}

/**
 * Setup a timer interrupt for updating the GUI. Unfortunately, the standard
 * libraries that I can find for this, are not equipped to work for the
 * Arduino Nano 33 IOT architecture. Luckily, documentation and various
 * helpful threads on the internet helped me piece the following code together.
 */
void DoughUI::_setupTimerInterrupt()
{
    REG_GCLK_GENDIV = GCLK_GENDIV_DIV(200) | // Use divider (32kHz/200 = 160Hz)
                      GCLK_GENDIV_ID(4);     // for Generic Clock GCLK4
    while (GCLK->STATUS.bit.SYNCBUSY)
        ; // Wait for synchronization

    REG_GCLK_GENCTRL = GCLK_GENCTRL_IDC |        // Set the duty cycle to 50/50 HIGH/LOW
                       GCLK_GENCTRL_GENEN |      // and enable the clock
                       GCLK_GENCTRL_SRC_OSC32K | // using the 32kHz clock source as input
                       GCLK_GENCTRL_ID(4);       // for Generic Clock GCLK4
    while (GCLK->STATUS.bit.SYNCBUSY)
        ; // Wait for synchronization

    REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN |     // Enable timer
                       GCLK_CLKCTRL_GEN_GCLK4 | // using Generic Clock GCLK4 as input
                       GCLK_CLKCTRL_ID_TC4_TC5; // and feed its output to TC4 and TC5
    while (GCLK->STATUS.bit.SYNCBUSY)
        ; // Wait for synchronization

    REG_TC4_CTRLA |= TC_CTRLA_PRESCALER_DIV8 | // Use prescaler (160Hz / 8 = 20Hz)
                     TC_CTRLA_WAVEGEN_MFRQ |   // Use match frequency (MFRQ) mode
                     TC_CTRLA_MODE_COUNT8 |    // Set the timer to 8-bit mode
                     TC_CTRLA_ENABLE;          // Enable TC4
    REG_TC4_INTENSET = TC_INTENSET_OVF;        // Enable TC4 overflow (OVF) interrupts
    REG_TC4_COUNT8_CC0 = 1;                    // Set the CC0 as the TOP value for MFRQ (1 => 50ms per pulse)
    while (TC4->COUNT8.STATUS.bit.SYNCBUSY)
        ; // Wait for synchronization

    // Enable interrupts for TC4 in the Nested Vector InterruptController (NVIC),
    NVIC_SetPriority(TC4_IRQn, 0); // Set NVIC priority for TC4 to 0 (highest)
    NVIC_EnableIRQ(TC4_IRQn);      // Enable TC4 interrupts
}

// ----------------------------------------------------------------------
// Loop
// ----------------------------------------------------------------------

/**
 * This callback is called when the TC4 timer hits an overflow interrupt.
 */
void TC4_Handler()
{
    DoughUI::Instance()->updatedLEDs();
    REG_TC4_INTFLAG = TC_INTFLAG_OVF; // Clear the OVF interrupt flag.
}

/**
 * Disables the TC4 interrupts, suspending timed async updates to
 * the user interface.
 */
void DoughUI::suspend()
{
    NVIC_DisableIRQ(TC4_IRQn);
}

/**
 * Enables the TC4 interrupts, resuming timed async updates to the
 * user interface.
 */
void DoughUI::resume()
{
    NVIC_EnableIRQ(TC4_IRQn);
}

/**
 * Fire pending button events.
 */
void DoughUI::processButtonEvents()
{
    onoffButton.loop();
    setupButton.loop();
}

/**
 * Clear pending button events.
 */
void DoughUI::clearButtonEvents()
{
    onoffButton.clearEvents();
    setupButton.clearEvents();
}

/**
 * Update the state of all the LEDs in the system.
 * This method is called both sync by methods in this class and async by
 * the timer interrupt code from above. The timer interrupt based invocation
 * makes it possible to do LED updates, while the device is busy doing
 * something else.
 */
void DoughUI::updatedLEDs()
{
    ledBuiltin.loop();
    led1.loop();
    led2.loop();
    led3.loop();
}

/**
 * Flash all LEDs, one at a time.
 */
void DoughUI::_flash_all_leds()
{
    ledBuiltin.on();
    delay(100);
    ledBuiltin.off();
    led1.on();
    delay(100);
    led1.off();
    led2.on();
    delay(100);
    led2.off();
    led3.on();
    delay(100);
    led3.off();
}
