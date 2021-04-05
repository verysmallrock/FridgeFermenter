#include <SPI.h>

#ifndef DISPLAY_INCLUDE
#define DISPLAY_INCLUDE

void setupDisplay();
void updateDisplay(bool bg, bool title, bool text);

void printTitle(String text);
void printText(int line, String textLeft, String textRight = "");

void printGraphBg(int line, String min, String max);
void printNextGraphPoint(int line, float percent);

#endif
