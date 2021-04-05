#ifndef ROTARY_ENCODER_INCLUDE
#define ROTARY_ENCODER_INCLUDE

#include "Button2.h"; //  https://github.com/LennartHennigs/Button2
#include<IoAbstraction.h>

#define ENCODER_BUTTON_PIN A3
#define ENCODER_APIN 2
#define ENCODER_BPIN 3
#define CLICKS_PER_STEP   4

#define MAX_ENCODER_VALUE 128 // TBD

void setupInput(void onClick(uint8_t, bool), void onRotate(int));
#endif
