/* ECLIPSE CORE v2.3 - Arduino UNO R4 WiFi */
#include "Config.h"
#include "Input.h"
#include "Display.h"
#include "Sound.h"
#include "Storage.h"
#include "Menu.h"
#include "Games.h"
#include "CodeGame.h"
#include "ReactGame.h"
#include "MemoryGame.h"
#include "SnakeGame.h"
#include "CoreFeatures.h"
#include "SoluneApp.h"
#include "SongsApp.h"
#include "Sensors/Sensors.cpp"

void setup(){
  pinMode(PIN_ENCODER_DT,INPUT_PULLUP); pinMode(PIN_ENCODER_CLK,INPUT_PULLUP);
  pinMode(PIN_ENCODER_SW,INPUT_PULLUP); pinMode(PIN_BACK,INPUT_PULLUP);
  pinMode(PIN_TRIG,OUTPUT); pinMode(PIN_ECHO,INPUT); digitalWrite(PIN_TRIG,LOW);
  pinMode(PIN_BUZZER,OUTPUT); buzzerOff();
  pinMode(PIN_RGB_R,OUTPUT); pinMode(PIN_RGB_G,OUTPUT); pinMode(PIN_RGB_B,OUTPUT); rgbOff();
  lcd.begin(16,2); lcd.clear();
  matrixLastScanMicros=micros();
  { uint8_t testFrame[8][12]={}; for(uint8_t y=0;y<8;y++)for(uint8_t x=0;x<12;x++)testFrame[y][x]=(x==y||x==(11-y)); showMatrix(testFrame); matrixDelay(500); clearMatrix(); matrixDelay(50); }
  loadHighScores();
  randomSeed(analogRead(PIN_LIGHT)^micros());
  int clk=digitalRead(PIN_ENCODER_CLK),dt=digitalRead(PIN_ENCODER_DT); encoderLastState=(clk<<1)|dt;
  encoderButtonStable=digitalRead(PIN_ENCODER_SW); encoderButtonLast=encoderButtonStable; encoderButtonTimer=millis();
  backButtonStable=digitalRead(PIN_BACK); backButtonLast=backButtonStable; backButtonTimer=millis();
  showEclipseLogo(); rgbPurple(); drawLCD("    ECLIPSE","      CORE");
  tone(PIN_BUZZER,660,80); matrixDelay(100); tone(PIN_BUZZER,880,80); matrixDelay(100); tone(PIN_BUZZER,1320,120); matrixDelay(350); buzzerOff();
  lcd.clear(); invalidateLCD(); state=STATE_MENU;
}

void loop(){
  matrixService(); updateEncoder(); updateEncoderButton(); updateBackButton();
  if(consumeBackPress()){handleBackButton();return;}
  switch(state){
    case STATE_MENU:updateMainMenu();break; case STATE_GAMES_MENU:updateGamesMenu();break;
    case STATE_CODE:updateCodeGame();break; case STATE_CODE_RESULT:updateCodeResult();break;
    case STATE_REACT_WAIT:updateReactWait();break; case STATE_REACT_READY:updateReactReady();break; case STATE_REACT_RESULT:updateReactResult();break;
    case STATE_MEMORY_SHOW:updateMemoryShow();break; case STATE_MEMORY_INPUT:updateMemoryInput();break; case STATE_MEMORY_RESULT:updateMemoryResult();break;
    case STATE_SNAKE_READY:drawSnakeReady();if(consumeEncoderPress()){state=STATE_SNAKE;invalidateLCD();lcd.clear();clearEncoderEvents();lastSnakeMove=millis();}break;
    case STATE_SNAKE:updateSnake();break; case STATE_SNAKE_GAME_OVER:updateSnakeGameOver();break;
    case STATE_LIGHT:updateLightApp();break; case STATE_DISTANCE:updateDistanceApp();break;
    case STATE_SCORES_MENU:updateScoresMenu();break; case STATE_SCORE_DETAIL:updateScoreDetail();break; case STATE_SOLUNE:updateSoluneApp();break; case STATE_SONGS:updateSongsApp();break; case STATE_CORE_INFO:updateCoreInfo();break;
  }
}
