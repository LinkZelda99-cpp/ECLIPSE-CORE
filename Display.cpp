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
//
// Use the Arduino Graphics drawing API instead of renderBitmap.
// This avoids the renderBitmap/loadPixels path and gives us one
// consistent matrix-rendering path for every app.
//

void showMatrix(uint8_t frame[8][12]) {
  matrix.beginDraw();

  // Start from a completely blank frame.
  matrix.clear();

  // UNO R4 WiFi matrix is monochrome.
  matrix.stroke(0xFFFFFFFF);

  // Draw every lit pixel.
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 12; x++) {
      if (frame[y][x] != 0) {
        matrix.point(x, y);
      }
    }
  }

  matrix.endDraw();
}

void clearMatrix() {
  matrix.beginDraw();
  matrix.clear();
  matrix.endDraw();
}

void showEclipseLogo() {
  showMatrix(eclipseLogo);
}
