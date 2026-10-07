#include "ReactGame.h"

uint16_t reactionScore(){
  if(reactTooEarly||reactTime==0)return 0;
  if(reactTime>=1000)return 1;
  return (uint16_t)(1000-reactTime);
}

void startReactGame(){
  reactTooEarly=false; reactTime=0; reactDelay=random(1500,4500); reactStarted=millis();
  state=STATE_REACT_WAIT; drawLCD("ECLIPSE REACT","WAIT..."); showMatrixNumber(0); rgbPurple(); clearEncoderEvents();
}

void updateReactWait(){
  if(consumeEncoderPress()){
    reactTooEarly=true; soundFailure(); state=STATE_REACT_RESULT; return;
  }
  if(millis()-reactStarted>=reactDelay){
    state=STATE_REACT_READY; reactStarted=millis();
    drawLCD("GO!","PRESS NOW!"); showMatrixNumber(0); rgbGreen(); playTone(1800,80);
  }
}

void updateReactReady(){
  if(consumeEncoderPress()){
    reactTime=millis()-reactStarted;
    uint16_t score=reactionScore();
    submitGameScore(1,score);
    soundSuccess(); rgbBlue(); state=STATE_REACT_RESULT;
  }
  showMatrixNumber(reactionScore());
}

void updateReactResult(){
  if(reactTooEarly){
    drawLCD("TOO EARLY!","SCORE 0000");
    showMatrixNumber(0); rgbRed();
  }else{
    drawLCD("REACTION "+fixedNumber((uint16_t)reactTime,4),"SCORE "+fixedNumber(reactionScore(),4));
    showMatrixNumber(reactionScore()); rgbBlue();
  }
  if(consumeEncoderPress())returnToGamesMenu();
}
