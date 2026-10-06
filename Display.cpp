#include "Display.h"

void invalidateLCD() {
  lcdCache0 = "";
  lcdCache1 = "";
}

String fitLCD(String text) {
  while (text.length() < 16) {
    text += " ";
  }

  if (text.length() > 16) {
    text = text.substring(0, 16);
  }

  return text;
}

void writeLCDLine(uint8_t row, String text) {
  text = fitLCD(text);

  if (row == 0) {
    if (text == lcdCache0) {
      return;
    }

    lcd.setCursor(0, 0);
    lcd.print(text);
    lcdCache0 = text;
  } else {
    if (text == lcdCache1) {
      return;
    }

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
// LED MATRIX
// ============================================================
// The UNO R4 WiFi matrix accepts a 96-bit frame as three
// uint32_t words. Bit 31 of word 0 is pixel 0, then the bits
// continue left-to-right, top-to-bottom.
//
// This matches the Arduino_LED_Matrix library's loadPixels()
// packing behavior, but avoids the renderBitmap macro entirely.

void showMatrix(uint8_t frame[8][12]) {
  uint32_t packed[3] = {0, 0, 0};

  for (uint8_t y = 0; y < 8; y++) {
    for (uint8_t x = 0; x < 12; x++) {

      if (frame[y][x] == 0) {
        continue;
      }

      uint8_t pixelIndex = y * 12 + x;
      uint8_t wordIndex = pixelIndex / 32;
      uint8_t bitIndex = pixelIndex % 32;

      packed[wordIndex] |=
        (uint32_t)1 << (31 - bitIndex);
    }
  }

  matrix.loadFrame(packed);
}

void clearMatrix() {
  const uint32_t blank[3] = {
    0x00000000UL,
    0x00000000UL,
    0x00000000UL
  };

  matrix.loadFrame(blank);
}

void showEclipseLogo() {
  showMatrix(eclipseLogo);
}

// ============================================================
// SCORE FONT
// ============================================================

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
  if (value > 999) {
    value = 999;
  }

  uint8_t digits[3] = {
    (uint8_t)((value / 100) % 10),
    (uint8_t)((value / 10) % 10),
    (uint8_t)(value % 10)
  };

  uint8_t frame[8][12] = {};

  for (uint8_t digit = 0; digit < 3; digit++) {
    uint8_t x0 = 1 + digit * 4;

    for (uint8_t y = 0; y < 5; y++) {
      for (uint8_t x = 0; x < 3; x++) {

        if (
          digitFont[digits[digit]][y] &
          (1 << (2 - x))
        ) {
          frame[y + 1][x0 + x] = 1;
        }
      }
    }
  }

  // Animated underline keeps numeric screens visually active.
  uint8_t pulse = (millis() / 140) % 12;
  frame[7][pulse] = 1;

  showMatrix(frame);
}
