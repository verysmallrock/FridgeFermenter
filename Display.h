#include <SPI.h>

#ifndef DISPLAY_INCLUDE
#define DISPLAY_INCLUDE

#define GRAY_0   0x0000 // 0, 0, 0
#define GRAY_100 0x10A2 // 21, 21, 21
#define GRAY_200 0x39E7 // 60
#define GRAY_300 0x630C // 99
#define GRAY_400 0x8C51 // 138
#define GRAY_500 0xB596 // 177
#define GRAY_600 0xDEDB // 216
#define GRAY_700 0xFFFF // 255
// use http://www.barth-dev.de/online/rgb565-color-picker/
#define YELLOW_200 0xEEA7 //238, 214, 58
#define ORANGE_200 0xEB87 // 238, 112, 58
#define GREEN_200 0x4F67 // 79, 238, 58

void setupDisplay();
void initTft();
void updateDisplay(bool bg, bool title, bool text);

void printTitle(String text);
void printText(int line, String textLeft, String textRight = "");
void printTextFancy(int line, String textLeft[], uint16_t leftColors[], int leftLen, String textRight[], uint16_t rightColors[], int rightLen);
void printTopRightStatus(String status);

void printGraphBg(int line, String min, String max, String bottomRightStr);
void setGraphBottomRightStr(int line, String bottomRightStr);
void printNextGraphPoint(int line, float percent);
void resetLinePositions();

#endif
