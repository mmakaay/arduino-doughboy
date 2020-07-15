#include "UI.h"

namespace Dough
{
    UI::UI() : onoffButton(ONOFF_BUTTON_PIN),
               setupButton(SETUP_BUTTON_PIN),
               ledBuiltin(LED_BUILTIN),
               led1(LED1_PIN),
               led2(LED2_PIN),
               led3(LED3_PIN) {}

    void UI::setup()
    {
        // Setup the buttons.
        onoffButton.setup();
        setupButton.setup();

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

        // Enable the timer interrupt handling.
        resume();
    }

    // Setup a timer interrupt for updating the GUI. Unfortunately, the standard
    // libraries that I can find for this, are not equipped to work for the
    // Arduino Nano 33 IOT architecture. Luckily, documentation and various
    // helpful threads on the internet helped me piece the following code together.
    void UI::_setupTimerInterrupt()
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
    }

    // Enables the TC4 interrupts in the Nested Vector InterruptController (NVIC),
    // starting timed async updates for the user interface.
    void UI::resume()
    {
        NVIC_SetPriority(TC4_IRQn, 0); // Set NVIC priority for TC4 to 0 (highest)
        NVIC_EnableIRQ(TC4_IRQn);      // Enable TC4 interrupts
    }

    // Disables the TC4 interrupts, suspending timed async updates to
    // the user interface.
    void UI::suspend()
    {
        NVIC_DisableIRQ(TC4_IRQn);
    }

    // Fire pending button events.
    void UI::processButtonEvents()
    {
        onoffButton.loop();
        setupButton.loop();
    }

    // Clear pending button events.
    void UI::clearButtonEvents()
    {
        onoffButton.clearEvents();
        setupButton.clearEvents();
    }

    // Update the state of all the LEDs in the system.
    // This method is called both sync by methods in this class and async by
    // the timer interrupt code from above. The timer interrupt based invocation
    // makes it possible to do LED updates, while the device is busy doing
    // something else.
    void UI::updateLEDs()
    {
        ledBuiltin.loop();
        led1.loop();
        led2.loop();
        led3.loop();
    }

    void UI::notifyConnectingToWifi()
    {
        led1.blink()->slow();
        led2.off();
        led3.off();
    }
    
    void UI::notifyConnectingToMQTT()
    {
        led1.blink()->fast();
        led2.off();
        led3.off();
    }

    void UI::notifyWaitingForConfiguration()
    {
        led1.on();
        led2.blink()->slow();
        led3.off();
    }

    void UI::notifyCalibrating()
    {
        led1.on();
        led2.blink()->fast();
        led3.off();
    }

    void UI::notifyMeasurementsActive()
    {
        led1.on();
        led2.on();
        led3.on();
    }

    void UI::notifyMeasurementsPaused()
    {
        led1.on();
        led2.on();
        led3.pulse();
    }

    void UI::notifySensorActivity()
    {
        led3.off();
        delay(50);
        led3.on();        
    }

    void UI::notifyNetworkActivity()
    {
        led1.off();
        delay(50);
        led1.on();        
    }
    
    // Flash all LEDs, one at a time in a synchroneous manner, making
    // this work when the UI timer interrupt is inactive. This is used
    // as a "Hey, I'm awake!" signal from the device after booting up.
    void UI::_flash_all_leds()
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
} // namespace Dough