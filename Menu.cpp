#include "Menu.h"
#include "SoluneApp.h"

void handleBackButton(){
  switch(state){
    case STATE_MENU: break;
    case STATE_GAMES_MENU: returnToMainMenu(); break;
    case STATE_SCORES_MENU: returnToMainMenu(); break;
    case STATE_SCORE_DETAIL:
      state=STATE_SCORES_MENU;
      lastScoreMenuIndex=-1;
      invalidateLCD();
      lcd.clear();
      clearEncoderEvents();
      break;
    case STATE_LIGHT:
    case STATE_DISTANCE:
    case STATE_SOLUNE:
    case STATE_CORE_INFO:
      returnToMainMenu();
      break;
    default:
      buzzerOff();
      returnToGamesMenu();
      break;
  }
}

void drawMainMenu(){
  if(mainMenuIndex==lastMainMenuIndex)return;
  lastMainMenuIndex=mainMenuIndex;
  drawLCD("ECLIPSE CORE","> "+String(mainMenuItems[mainMenuIndex]));
  showEclipseLogo();
  rgbPurple();
}

void updateMainMenu(){
  int d=consumeEncoderDelta();
  if(d){
    mainMenuIndex+=d>0?1:-1;
    if(mainMenuIndex<0)mainMenuIndex=MAIN_MENU_COUNT-1;
    if(mainMenuIndex>=MAIN_MENU_COUNT)mainMenuIndex=0;
    soundNavigate();
  }
  drawMainMenu();
  if(consumeEncoderPress()){
    soundSelect();
    switch(mainMenuIndex){
      case 0: state=STATE_GAMES_MENU; lastGameMenuIndex=-1; break;
      case 1: state=STATE_LIGHT; break;
      case 2: state=STATE_DISTANCE; break;
      case 3: state=STATE_SCORES_MENU; lastScoreMenuIndex=-1; break;
      case 4: startSolune(); break;
      case 5: state=STATE_CORE_INFO; break;
    }
    invalidateLCD();
    lcd.clear();
  }
}

void drawGamesMenu(){
  if(gameMenuIndex==lastGameMenuIndex)return;
  lastGameMenuIndex=gameMenuIndex;
  drawLCD("GAMES","> "+String(gameMenuItems[gameMenuIndex]));
  showEclipseLogo();
  rgbPurple();
}

void updateGamesMenu(){
  int d=consumeEncoderDelta();
  if(d){
    gameMenuIndex+=d>0?1:-1;
    if(gameMenuIndex<0)gameMenuIndex=GAME_MENU_COUNT-1;
    if(gameMenuIndex>=GAME_MENU_COUNT)gameMenuIndex=0;
    soundNavigate();
  }
  drawGamesMenu();
  if(consumeEncoderPress()){
    soundSelect();
    switch(gameMenuIndex){
      case 0:startCodeGame();break;
      case 1:startReactGame();break;
      case 2:startMemoryGame();break;
      case 3:startSnakeGame();break;
    }
    invalidateLCD();
    lcd.clear();
  }
}

void drawScoresMenu(){
  if(scoreMenuIndex==lastScoreMenuIndex)return;
  lastScoreMenuIndex=scoreMenuIndex;
  uint16_t score=getHighScore(scoreMenuIndex);
  String name=String(scoreMenuItems[scoreMenuIndex]);
  if(name=="ECLIPSE CODE")name="CODE";
  else if(name=="ECLIPSE REACT")name="REACT";
  else if(name=="ECLIPSE MEMORY")name="MEMORY";
  drawLCD("SCORES > "+name,"BEST: "+fixedNumber(score,4));
  showMatrixNumber(score);
  rgbPurple();
}

void updateScoresMenu(){
  int d=consumeEncoderDelta();
  if(d){
    scoreMenuIndex+=d>0?1:-1;
    if(scoreMenuIndex<0)scoreMenuIndex=SCORE_MENU_COUNT-1;
    if(scoreMenuIndex>=SCORE_MENU_COUNT)scoreMenuIndex=0;
    soundNavigate();
  }
  drawScoresMenu();
  if(consumeEncoderPress()){
    soundSelect();
    state=STATE_SCORE_DETAIL;
    invalidateLCD();
    lcd.clear();
  }
}

void drawScoreDetail(){
  uint16_t score=getHighScore(scoreMenuIndex);
  String name=String(scoreMenuItems[scoreMenuIndex]);
  drawLCD(name,"HIGH SCORE "+fixedNumber(score,4));
  showMatrixNumber(score);
  rgbPurple();
}

void updateScoreDetail(){
  drawScoreDetail();
  if(consumeEncoderPress()){
    state=STATE_SCORES_MENU;
    lastScoreMenuIndex=-1;
    invalidateLCD();
    lcd.clear();
    soundSelect();
  }
}
