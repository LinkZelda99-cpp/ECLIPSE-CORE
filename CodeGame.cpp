#include "CodeGame.h"

void startCodeGame() {
  for (int i = 0; i < 4; i++) {
    codeSecret[i] = random(0, 10);
    codeGuess[i] = 0;
  }

  codePosition = 0;
  codeWon = false;
  state = STATE_CODE;
  rgbBlue();
  clearEncoderEvents();
}

uint16_t codeScore() {
  if (!codeWon) return 0;
  return 1000;
}

void drawCodeGame() {
  String l = "CODE ";
  for (int i = 0; i < 4; i++) l += String(codeGuess[i]);

  drawLCD(l, "DIGIT " + String(codePosition + 1) + "/4");
  showMatrixNumber(codeScore());
  rgbBlue();
}

void evaluateCode() {
  codeWon = true;

  for (int i = 0; i < 4; i++) {
    if (codeGuess[i] != codeSecret[i]) {
      codeWon = false;
      break;
    }
  }

  codeResultStarted = millis();
  state = STATE_CODE_RESULT;

  if (codeWon) {
    soundSuccess();
    rgbGreen();
  } else {
    soundFailure();
    rgbRed();
  }
}

void updateCodeGame() {
  drawCodeGame();

  int d = consumeEncoderDelta();

  if (d) {
    int v = codeGuess[codePosition] + d;

    while (v < 0) v += 10;
    while (v > 9) v -= 10;

    codeGuess[codePosition] = v;
    soundNavigate();
  }

  if (consumeEncoderPress()) {
    if (codePosition < 3) {
      codePosition++;
      soundSelect();
    } else {
      evaluateCode();
    }
  }
}

void updateCodeResult() {
  if (codeWon) {
    drawLCD("ACCESS GRANTED", "SCORE 1000");
    showMatrixNumber(1000);
    rgbGreen();
  } else {
    String a = String(codeSecret[0]) + String(codeSecret[1]) +
               String(codeSecret[2]) + String(codeSecret[3]);

    drawLCD("ACCESS DENIED", a);
    showMatrixNumber(0);
    rgbRed();
  }

  if (millis() - codeResultStarted >= 1800) {
    returnToGamesMenu();
  }
}
