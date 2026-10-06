#include "Display.h"

void invalidateLCD() {
  lcdCache0 = "";
  lcdCache1 = "";
}

String fitLCD(String text) {
  while (text.length() < 16) text += " ";
  if (text.length() > 16) text = text.substring(0, 16);
  return text;
}

void writeLCDLine(uint8_t row, String text) {
  text = fitLCD(text);

  if (row == 0) {
    if (text == lcdCache0) return;
    lcd.setCursor(0, 0);
    lcd.print(text);
    lcdCache0 = text;
  } else {
    if (text == lcdCache1) return;
    lcd.setCursor(0, 1);
    lcd.print(text);
    lcdCache1 = text;
  }
}

void drawLCD(String line0, String line1) {
  writeLCDLine(0, line0);
  writeLCDLine(1, line1);
}

// ============================================================
// LED MATRIX ENGINE
// ============================================================
// Keep one rendering path for the entire system. The UNO R4
// library's renderBitmap path is reliable on the installed R4
// core and lets every app replace the frame independently.

void showMatrix(uint8_t frame[8][12]) {
  matrix.renderBitmap(frame, 8, 12);
}

void clearMatrix() {
  uint8_t blank[8][12] = {};
  showMatrix(blank);
}

void showEclipseLogo() {
  showMatrix(eclipseLogo);
}

// Tiny 3x5 numeric font. The matrix is 12x8, so up to three
// digits fit comfortably while leaving room for a visual frame.
const uint8_t digitFont[10][5] = {
  {0b111, 0b101, 0b101, 0b101, 0b111},
  {0b010, 0b110, 0b010, 0b010, 0b111},
  {0b111, 0b001, 0b111, 0b100, 0b111},
  {0b111, 0b001, 0b111, 0b001, 0b111},
  {0b101, 0b101, 0b111, 0b001, 0b001},
  {0b111, 0b100, 0b111, 0b001, 0b111},
  {0b011, 0b100, 0b111, 0b101, 0b111},
  {0b111, 0b001, 0b010, 0b010, 0b010},
  {0b111, 0b101, 0b111, 0b101, 0b111},
  {0b111, 0b101, 0b111, 0b001, 0b110}
};

void showMatrixNumber(uint16_t value) {
  value = min(value, (uint16_t)999);

  uint8_t digits[3] = {
    (uint8_t)((value / 100) % 10),
    (uint8_t)((value / 10) % 10),
    (uint8_t)(value % 10)
  };

  uint8_t frame[8][12] = {};

  // Three digits, centered horizontally.
  for (uint8_t d = 0; d < 3; d++) {
    uint8_t x0 = 1 + d * 4;
    for (uint8_t y = 0; y < 5; y++) {
      for (uint8_t x = 0; x < 3; x++) {
        if (digitFont[digits[d]][y] & (1 << (2 - x))) {
          frame[y + 1][x0 + x] = 1;
        }
      }
    }
  }

  // A subtle moving underline makes the score feel alive instead
  // of like a static label.
  uint8_t pulse = (millis() / 140) % 12;
  frame[7][pulse] = 1;

  showMatrix(frame);
}
