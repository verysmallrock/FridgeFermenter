#ifndef ROTARY_ENCODER_INCLUDE
#define ROTARY_ENCODER_INCLUDE

#include<IoAbstraction.h>

#define ENCODER_BUTTON_PIN A3
#define ENCODER_APIN 2
#define ENCODER_BPIN 3
#define CLICKS_PER_STEP   4

bool setupInput(void onClick(uint8_t, bool), void onRotate(int));
#endif
