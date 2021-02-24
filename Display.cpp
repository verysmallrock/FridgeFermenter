#include "Display.h"

#define T 4000
#define DLY() delay(2000)

Ucglib_ST7735_18x128x160_HWSPI ucg(/*cd=*/ D9, /*cs=*/ D10, /*reset=*/ D8);

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
