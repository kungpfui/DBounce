#include <DBounce.h>

/**
  DBounce demonstration sketch.
  Turns on/off LEDs in dependence of debounced tactile switches.
*/

const uint8_t leds[] = { 0, 2, 4, 6 };        // digital gpio numbers
const uint8_t tactiles[] = { 1, 3, 5, 7 };    // digital gpio numbers
DBounce<uint8_t, __DBNC_N(tactiles), 30, 30> buttons(tactiles);


// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital outputs, here some leds
  for (uint8_t i = 0; i < __DBNC_N(leds); i++) {
    digitalWrite(leds[i], HIGH);
    pinMode(leds[i], OUTPUT);
  }

  // call the setup function of the deounce object.
  // the setup function initialise the GPIOs as specified
  buttons.setup(INPUT_PULLUP);
}


// the loop routine runs over and over again forever:
void loop() {
  buttons.update();

  // find buttons with a state change and reflect the new state with some LEDs.
  // any change?
  if (buttons.change() != 0) {

    // first button
    if (buttons.change(0)) {

      // high (=true) or low (=false)?
      if (buttons[0]) {
        // => rose (de-pressed)
        digitalWrite(leds[0], HIGH);
      } else {
        // => fell (pressed)
        digitalWrite(leds[0], LOW);
      }
    }

    // do the same with the other buttons and LEDs.
    // of course you can write this code much simpler and in a loop.
    uint8_t idx;
    for(idx = 1; idx < __DBNC_N(tactiles); idx++) {
      if (buttons.change(idx)) {
        digitalWrite(leds[idx], buttons[idx]);
      }
    }
    
  }
}
