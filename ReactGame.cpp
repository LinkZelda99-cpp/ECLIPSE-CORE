#include "ReactGame.h"

uint16_t reactionScore() {
  if (reactTooEarly) return 0;
  if (reactTime == 0) return 0;
  if (reactTime >= 1000) return 1;

  return 1000 - reactTime;
}

void startReactGame() {
  reactTooEarly = false;
  reactTime = 0;
  reactDelay = random(1500, 4500);
  reactStarted = millis();
  state = STATE_REACT_WAIT;

  drawLCD("ECLIPSE REACT", "WAIT...");
  showMatrixNumber(0);
  rgbPurple();

  clearEncoderEvents();
}

void updateReactWait() {
  if (consumeEncoderPress()) {
    reactTooEarly = true;
    soundFailure();
    state = STATE_REACT_RESULT;
    return;
  }

  if (millis() - reactStarted >= reactDelay) {
    state = STATE_REACT_READY;
    reactStarted = millis();

    drawLCD("GO!", "PRESS NOW!");
    showMatrixNumber(0);
    rgbGreen();

    tone(PIN_BUZZER, 1800, 80);
  }

  // The score stays visible while the system waits.
  showMatrixNumber(reactionScore());
}

void updateReactReady() {
  showMatrixNumber(reactionScore());

  if (consumeEncoderPress()) {
    reactTime = millis() - reactStarted;
    soundSuccess();
    rgbBlue();
    state = STATE_REACT_RESULT;
  }
}

void updateReactResult() {
  if (reactTooEarly) {
    drawLCD("TOO EARLY!", "SCORE 0");
    showMatrixNumber(0);
    rgbRed();
  } else {
    drawLCD("REACTION TIME", String(reactTime) + " ms");
    showMatrixNumber(reactionScore());
    rgbBlue();
  }

  if (consumeEncoderPress()) {
    returnToGamesMenu();
  }
}
