/* 
 * button.ino - firmware for the RZL mushroom button
 * 
 * Author: slowpoke <mail+rzl@slowpoke.io>
 * 
 * This program is Free Software under the non-terms
 * of the Anti-License. Do whatever the fuck you want.
 */
#include "button.h"

Trigger::Trigger() {};

Trigger::Trigger(int pin, time_t debounce_delay, bool invert=false) {
    this -> pin = pin;
    this -> debounce_delay = debounce_delay;
    this -> invert = invert;
}

void Trigger::update(void) {
    time_t now = millis();

    // get the current input signal
    this -> current_in = (this -> invert) ? !digitalRead(this -> pin)
                                          : digitalRead(this -> pin);

    // check for flanks in either direction
    if (this -> current_in != this -> last_in) {
        // begin debouncing
        this -> debouncing = true;
        // reset debounce timer
        this -> debounce_timer = now;
    }

    // if we're debouncing & the input signal is stable, check debounce timer
    if (this -> debouncing && (this -> current_in == this -> last_in)) {
        if (now - this -> debounce_timer > this -> debounce_delay) {
            // register a button press/release if the
            // debounce timer is exceeded
            this -> pressed = this -> current_in;
            // stop debouncing
            this -> debouncing = false;
            // if this is a press (not a release), remember the time
            if (this -> current_in == HIGH) {
                this -> last_press = now;
            }
        }
    }
    if (this -> current_in == LOW) {
        // reset button state
        this -> pressed = false;
    }
    // save input signal state
    this -> last_in = this -> current_in;
}

Button::Button(Trigger* trig1, Trigger* trig2, time_t fast_press_time = 20) {
    this -> trig1 = *trig1;
    this -> trig2 = *trig2;
    this -> fast_press_time = fast_press_time;
}

void Button::update(void) {
    this -> trig1.update();
    this -> trig2.update();

    if (!this -> pressed) {
        // check for flank (both triggers pressed)
        if (this -> trig1.pressed && this -> trig2.pressed) {
            this -> pressed = true;
            // we don't care which of the two triggers is pressed first,
            // so we simple use an absolute value for the difference
            this -> trigger_diff = abs(this -> trig1.last_press - 
                                       this -> trig2.last_press);
            // check if this was fast press
            if (this -> trigger_diff < this -> fast_press_time) {
                this -> fast_pressed = true;
            }
        }
    } else {
        // check if either of the triggers is no longer pressed
        if (!(this -> trig1.pressed && this -> trig2.pressed)) {
            // reset the button state
            this -> pressed = false;
            this -> fast_pressed = false;
        }
    }
}

// global state
Trigger* TRIG1_STATE = new Trigger(TRIG1_PIN, 2, true);
Trigger* TRIG2_STATE = new Trigger(TRIG2_PIN, 2, false);
Button* BUTTON = new Button(TRIG1_STATE, TRIG2_STATE, 5);

void setup(void) {
    pinMode(TRIG1_PIN, INPUT_PULLUP);
    pinMode(TRIG2_PIN, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);

    // LED TEST
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);

    Serial.begin(9600);
    while (!Serial) {;}
}


void loop(void) {
    BUTTON -> update();
}
