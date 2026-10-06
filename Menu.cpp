#include "Menu.h"

void handleBackButton() {
  switch (state) {
    case STATE_MENU:
      break;

    case STATE_GAMES_MENU:
      returnToMainMenu();
      break;

    case STATE_LIGHT:
    case STATE_DISTANCE:
    case STATE_SCORES:
    case STATE_CORE_INFO:
      returnToMainMenu();
      break;

    default:
      buzzerOff();
      returnToGamesMenu();
      break;
  }
}

void drawMainMenu() {
  if (mainMenuIndex == lastMainMenuIndex) return;

  lastMainMenuIndex = mainMenuIndex;
  drawLCD("ECLIPSE CORE", "> " + String(mainMenuItems[mainMenuIndex]));

  // Menus intentionally use the logo. Games/features replace it
  // immediately when their own update function runs.
  showEclipseLogo();
  rgbPurple();
}

void updateMainMenu() {
  int d = consumeEncoderDelta();

  if (d) {
    mainMenuIndex += d > 0 ? 1 : -1;

    if (mainMenuIndex < 0) mainMenuIndex = MAIN_MENU_COUNT - 1;
    if (mainMenuIndex >= MAIN_MENU_COUNT) mainMenuIndex = 0;

    soundNavigate();

    // The matrix reacts immediately to menu navigation.
    // A different menu item gets a different visual pulse.
    uint8_t frame[8][12] = {};
    uint8_t offset = (mainMenuIndex * 2) % 8;

    for (int y = 0; y < 8; y++) {
      frame[y][offset] = 1;
      frame[y][11 - offset] = 1;
    }

    showMatrix(frame);
  }

  drawMainMenu();

  if (consumeEncoderPress()) {
    soundSelect();

    switch (mainMenuIndex) {
      case 0: state = STATE_GAMES_MENU; lastGameMenuIndex = -1; break;
      case 1: state = STATE_LIGHT; break;
      case 2: state = STATE_DISTANCE; break;
      case 3: state = STATE_SCORES; break;
      case 4: state = STATE_CORE_INFO; break;
    }

    invalidateLCD();
    lcd.clear();
  }
}

void drawGamesMenu() {
  if (gameMenuIndex == lastGameMenuIndex) return;

  lastGameMenuIndex = gameMenuIndex;
  drawLCD("GAMES", "> " + String(gameMenuItems[gameMenuIndex]));

  showEclipseLogo();
  rgbPurple();
}

void updateGamesMenu() {
  int d = consumeEncoderDelta();

  if (d) {
    gameMenuIndex += d > 0 ? 1 : -1;

    if (gameMenuIndex < 0) gameMenuIndex = GAME_MENU_COUNT - 1;
    if (gameMenuIndex >= GAME_MENU_COUNT) gameMenuIndex = 0;

    soundNavigate();

    // Navigation gives immediate matrix feedback.
    uint8_t frame[8][12] = {};
    uint8_t y = (gameMenuIndex * 2) % 8;

    for (int x = 0; x < 12; x++) {
      frame[y][x] = 1;
    }

    showMatrix(frame);
  }

  drawGamesMenu();

  if (consumeEncoderPress()) {
    soundSelect();

    switch (gameMenuIndex) {
      case 0: startCodeGame(); break;
      case 1: startReactGame(); break;
      case 2: startMemoryGame(); break;
      case 3: startSnakeGame(); break;
    }

    invalidateLCD();
    lcd.clear();
  }
}
