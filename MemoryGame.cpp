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
  drawLCD("MEMORY", "LEVEL " + String(memoryLevel));

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

  // Between sequence flashes, the matrix still carries the score.
  // This makes the two displays feel synchronized.
  if (millis() - memoryTimer < 250) {
    showMemorySymbol(memorySequence[memoryShowPosition == 0 ? 0 : memoryShowPosition - 1]);
  }
}

void drawMemoryInput() {
  drawLCD(
    "MEMORY",
    "PICK " + String(memoryChoice + 1) + " " +
    String(memoryInputPosition + 1) + "/" + String(memoryLevel)
  );

  // LCD carries the interaction; matrix carries the live score.
  showMatrixNumber(memoryScore());

  // Add a small indicator for the currently selected quadrant.
  uint8_t f[8][12] = {};
  uint8_t q = memoryChoice;

  int x0 = (q % 2 == 0) ? 1 : 7;
  int y0 = (q < 2) ? 1 : 5;

  for (int y = y0; y < y0 + 3 && y < 8; y++)
    for (int x = x0; x < x0 + 4; x++)
      f[y][x] = 1;

  // Keep a single pixel moving to show active encoder state.
  uint8_t p = (millis() / 120) % 4;
  f[y0][x0 + p] = 1;

  // The selected quadrant is intentionally the primary matrix visual.
  // Score is shown on LCD for the interactive matrix game state.
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
    drawLCD("MEMORY MASTER", "SCORE " + String(memoryScore()));
    showMatrixNumber(memoryScore());
    rgbGreen();
  } else {
    drawLCD("MEMORY FAILED", "SCORE " + String(memoryScore()));
    showMatrixNumber(memoryScore());
    rgbRed();
  }

  if (consumeEncoderPress()) {
    returnToGamesMenu();
  }
}
