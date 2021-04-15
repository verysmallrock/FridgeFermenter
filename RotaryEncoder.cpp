#include "RotaryEncoder.h"

bool setupInput(void onClick(uint8_t, bool), void onRotate(int)) {
  switches.initialise(ioUsingArduino(), true);
  bool switchOK = switches.addSwitch(ENCODER_BUTTON_PIN, onClick);

  setupRotaryEncoderWithInterrupt(ENCODER_APIN, ENCODER_BPIN, onRotate);
  //switches.changeEncoderPrecision(MAX_ENCODER_VALUE, 100);
}
