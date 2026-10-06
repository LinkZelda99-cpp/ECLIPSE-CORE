#include "CoreFeatures.h"

void updateLightSensor() {
  if (millis() - lastLightRead < 100) {
    return;
  }

  lastLightRead = millis();
  lightReading = analogRead(PIN_LIGHT);
}

long readUltrasonic() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);

  digitalWrite(PIN_TRIG, LOW);

  unsigned long duration = pulseIn(PIN_ECHO, HIGH, 18000UL);

  if (!duration) {
    return -1;
  }

  long cm = duration / 58;

  if (cm < 2 || cm > 300) {
    return -1;
  }

  return cm;
}

void updateDistanceSensor() {
  if (millis() - lastDistanceRead < 150) {
    return;
  }

  lastDistanceRead = millis();
  distanceReading = readUltrasonic();
}

void returnToMainMenu() {
  state = STATE_MENU;
  lastMainMenuIndex = -1;

  invalidateLCD();
  lcd.clear();
  showEclipseLogo();
  rgbPurple();

  clearEncoderEvents();
}

void returnToGamesMenu() {
  state = STATE_GAMES_MENU;
  lastGameMenuIndex = -1;

  invalidateLCD();
  lcd.clear();

  clearEncoderEvents();
}

void updateLightApp() {
  updateLightSensor();

  int percent = map(lightReading, 0, 1023, 0, 100);

  drawLCD(
    "LIGHT",
    String(percent) + "% " + String(lightReading)
  );

  uint8_t frame[8][12] = {};

  int bars = map(lightReading, 0, 1023, 0, 12);
  bars = constrain(bars, 0, 12);

  for (int x = 0; x < bars; x++) {
    for (int y = 3; y < 8; y++) {
      frame[y][x] = 1;
    }
  }

  showMatrix(frame);
  rgbBlue();

  if (consumeEncoderPress()) {
    returnToMainMenu();
  }
}

void updateDistanceApp() {
  updateDistanceSensor();

  if (distanceReading < 0) {
    drawLCD("DISTANCE", "NO ECHO");
  } else {
    drawLCD(
      "DISTANCE",
      String(distanceReading) + " cm"
    );
  }

  uint8_t frame[8][12] = {};

  if (distanceReading < 0) {
    for (int i = 0; i < 8; i++) {
      frame[i][i] = 1;
      frame[i][11 - i] = 1;
    }
  } else {
    int bars = map(
      constrain(distanceReading, 2L, 100L),
      100,
      2,
      1,
      12
    );

    bars = constrain(bars, 1, 12);

    for (int x = 0; x < bars; x++) {
      for (int y = 2; y < 6; y++) {
        frame[y][x] = 1;
      }
    }
  }

  showMatrix(frame);
  rgbBlue();

  if (consumeEncoderPress()) {
    returnToMainMenu();
  }
}

void updateScoresApp() {
  drawLCD(
    "HIGH SCORE",
    String(highScore) + " POINTS"
  );

  uint8_t frame[8][12] = {};

  int bars = highScore % 13;

  if (highScore > 0 && bars == 0) {
    bars = 12;
  }

  for (int x = 0; x < bars; x++) {
    for (int y = 4; y < 8; y++) {
      frame[y][x] = 1;
    }
  }

  showMatrix(frame);
  rgbPurple();

  if (consumeEncoderPress()) {
    returnToMainMenu();
  }
}

void updateCoreInfo() {
  drawLCD("CORE ONLINE", "v2.2 R4 WiFi");
  showEclipseLogo();
  rgbPurple();

  if (consumeEncoderPress()) {
    returnToMainMenu();
  }
}
