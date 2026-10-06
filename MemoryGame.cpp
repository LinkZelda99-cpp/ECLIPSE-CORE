#include "MemoryGame.h"

uint16_t memoryScore() {
  return memoryLevel > 0 ? memoryLevel - 1 : 0;
}

void startMemoryGame() {
  for (int i = 0; i < MEMORY_MAX_LEVEL; i++) {
    memorySequence[i] = random(0, 4);
  }

  memoryLevel = 1;
  memoryShowPosition = 0;
  memoryInputPosition = 0;
  memoryChoice = 0;
  memoryWon = false;
  memoryTimer = millis();
  state = STATE_MEMORY_SHOW;
  rgbPurple();
  clearEncoderEvents();
}

void showMemorySymbol(uint8_t s) {
  uint8_t f[8][12] = {};

  switch (s) {
    case 0:
      for (int y = 1; y < 4; y++)
        for (int x = 1; x < 5; x++) f[y][x] = 1;
      break;

    case 1:
      for (int y = 1; y < 4; y++)
        for (int x = 7; x < 11; x++) f[y][x] = 1;
      break;

    case 2:
      for (int y = 5; y < 8; y++)
        for (int x = 1; x < 5; x++) f[y][x] = 1;
      break;

    case 3:
      for (int y = 5; y < 8; y++)
        for (int x = 7; x < 11; x++) f[y][x] = 1;
      break;
  }

  showMatrix(f);
}

void updateMemoryShow() {
  // MEMORY is a matrix-first game, so the LCD becomes the score HUD.
  drawLCD("MEMORY SCORE", String(memoryScore()));

  if (memoryShowPosition >= memoryLevel) {
    memoryInputPosition = 0;
    memoryChoice = 0;
    state = STATE_MEMORY_INPUT;
    clearMatrix();
    return;
  }

  if (millis() - memoryTimer >= 650) {
    uint8_t s = memorySequence[memoryShowPosition];

    showMemorySymbol(s);
    tone(PIN_BUZZER, 700 + s * 250, 100);

    memoryShowPosition++;
    memoryTimer = millis();
  }

  // Keep the last symbol visible briefly instead of leaving the
  // matrix blank between sequence steps.
  if (memoryShowPosition > 0 &&
      memoryShowPosition <= memoryLevel &&
      millis() - memoryTimer < 250) {
    showMemorySymbol(memorySequence[memoryShowPosition - 1]);
  }
}

void drawMemoryInput() {
  // Matrix is the game. LCD is the live numeric score HUD.
  drawLCD("MEMORY SCORE", String(memoryScore()));

  uint8_t f[8][12] = {};
  uint8_t q = memoryChoice;

  int x0 = (q % 2 == 0) ? 1 : 7;
  int y0 = (q < 2) ? 1 : 5;

  for (int y = y0; y < y0 + 3 && y < 8; y++) {
    for (int x = x0; x < x0 + 4; x++) {
      f[y][x] = 1;
    }
  }

  // Animate the selected quadrant so the matrix visibly responds
  // to the encoder even before the player presses the button.
  uint8_t p = (millis() / 120) % 4;
  f[y0][x0 + p] = 1;

  showMatrix(f);
}

void updateMemoryInput() {
  drawMemoryInput();

  int d = consumeEncoderDelta();

  if (d) {
    memoryChoice += d;

    while (memoryChoice < 0) memoryChoice += 4;
    while (memoryChoice >= 4) memoryChoice -= 4;

    soundNavigate();
  }

  if (consumeEncoderPress()) {
    uint8_t expected = memorySequence[memoryInputPosition];

    if (memoryChoice != expected) {
      memoryWon = false;
      state = STATE_MEMORY_RESULT;
      soundFailure();
      rgbRed();
      return;
    }

    soundSelect();
    memoryInputPosition++;

    if (memoryInputPosition >= memoryLevel) {
      if (memoryLevel >= MEMORY_MAX_LEVEL) {
        memoryWon = true;
        state = STATE_MEMORY_RESULT;
        soundSuccess();
        rgbGreen();
        return;
      }

      memoryLevel++;
      memoryShowPosition = 0;
      memoryInputPosition = 0;
      memoryChoice = 0;
      memoryTimer = millis();
      state = STATE_MEMORY_SHOW;
      soundSuccess();
    }
  }
}

void updateMemoryResult() {
  if (memoryWon) {
    drawLCD("MEMORY MASTER", String(memoryScore()));
    showMatrixNumber(memoryScore());
    rgbGreen();
  } else {
    drawLCD("MEMORY FAILED", String(memoryScore()));
    showMatrixNumber(memoryScore());
    rgbRed();
  }

  if (consumeEncoderPress()) {
    returnToGamesMenu();
  }
}
