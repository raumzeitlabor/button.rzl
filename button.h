/*
 * button.h - small button library
 *
 * This code is free software under the non-terms
 * of the Anti-License. Do whatever the fuck you want.
 */
#ifndef button_h
#define button_h

#include <Arduino.h>

// typedefs for better readability
typedef unsigned long time_t;
typedef int state;

// pins
int TRIG1_PIN = 2; // brown cable
int TRIG2_PIN = 3; // yellow cable
int LED_PIN   = 13; // status led, for testing

// other globals
state LED_STATE = LOW;

/* This class implements a simple abstraction over a single trigger,
 * including debouncing logic and timing information.
 */
class Trigger {
    // the pin this trigger reads from
    int pin;

    // whether the input signal should be inverted
    bool invert;

    // state bookkeeping
    state current_in = LOW;
    state last_in    = LOW;
    bool debouncing  = false;
    time_t debounce_timer = 0;

    public:
    // delay until a press of the trigger is registered
    time_t debounce_delay = 50;
    // whether the trigger is currently pressed
    bool pressed = false;
    // time when the last button press was registered
    time_t last_press = 0;

    Trigger(void);
    Trigger(int pin, time_t debounce_delay, bool invert);

    // update the internal state
    void update(void);

};

/* A higher level abstraction that uses two triggers to implement fast
 * keypresses.
 */
class Button {
    Trigger trig1;
    Trigger trig2;
    // button presses where the time difference between the trigger presses is
    // below this interval are considered a fast press
    time_t fast_press_time;

    public:
    bool pressed;
    bool fast_pressed;
    time_t trigger_diff;

    Button(Trigger*, Trigger*, time_t);
    
    // update the internal state
    void update(void);
};

#endif
