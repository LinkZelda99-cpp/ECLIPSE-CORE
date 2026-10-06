#pragma once
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Arduino_LED_Matrix.h>
#include <EEPROM.h>

extern const uint8_t PIN_ENCODER_DT, PIN_ENCODER_CLK, PIN_DHT11, PIN_ENCODER_SW, PIN_TRIG, PIN_ECHO, PIN_BUZZER, PIN_BACK;
extern const uint8_t PIN_RGB_R, PIN_RGB_G, PIN_RGB_B, PIN_LCD_RS, PIN_LIGHT, PIN_LCD_E, PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7;
extern LiquidCrystal lcd;
extern ArduinoLEDMatrix matrix;
extern const int EEPROM_HIGH_SCORE_ADDRESS;
extern uint16_t highScore;
extern uint8_t eclipseLogo[8][12];
extern uint8_t matrixFrame[8][12]; extern uint8_t matrixScanIndex; extern unsigned long matrixLastScanMicros; extern const unsigned long MATRIX_SCAN_INTERVAL_US;

enum AppState { STATE_MENU, STATE_GAMES_MENU, STATE_CODE, STATE_CODE_RESULT, STATE_REACT_WAIT, STATE_REACT_READY, STATE_REACT_RESULT, STATE_MEMORY_SHOW, STATE_MEMORY_INPUT, STATE_MEMORY_RESULT, STATE_SNAKE_READY, STATE_SNAKE, STATE_SNAKE_GAME_OVER, STATE_LIGHT, STATE_DISTANCE, STATE_SCORES, STATE_CORE_INFO };
extern AppState state;
extern const char* mainMenuItems[]; extern const uint8_t MAIN_MENU_COUNT; extern int mainMenuIndex; extern int lastMainMenuIndex;
extern const char* gameMenuItems[]; extern const uint8_t GAME_MENU_COUNT; extern int gameMenuIndex; extern int lastGameMenuIndex;
extern int encoderLastState, encoderAccumulator, encoderDelta;
extern bool encoderButtonStable, encoderButtonLast; extern unsigned long encoderButtonTimer; extern const unsigned long BUTTON_DEBOUNCE_MS; extern bool encoderPressEvent;
extern bool backButtonStable, backButtonLast; extern unsigned long backButtonTimer; extern bool backPressEvent;
extern String lcdCache0, lcdCache1;
extern int lightReading; extern long distanceReading; extern unsigned long lastLightRead, lastDistanceRead;
extern uint8_t codeSecret[4], codeGuess[4], codePosition; extern bool codeWon; extern unsigned long codeResultStarted;
extern unsigned long reactStarted, reactDelay, reactTime; extern bool reactTooEarly;
extern const uint8_t MEMORY_MAX_LEVEL; extern uint8_t memorySequence[]; extern uint8_t memoryLevel, memoryShowPosition, memoryInputPosition; extern int memoryChoice; extern bool memoryWon; extern unsigned long memoryTimer;
struct SnakeSegment { int8_t x; int8_t y; };
extern const uint8_t SNAKE_WIDTH, SNAKE_HEIGHT, SNAKE_MAX_LENGTH; extern SnakeSegment snake[96]; extern uint8_t snakeLength, snakeDirection; extern const int8_t snakeDX[4], snakeDY[4]; extern int8_t appleX, appleY; extern uint16_t snakeScore; extern unsigned long lastSnakeMove, lastAppleBlink; extern bool appleVisible; extern const unsigned long SNAKE_MOVE_MS, APPLE_BLINK_MS;

void handleBackButton();
void invalidateLCD(); String fitLCD(String text); void writeLCDLine(uint8_t row, String text); void drawLCD(String line0, String line1);
void rgbOff(); void rgbRed(); void rgbGreen(); void rgbBlue(); void rgbPurple(); void rgbWhite(); void buzzerOff();
void soundNavigate(); void soundSelect(); void soundSuccess(); void soundFailure(); void soundScore(); void soundGameOver();
void showMatrix(uint8_t frame[8][12]); void clearMatrix(); void showEclipseLogo(); void showMatrixNumber(uint16_t value);
void updateEncoder(); void updateEncoderButton(); void updateBackButton(); bool consumeBackPress(); int consumeEncoderDelta(); bool consumeEncoderPress(); void clearEncoderEvents();
void loadHighScore(); void saveHighScore(); void submitScore(uint16_t score);
void updateLightSensor(); long readUltrasonic(); void updateDistanceSensor(); void returnToMainMenu(); void returnToGamesMenu();
void drawMainMenu(); void updateMainMenu(); void drawGamesMenu(); void updateGamesMenu();
void startCodeGame(); void drawCodeGame(); void evaluateCode(); void updateCodeGame(); void updateCodeResult();
void startReactGame(); void updateReactWait(); void updateReactReady(); void updateReactResult();
void startMemoryGame(); void showMemorySymbol(uint8_t symbol); void updateMemoryShow(); void drawMemoryInput(); void updateMemoryInput(); void updateMemoryResult();
void placeApple(); bool snakeHitsBody(int8_t x,int8_t y,bool growing); void resetSnake(); void startSnakeGame(); void drawSnakeReady(); void drawSnake(); void turnSnake(int turn); void snakeGameOver(); void updateSnakeMovement(); void updateSnake(); void updateSnakeGameOver();
void updateLightApp(); void updateDistanceApp(); void updateScoresApp(); void updateCoreInfo();
