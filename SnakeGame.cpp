#include "SnakeGame.h"

void placeApple() {
  if (snakeLength >= SNAKE_MAX_LENGTH) {
    return;
  }

  bool valid = false;

  while (!valid) {
    appleX = random(0, SNAKE_WIDTH);
    appleY = random(0, SNAKE_HEIGHT);
    valid = true;

    for (uint8_t i = 0; i < snakeLength; i++) {
      if (snake[i].x == appleX && snake[i].y == appleY) {
        valid = false;
        break;
      }
    }
  }
}

bool snakeHitsBody(int8_t x, int8_t y, bool growing) {
  uint8_t checkLength = snakeLength;

  // When the snake is not growing, its tail moves away on this step,
  // so moving into the current tail cell is legal.
  if (!growing && checkLength > 0) {
    checkLength--;
  }

  for (uint8_t i = 0; i < checkLength; i++) {
    if (snake[i].x == x && snake[i].y == y) {
      return true;
    }
  }

  return false;
}

void resetSnake() {
  snakeLength = 3;
  snakeScore = 0;
  snakeDirection = 1;

  snake[0] = {6, 4};
  snake[1] = {5, 4};
  snake[2] = {4, 4};

  appleX = 9;
  appleY = 4;
  placeApple();

  lastSnakeMove = millis();
  lastAppleBlink = millis();
  appleVisible = true;

  buzzerOff();
  rgbGreen();
  clearEncoderEvents();
}

void startSnakeGame() {
  resetSnake();
  state = STATE_SNAKE_READY;
  invalidateLCD();
  lcd.clear();
}

void drawSnakeReady() {
  drawLCD("SNAKE", "PRESS TO START");

  uint8_t frame[8][12] = {};

  // Small snake icon.
  frame[4][3] = 1;
  frame[4][4] = 1;
  frame[4][5] = 1;
  frame[3][5] = 1;

  // Apple.
  frame[2][9] = 1;

  showMatrix(frame);
  rgbGreen();
}

void drawSnake() {
  uint8_t frame[8][12] = {};

  // Draw the snake body.
  for (uint8_t i = 0; i < snakeLength; i++) {
    frame[snake[i].y][snake[i].x] = 1;
  }

  // The apple blinks independently of the snake.
  if (appleVisible) {
    frame[appleY][appleX] = 1;
  }

  showMatrix(frame);

  drawLCD(
    "SNAKE",
    "SCORE: " + String(snakeScore)
  );

  rgbGreen();
}

void turnSnake(int turn) {
  int newDirection = snakeDirection + turn;

  if (newDirection < 0) {
    newDirection = 3;
  }

  if (newDirection > 3) {
    newDirection = 0;
  }

  snakeDirection = newDirection;
  soundNavigate();
}

void snakeGameOver() {
  submitScore(snakeScore);
  soundGameOver();

  rgbRed();
  state = STATE_SNAKE_GAME_OVER;
  invalidateLCD();
}

void updateSnakeMovement() {
  int8_t nextX = snake[0].x + snakeDX[snakeDirection];
  int8_t nextY = snake[0].y + snakeDY[snakeDirection];

  // Wall collision.
  if (
    nextX < 0 ||
    nextX >= SNAKE_WIDTH ||
    nextY < 0 ||
    nextY >= SNAKE_HEIGHT
  ) {
    snakeGameOver();
    return;
  }

  bool ateApple = (nextX == appleX && nextY == appleY);

  // Body collision.
  if (snakeHitsBody(nextX, nextY, ateApple)) {
    snakeGameOver();
    return;
  }

  // Grow before shifting when an apple was eaten.
  if (ateApple && snakeLength < SNAKE_MAX_LENGTH) {
    snakeLength++;
  }

  for (int i = snakeLength - 1; i > 0; i--) {
    snake[i] = snake[i - 1];
  }

  snake[0].x = nextX;
  snake[0].y = nextY;

  if (ateApple) {
    snakeScore++;
    soundScore();
    placeApple();

    // Make a newly spawned apple immediately visible.
    appleVisible = true;
    lastAppleBlink = millis();
  }

  lastSnakeMove = millis();
}

void updateSnake() {
  // Encoder rotation turns the snake left or right.
  int delta = consumeEncoderDelta();

  if (delta > 0) {
    turnSnake(1);
  } else if (delta < 0) {
    turnSnake(-1);
  }

  // Blink the apple.
  if (millis() - lastAppleBlink >= APPLE_BLINK_MS) {
    lastAppleBlink = millis();
    appleVisible = !appleVisible;
  }

  // Advance the snake on its fixed movement interval.
  if (millis() - lastSnakeMove >= SNAKE_MOVE_MS) {
    updateSnakeMovement();
  }

  // Only draw while the game is still active.
  if (state == STATE_SNAKE) {
    drawSnake();
  }
}

void updateSnakeGameOver() {
  drawLCD(
    "SNAKE GAME OVER",
    "SCORE: " + String(snakeScore)
  );

  uint8_t frame[8][12] = {};

  // Large X on the 12x8 matrix.
  for (int y = 0; y < 8; y++) {
    int leftX = 1 + (y * 9) / 7;
    int rightX = 10 - (y * 9) / 7;

    if (leftX >= 0 && leftX < 12) {
      frame[y][leftX] = 1;
    }

    if (rightX >= 0 && rightX < 12) {
      frame[y][rightX] = 1;
    }
  }

  showMatrix(frame);
  rgbRed();

  if (consumeEncoderPress()) {
    startSnakeGame();
  }
}
