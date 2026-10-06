#include "Display.h"
#include <string.h>

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
// The UNO R4 WiFi matrix is a 12x8 charlieplexed display.
//
// Arduino_LED_Matrix normally refreshes all 96 LEDs from an FspTimer.
// ECLIPSE CORE deliberately does not use matrix.begin()/renderBitmap()
// because that timer-based path was leaving this project with a blank
// matrix. Instead, we use the library's public matrix.on()/off()
// functions and perform the 96-LED multiplexing ourselves.
//
// One LED is selected every 100 us:
//   96 LEDs * 100 us = 9.6 ms/frame ~= 104 Hz.
//
// This is fast enough to look continuously illuminated while avoiding
// the library's FspTimer dependency.

void matrixService() {
  unsigned long now = micros();

  if ((unsigned long)(now - matrixLastScanMicros) < MATRIX_SCAN_INTERVAL_US) {
    return;
  }

  matrixLastScanMicros = now;

  uint8_t index = matrixScanIndex;
  uint8_t y = index / 12;
  uint8_t x = index % 12;

  if (matrixFrame[y][x]) {
    matrix.on(index);
  } else {
    matrix.off(index);
  }

  matrixScanIndex = (index + 1) % 96;
}

void matrixDelay(unsigned long milliseconds) {
  unsigned long start = millis();

  while ((unsigned long)(millis() - start) < milliseconds) {
    matrixService();
  }
}

void showMatrix(uint8_t frame[8][12]) {
  memcpy(matrixFrame, frame, sizeof(matrixFrame));
}

void clearMatrix() {
  memset(matrixFrame, 0, sizeof(matrixFrame));
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
        if (digitFont[digits[digit]][y] & (1 << (2 - x))) {
          frame[y + 1][x0 + x] = 1;
        }
      }
    }
  }

  uint8_t pulse = (millis() / 140) % 12;
  frame[7][pulse] = 1;

  showMatrix(frame);
}
