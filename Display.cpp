#include "Display.h"
#include "AppState.h"
#define DEBUG false

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735

#define TFT_CS        10
#define TFT_RST        8
#define TFT_DC         9
#define TFT_MOSI 11
#define TFT_SCLK 12

#define T 4000
#define DLY() delay(2000)

// 128×160px
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

bool resetBackground = true;

#define TopOfScreen 5
#define LeftOfScreen 5
#define RightOfScreen LeftOfScreen

/*
   HelvB10 -
    1. get Helvb10.bdf from ucglib source
    2. run through binFontsTool (https://forum.arduino.cc/index.php?topic=447956.0) to convert to c code (.h file)
    3. add to project, view, and adjust as needed with correctFontY()
*/
#include "HelvB10.h"
/*
   Conversion of ucgLib 5x7.bdf using binFontsTool
*/
#include "TinyFont.h"

/*
    1. use ucglib: ./do_fontsize_a_v2.sh 22 ../ttf/fu/FreeUniversal-Bold.ttf fubtest
    2. run through binFontsTool
    3. adjust with correctFontY()
*/
#include "TitleFont.h"

void correctFontY(int yDelta, int numGlyphs, GFXglyph *glyphs) {
  int dY = 0;
  for (int i = 0; i < numGlyphs; ++i) {
    // bring down dY by x pixels
    dY = glyphs[i].yOffset;
    if (dY < -(yDelta + 1)) {
      glyphs[i].yOffset = dY + yDelta;
    }
  }
}

#define TitleFont &FreeUniversal // 15px tall
#define TitleFontHeight 15
#define TitleFontScale 1

#define SmallFont &Helvetica_Bold // 11px tall
#define SmallFontHeight 13
#define SmallFontScale 1

#define TinyFont &FixedTiny // 7px tall
#define TinyFontHeight 7
#define TinyFontScale 1

#define LineSpacing 4

// last printed text in SmallFont
String lastTextLeft[10];
String lastTextRight[10];

// last printed fancy text in SmallFont
String lastFancyTextLeft[10][10];
String lastFancyTextRight[10][10];
uint16_t lastFancyTextLeftColors[10][10];
uint16_t lastFancyTextRightColors[10][10];

// last strings printed on the graph
String lastGraphStrings[10][10];

void setupDisplay() {
  correctFontY(7, Helvetica_Bold.last - Helvetica_Bold.first, Helvetica_Bold.glyph);
  correctFontY(5, FreeUniversal.last - FreeUniversal.first, FreeUniversal.glyph);
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

  // reset lastText lines
  for (int i = 0; i < 10; ++i) {
    lastTextLeft[i] = "";
    lastTextRight[i] = "";

    for (int j = 0; j < 10; ++j) {
      lastGraphStrings[i][j] = "";
      lastFancyTextLeft[i][j] = "";
      lastFancyTextRight[i][j] = "";
      lastFancyTextLeftColors[i][j] = 0;
      lastFancyTextRightColors[i][j] = 0;
    }
  }

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
  if (w > 200)
    w = 0;
  return w;
}

// TODO: Can use GFXcanvas1 to draw in memory and blt to the screen for flicker-free text changes.
// see https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts
void printText(int line, String textLeft, String textRight) {
  tft.setFont(SmallFont);
  tft.setTextColor(GRAY_600);
  tft.setCursor(LeftOfScreen, getLineTop(line));
  int lTop = getLineTop(line);
  int winWidth = tft.width();

  // Clear all First
  if (textLeft != lastTextLeft[line]) {
    int lastLWidth = getStringWidth(lastTextLeft[line]);
    tft.fillRect(2, lTop - SmallFontHeight, lastLWidth, SmallFontHeight, GRAY_100);
  }
  if (textRight != "" && lastTextRight[line] != textRight) {
    int lastRWidth = getStringWidth(lastTextRight[line]);
    tft.fillRect(winWidth - lastRWidth - LeftOfScreen, lTop - SmallFontHeight, lastRWidth, SmallFontHeight, GRAY_100);
  }

  // Print all second
  if (textLeft != lastTextLeft[line]) {
    tft.print(textLeft);
    lastTextLeft[line] = textLeft;
  }

  if (textRight != "" && lastTextRight[line] != textRight) {
    int rWidth = getStringWidth(textRight);
    tft.setCursor(winWidth - LeftOfScreen - rWidth, lTop);
    tft.print(textRight);
    lastTextRight[line] = textRight;
  }
}

void printTextFancy(int line, String textLeft[], uint16_t leftColors[], int leftLen, String textRight[], uint16_t rightColors[], int rightLen) {
  tft.setFont(SmallFont);
  int lTop = getLineTop(line);
  int winWidth = tft.width();
  if (leftLen > 10) {
    Serial.println("TOO MANY LEFT STRINGS");
    leftLen = 10;
  }
  if (rightLen > 10) {
    Serial.println("TOO MANY RIGHT STRINGS");
    rightLen = 10;
  }

  // ====== Clear changed text first ======
  // on the left, start at zero and clear everything after the first change (to the right)
  String sumText = "";
  String *lastLeftEntries = lastFancyTextLeft[line];
  uint16_t *lastLeftColors = lastFancyTextLeftColors[line];
  int i, leftPos = LeftOfScreen, fillWidth;
  for(i = 0; i < leftLen; ++i) {
    if (lastLeftEntries[i] != textLeft[i] || lastLeftColors[i] != leftColors[i]) {
      break;
    }
    sumText += getStringWidth(lastLeftEntries[i]);
  }
  leftPos += getStringWidth(sumText);

  int leftStartIndex = i;
  sumText = "";
  if (leftStartIndex < leftLen) {
    for(i = leftStartIndex; i < leftLen; ++i)
      sumText += textLeft[i];
  }
  fillWidth = getStringWidth(sumText);
  if (fillWidth > 0) {
    tft.fillRect(leftPos, lTop - SmallFontHeight, fillWidth, SmallFontHeight, GRAY_100);
  }

  // on the right, start at (len - 1) and clear everything before the first change (to the left)
  sumText = "";
  String *lastRightEntries = lastFancyTextRight[line];
  uint16_t *lastRightColors = lastFancyTextRightColors[line];
  int rightPos = winWidth - RightOfScreen - 1;
  for(i = rightLen - 1; i >= 0; --i) {
    if (lastRightEntries[i] != textRight[i] || lastRightColors[i] != rightColors[i])
      break;
    sumText += lastRightEntries[i];
  }
  if (i > 0 && i < rightLen - 2 && rightLen > 1) {
    // Hack -- include " - " and " / " if there are changes
    sumText += lastRightEntries[i];
    ++i;
  }
  if (sumText != "") {
    rightPos -= RightOfScreen;
  }
  rightPos -= getStringWidth(sumText);

  sumText = "";
  int rightStartIndex = i;
  fillWidth = 0;
  if (rightStartIndex >= 0) {
    for(i = 0; i <= rightStartIndex; ++i) {
      sumText += textRight[i];
    }

  }
  fillWidth += getStringWidth(sumText);
  if (fillWidth > 0) {
    int clearCorrection = 15;
    tft.fillRect(rightPos - fillWidth - clearCorrection, lTop - SmallFontHeight, fillWidth + clearCorrection + 1, SmallFontHeight, GRAY_100);
  }

  // ====== Print all text ======
  // Left
  tft.setCursor(leftPos, lTop);
  for(i = leftStartIndex; i < leftLen; ++i) {
    tft.setTextColor(leftColors[i]);
    tft.print(textLeft[i]);
  }

  // Right
  tft.setCursor(rightPos - fillWidth, lTop);
  for(i = 0; i <= rightStartIndex; ++i) {
    tft.setTextColor(rightColors[i]);
    tft.print(textRight[i]);
  }

  // remember last text printed
  for(i = 0; i < leftLen; ++i) {
    lastFancyTextLeft[line][i] = textLeft[i];
    lastFancyTextLeftColors[line][i] = leftColors[i];
  }
  for(i = 0; i < rightLen; ++i) {
    lastFancyTextRight[line][i] = textRight[i];
    lastFancyTextRightColors[line][i] = rightColors[i];
  }
}

void printTitle(String text) {
  tft.setFont(TitleFont);
  tft.setTextColor(GRAY_600);
  tft.setCursor(2, getLineTop(0));

  tft.print(text);
}

String lastStatus = "";
void printTopRightStatus(String status) {
  if (lastStatus != status) {
      tft.setFont(TinyFont);
      tft.setTextColor(GRAY_600);
      int winWidth = tft.width();
      int textWidth = 30;


      tft.fillRect(winWidth - textWidth, 0 , textWidth + LeftOfScreen, TinyFontHeight + TopOfScreen, GRAY_100);
      int stringWidth = getStringWidth(status);
      tft.setCursor(winWidth - LeftOfScreen - stringWidth, TinyFontHeight + TopOfScreen);
      tft.print(status);

      lastStatus = status;
  }
}

void _printGraphText(int line, String minStr, String maxStr, String bottomRightStr) {
  tft.setFont(TinyFont);
  tft.setTextColor(GRAY_600);
  tft.setCursor(LeftOfScreen + 1, getLineTop(line) - (SmallFontHeight - TinyFontHeight));

  tft.print(maxStr);
  tft.setCursor(LeftOfScreen + 1, getLineTop(line + 1));
  tft.print(minStr);

  int winWidth = tft.width();
  if (lastGraphStrings[line][2] != bottomRightStr) {
    int stringWidth = getStringWidth(lastGraphStrings[line][2]);
    tft.fillRect(winWidth - LeftOfScreen - stringWidth - 2, getLineTop(line + 1) - SmallFontHeight, stringWidth + 1, SmallFontHeight, GRAY_200);
    lastGraphStrings[line][2] = bottomRightStr;
  }
  int rWidth = getStringWidth(bottomRightStr);
  tft.setCursor(winWidth - LeftOfScreen - rWidth - 2, getLineTop(line + 1));
  tft.print(bottomRightStr);
}

void printGraphBg(int line, String minStr, String maxStr, String bottomRightStr) {
  tft.fillRect(LeftOfScreen, getLineTop(line - 1) + LineSpacing, tft.width() - LeftOfScreen * 2, SmallFontHeight * 3 - LineSpacing * 2, GRAY_200);

  _printGraphText(line, minStr, maxStr, bottomRightStr);
  lastGraphStrings[line][0] = minStr;
  lastGraphStrings[line][1] = maxStr;
  lastGraphStrings[line][2] = bottomRightStr;
}

void setGraphBottomRightStr(int line, String bottomRightStr) {
  _printGraphText(line, lastGraphStrings[line][0], lastGraphStrings[line][1], bottomRightStr);
}

int lastLinePositions[10] = { 0 };
int previousPixelY[10] = { -1, -1, -1, -1, -1, -1, -1, -1 , -1, -1 };
void printNextGraphPoint(int line, float percent) {
  // if (line == 2) {
  //     Serial.println("================= ");
  //     Serial.println("data " + String(previousPixelY[line]) + " " + lastLinePositions[line] + " " + String(tft.width() - LeftOfScreen * 2));
  // }
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
  _printGraphText(line, lastGraphStrings[line][0], lastGraphStrings[line][1], lastGraphStrings[line][2]);
}

void updateDisplay(bool bg, bool title, bool text) {
  resetBackground = bg;
  drawBackground();
}
