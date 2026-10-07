#include "CodeGame.h"

static uint16_t currentCodeValue(){
  return (uint16_t)(codeGuess[0]*1000+codeGuess[1]*100+codeGuess[2]*10+codeGuess[3]);
}

static uint16_t codeScore(){
  if(!codeWon)return 0;
  int score=1000-(codeAttempts-1)*150;
  if(score<250)score=250;
  return (uint16_t)score;
}

void startCodeGame(){
  for(uint8_t i=0;i<4;i++){codeSecret[i]=random(0,10);codeGuess[i]=0;}
  codePosition=0; codeAttempts=0; codeExact=0; codeClose=0;
  codeWon=false; codeFinished=false; codeResultStarted=0;
  state=STATE_CODE; rgbBlue(); clearEncoderEvents();
}

void drawCodeGame(){
  drawLCD("CODE "+fixedNumber(currentCodeValue(),4),"TRY "+String(codeAttempts+1)+"/6");
  showMatrixNumber(currentCodeValue());
  rgbBlue();
}

void evaluateCode(){
  codeAttempts++;
  codeExact=0; codeClose=0;

  bool usedSecret[4]={false,false,false,false};
  bool usedGuess[4]={false,false,false,false};

  for(uint8_t i=0;i<4;i++){
    if(codeGuess[i]==codeSecret[i]){
      codeExact++;
      usedSecret[i]=true;
      usedGuess[i]=true;
    }
  }

  for(uint8_t i=0;i<4;i++){
    if(usedGuess[i])continue;
    for(uint8_t j=0;j<4;j++){
      if(usedSecret[j])continue;
      if(codeGuess[i]==codeSecret[j]){
        codeClose++;
        usedSecret[j]=true;
        break;
      }
    }
  }

  codeWon=(codeExact==4);
  codeFinished=codeWon||(codeAttempts>=6);
  codeResultStarted=millis();
  state=STATE_CODE_RESULT;

  if(codeWon){
    submitGameScore(0,codeScore());
    soundSuccess();
    rgbGreen();
  }else{
    soundFailure();
    rgbRed();
  }
}

void updateCodeGame(){
  drawCodeGame();

  int d=consumeEncoderDelta();
  if(d){
    int v=codeGuess[codePosition]+d;
    while(v<0)v+=10;
    while(v>9)v-=10;
    codeGuess[codePosition]=(uint8_t)v;
    soundNavigate();
  }

  if(consumeEncoderPress()){
    if(codePosition<3){
      codePosition++;
      soundSelect();
    }else{
      evaluateCode();
    }
  }
}

void updateCodeResult(){
  if(codeWon){
    drawLCD("ACCESS GRANTED","SCORE "+fixedNumber(codeScore(),4));
    showMatrixNumber(codeScore());
    rgbGreen();
  }else if(codeFinished){
    drawLCD("CODE FAILED",String(codeSecret[0])+String(codeSecret[1])+String(codeSecret[2])+String(codeSecret[3]));
    showMatrixNumber(0);
    rgbRed();
  }else{
    String clue=String(codeExact)+" EXACT "+String(codeClose)+" CLOSE";
    drawLCD(clue,"TRY "+String(codeAttempts+1)+"/6");
    showMatrixNumber(currentCodeValue());
    rgbRed();
  }

  if(millis()-codeResultStarted>=1400){
    if(codeFinished){
      returnToGamesMenu();
    }else{
      state=STATE_CODE;
      codePosition=0;
      invalidateLCD();
      lcd.clear();
      clearEncoderEvents();
      rgbBlue();
    }
  }
}
