#include "Display.h"
#define USE_UCGLIB false
#define DEBUG false

#if USE_UCGLIB
#include "Ucglib.h"

#define T 4000
#define DLY() delay(2000)

Ucglib_ST7735_18x128x160_SWSPI ucg(/*sclk=*/ 12, /*data=*/ 11, /*cd=*/ 9, /*cs=*/ 10, /*reset=*/ 8);
//Ucglib_ST7735_18x128x160_HWSPI ucg(/*cd=*/ 9, /*cs=*/ 10, /*reset=*/ 8);

bool resetBackground = true;

#define TopOfScreen 5
#define LeftOfScreen 5
#define TitleFont ucg_font_fub17_hr // 17px tall
#define TitleFontHeight 17

#define SmallFont ucg_font_helvB10_hr // 11px tall
#define SmallFontHeight 13

#define TinyFont ucg_font_fixed_v0_hf // 7px tall
#define TinyFontHeight 7

#define LineSpacing 4

void setupDisplay() {
  ucg.begin(UCG_FONT_MODE_TRANSPARENT);
  ucg.setRotate270();
  ucg.clearScreen();
  ucg.setPrintDir(0);
}

void drawBackground() {
  if (!resetBackground) {
    return;
  }
  ucg.setColor(0, 21, 21, 21);

  ucg.drawBox(0, 0, ucg.getWidth(), ucg.getHeight());
  resetBackground = false;
}

void clearTitle() {
  ucg.setColor(0, 21, 21, 21);
  ucg.drawBox(0, 0, ucg.getWidth(), TopOfScreen + TitleFontHeight);
}

void clearText() {
  ucg.setColor(0, 21, 21, 21);
  int top = TopOfScreen + TitleFontHeight;
  ucg.drawBox(0, top, ucg.getWidth(), ucg.getHeight() - top);
}

int getLineTop(int line) {
  if (line == 0) { // title
    return TopOfScreen + TitleFontHeight;
  } else {
    return TopOfScreen + TitleFontHeight + line * LineSpacing + (line) * SmallFontHeight;
  }
}

String lastTextLeft[10];
String lastTextRight[10];
void printText(int line, String textLeft, String textRight) {
  ucg.setFont(SmallFont);
  ucg.setColor(221, 221, 221);
  ucg.setPrintPos(2, getLineTop(line));
  int lTop = getLineTop(line);
  int winWidth = ucg.getWidth();

  if (textLeft != lastTextLeft[line]) {
    ucg.print(textLeft);
    lastTextLeft[line] = textLeft;
  }

  if (textRight != "" && lastTextRight[line] != textRight) {
    ucg.setColor(0, 21, 21, 21);
    int lastRWidth = ucg.getStrWidth(lastTextRight[line].c_str());
    ucg.drawBox(winWidth - lastRWidth - LeftOfScreen, lTop - SmallFontHeight, lastRWidth, SmallFontHeight);
    ucg.setColor(221, 221, 221);


    int rWidth = ucg.getStrWidth(textRight.c_str());
    ucg.setPrintPos(winWidth - LeftOfScreen - rWidth, lTop);
    ucg.print(textRight);
    lastTextRight[line] = textRight;
  }
}

void printTitle(String text) {
  ucg.setFont(TitleFont);
  ucg.setColor(255, 255, 255);
  ucg.setPrintPos(2, getLineTop(0));

  ucg.print(text);
}

void _printGraphText(int line, String minStr, String maxStr) {
  ucg.setFont(TinyFont);
  ucg.setPrintPos(LeftOfScreen + 1, getLineTop(line) - (SmallFontHeight - TinyFontHeight));
  ucg.setColor(221, 221, 221);
  ucg.print(maxStr);
  ucg.setPrintPos(LeftOfScreen + 1, getLineTop(line + 1));
  ucg.print(minStr);
}

String lastGraphStrings[10][10];
void printGraphBg(int line, String minStr, String maxStr) {
  ucg.setColor(0, 39, 39, 39);
  ucg.drawBox(LeftOfScreen, getLineTop(line - 1) + LineSpacing, ucg.getWidth() - LeftOfScreen * 2, SmallFontHeight * 3 - LineSpacing * 2);

  _printGraphText(line, minStr, maxStr);
  lastGraphStrings[line][0] = minStr;
  lastGraphStrings[line][1] = maxStr;
}

int lastLinePositions[10] = { 0 };
int previousPixelY[10] = { -1, -1, -1, -1, -1, -1, -1, -1 , -1, -1 };
void printNextGraphPoint(int line, float percent) {
  if (line == 2) {
      Serial.println("================= ");
      Serial.println("data " + String(previousPixelY[line]) + " " + lastLinePositions[line] + " " + String(ucg.getWidth() - LeftOfScreen * 2));

  }
  int top = getLineTop(line - 1) + LineSpacing;
  int graphHeight = SmallFontHeight * 3 - LineSpacing * 2;
  int maxVal = top;
  int minVal = top + graphHeight;

  int x = lastLinePositions[line] + LeftOfScreen;
  int lastX = x - 1;
  if (x == LeftOfScreen)
    lastX = ucg.getWidth() - LeftOfScreen - 1;

  ++lastLinePositions[line];
  if (lastLinePositions[line] >= ucg.getWidth() - LeftOfScreen * 2)
    lastLinePositions[line] = 0;
  int y = int(top + graphHeight * (1 - percent));

  ucg.setColor(0, 39, 39, 39);
  // erase all vertical pixels on current x
  ucg.drawVLine(x, top, graphHeight);

  // clear last 'active' line
  if (previousPixelY[line] != -1) {
    if (line == 2) {
      Serial.println("clearing last line " + String(lastX));
    }
    ucg.drawVLine(lastX, top, graphHeight);

    ucg.setColor(221, 221, 221);
    ucg.drawPixel(lastX, previousPixelY[line]);
  }


  // draw current 'active' line
  ucg.setColor(237, 86, 21);
  int yHeight = LineSpacing * 2 + 1;

  if (y <= top + yHeight / 2) {
    y = max(y, top);
    yHeight = yHeight / 2 + (y - top);
    ucg.drawVLine(x, top, yHeight);
  }
  else if (y >= (top + graphHeight) - yHeight / 2) {
    y = min(y, top + graphHeight - 1);
    int drawOverAmount = (y - LineSpacing * 1 + yHeight) - (top + graphHeight);
    int drawY = y - (yHeight / 2 - 0.5);
    ucg.drawVLine(x, drawY, yHeight - drawOverAmount);
  } else {
    ucg.drawVLine(x, y - LineSpacing * 1, yHeight);
  }

  previousPixelY[line] = y;
  _printGraphText(line, lastGraphStrings[line][0], lastGraphStrings[line][1]);
}

void updateDisplay(bool bg, bool title, bool text) {
  resetBackground = bg;
  drawBackground();
}

#else





#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735

#define TFT_CS        10
#define TFT_RST        8 
#define TFT_DC         9
#define TFT_MOSI 11 
#define TFT_SCLK 12

#define T 4000
#define DLY() delay(2000)

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

bool resetBackground = true;


#define TopOfScreen 5
#define LeftOfScreen 5



#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/Tiny3x3a2pt7b.h>
#include <Fonts/Org_01.h>
#include <Fonts/FreeMono9pt7b.h>

#include "FreeSans17px.h"
#include "FreeSans11px.h"
#include "FreeSans7px.h"

#define TitleFont &FreeSans17px // 17px tall
#define TitleFontHeight 17
#define TitleFontScale 1

#define SmallFont &FreeSans11px // 11px tall
#define SmallFontHeight 13
#define SmallFontScale 1

#define TinyFont &Org_01 // 7px tall
#define TinyFontHeight 5
#define TinyFontScale 1

#define LineSpacing 4

#define GRAY_0   0x0000 // 0, 0, 0
#define GRAY_100 0x10A2 // 21, 21, 21
#define GRAY_200 0x39E7 // 60
#define GRAY_300 0x630C // 99
#define GRAY_400 0x8C51 // 138
#define GRAY_500 0xB596 // 177
#define GRAY_600 0xDEDB // 216
#define GRAY_700 0xFFFF // 255
#define ORANGE_200 0xEAA2 // 237, 86, 21


void setupDisplay() {
  tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab
  tft.setRotation(1); // 270 degress
  tft.fillScreen(GRAY_100);
  //ucg.setPrintDir(0);
}

void drawBackground() {
  if (!resetBackground) {
    return;
  }
  tft.fillRect(0, 0, tft.width(), tft.height(), GRAY_100);
  resetBackground = false;
}

void clearTitle() {
  tft.fillRect(0, 0, tft.width(), TopOfScreen + TitleFontHeight, GRAY_100);
}

int getLineTop(int line) {
  if (line == 0) { // title
    return TopOfScreen + TitleFontHeight;
  } else {
    return TopOfScreen + TitleFontHeight + line * LineSpacing + (line) * SmallFontHeight;
  }
}

int getStringWidth(String str) {
  int16_t  x1, y1;
  uint16_t w, h;
  tft.getTextBounds(str, 0, 0, &x1, &y1, &w, &h);
  return w;
}

String lastTextLeft[10];
String lastTextRight[10];
void printText(int line, String textLeft, String textRight) {
  tft.setFont(SmallFont);
  tft.setTextColor(GRAY_600);
  tft.setCursor(2, getLineTop(line));
  int lTop = getLineTop(line);
  int winWidth = tft.width();

  if (textLeft != lastTextLeft[line]) {
    tft.print(textLeft);
    lastTextLeft[line] = textLeft;
  }

  if (textRight != "" && lastTextRight[line] != textRight) {
    int lastRWidth = getStringWidth(lastTextRight[line].c_str());
    if (line == 1) {
      Serial.println("Last Right width " + String(lastRWidth) + " clearing (" + String(winWidth - lastRWidth - LeftOfScreen) + ", " + String(lTop - SmallFontHeight) + ", " + String(lastRWidth) + ")");
    }
    tft.fillRect(winWidth - lastRWidth - LeftOfScreen, lTop - SmallFontHeight, lastRWidth, SmallFontHeight, GRAY_100);
    
    int rWidth = getStringWidth(textRight.c_str());
    if (line == 1) {
      Serial.println("Right width " + String(rWidth) + " printing to (" + String(winWidth - LeftOfScreen - rWidth) + ", " + String(lTop) + ")");
    }
    tft.setCursor(winWidth - LeftOfScreen - rWidth, lTop);
    tft.print(textRight);
    lastTextRight[line] = textRight;
  }
}

void printTitle(String text) {
  tft.setFont(TitleFont);
  tft.setTextColor(GRAY_600);
  tft.setCursor(2, getLineTop(0));

  tft.print(text);
}

void _printGraphText(int line, String minStr, String maxStr) {
  tft.setFont(TinyFont);
  tft.setTextColor(GRAY_600);
  tft.setCursor(LeftOfScreen + 1, getLineTop(line) - (SmallFontHeight - TinyFontHeight));
  
  tft.print(maxStr);
  tft.setCursor(LeftOfScreen + 1, getLineTop(line + 1));
  tft.print(minStr);
}

String lastGraphStrings[10][10];
void printGraphBg(int line, String minStr, String maxStr) {
  tft.fillRect(LeftOfScreen, getLineTop(line - 1) + LineSpacing, tft.width() - LeftOfScreen * 2, SmallFontHeight * 3 - LineSpacing * 2, GRAY_200);

  _printGraphText(line, minStr, maxStr);
  lastGraphStrings[line][0] = minStr;
  lastGraphStrings[line][1] = maxStr;
}

int lastLinePositions[10] = { 0 };
int previousPixelY[10] = { -1, -1, -1, -1, -1, -1, -1, -1 , -1, -1 };
void printNextGraphPoint(int line, float percent) {
  if (line == 2 && DEBUG) {
      Serial.println("================= ");
      Serial.println("data " + String(previousPixelY[line]) + " " + lastLinePositions[line] + " " + String(tft.width() - LeftOfScreen * 2));
  }
  int top = getLineTop(line - 1) + LineSpacing;
  int graphHeight = SmallFontHeight * 3 - LineSpacing * 2;
  int maxVal = top;
  int minVal = top + graphHeight;

  int x = lastLinePositions[line] + LeftOfScreen;
  int lastX = x - 1;
  if (x == LeftOfScreen)
    lastX = tft.width() - LeftOfScreen - 1;

  ++lastLinePositions[line];
  if (lastLinePositions[line] >= tft.width() - LeftOfScreen * 2)
    lastLinePositions[line] = 0;
  int y = int(top + graphHeight * (1 - percent));

  // erase all vertical pixels on current x
  tft.drawFastVLine(x, top, graphHeight, GRAY_200);

  // clear last 'active' line
  if (previousPixelY[line] != -1) {
    if (line == 2 && DEBUG) {
      Serial.println("clearing last line " + String(lastX));
    }
    tft.drawFastVLine(lastX, top, graphHeight, GRAY_200);

    tft.drawPixel(lastX, previousPixelY[line], GRAY_600);
  }


  // draw current 'active' line
  int yHeight = LineSpacing * 2 + 1;

  if (y <= top + yHeight / 2) {
    y = max(y, top);
    yHeight = yHeight / 2 + (y - top);
    tft.drawFastVLine(x, top, yHeight, ORANGE_200);
  }
  else if (y >= (top + graphHeight) - yHeight / 2) {
    y = min(y, top + graphHeight - 1);
    int drawOverAmount = (y - LineSpacing * 1 + yHeight) - (top + graphHeight);
    int drawY = y - (yHeight / 2 - 0.5);
    tft.drawFastVLine(x, drawY, yHeight - drawOverAmount, ORANGE_200);
  } else {
    tft.drawFastVLine(x, y - LineSpacing * 1, yHeight, ORANGE_200);
  }

  previousPixelY[line] = y;
  _printGraphText(line, lastGraphStrings[line][0], lastGraphStrings[line][1]);
}

void updateDisplay(bool bg, bool title, bool text) {
  resetBackground = bg;
  drawBackground();
}



#endif
