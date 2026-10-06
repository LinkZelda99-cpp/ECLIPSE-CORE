#include "Display.h"

namespace {
  // Persistent packed frame used by ArduinoLEDMatrix::loadFrame().
  // Keeping this buffer static avoids passing a temporary bitmap through
  // the library's renderBitmap macro.
  uint32_t matrixFrame[3] = {0, 0, 0};
}

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

// ---------------------------------------------------------------------------
// LED MATRIX ENGINE
// ---------------------------------------------------------------------------
// The UNO R4 WiFi's LED matrix is driven by ArduinoLEDMatrix.
// Instead of renderBitmap(), ECLIPSE CORE now uses the library's direct
// loadFrame() API with a persistent 96-bit frame buffer.
//
// Arduino's matrix library packs the 96 pixels as three 32-bit words,
// in row-major order. The library itself handles the electrical
// charlieplexing/bit reversal when the frame is refreshed.

void showMatrix(uint8_t frame[8][12]) {
  matrixFrame[0] = 0;
  matrixFrame[1] = 0;
  matrixFrame[2] = 0;

  for (uint8_t y = 0; y < 8; y++) {
    for (uint8_t x = 0; x < 12; x++) {
      matrixFrame[0] <<= 1;
      matrixFrame[1] <<= 1;
      matrixFrame[2] <<= 1;

      uint8_t pixel = frame[y][x] ? 1 : 0;

      if (y * 12 + x < 32) {
        matrixFrame[0] |= pixel;
      } else if (y * 12 + x < 64) {
        matrixFrame[1] |= pixel;
      } else {
        matrixFrame[2] |= pixel;
      }
    }
  }

  matrix.loadFrame(matrixFrame);
}

void clearMatrix() {
  const uint32_t blank[3] = {0, 0, 0};
  matrix.loadFrame(blank);
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

  uint8_t pulse = (millis() / 140) % 12;
  frame[7][pulse] = 1;

  showMatrix(frame);
}
