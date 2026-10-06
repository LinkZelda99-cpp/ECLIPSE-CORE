#include "ReactGame.h"
void startReactGame(){reactTooEarly=false;reactTime=0;reactDelay=random(1500,4500);reactStarted=millis();state=STATE_REACT_WAIT;drawLCD("ECLIPSE REACT","WAIT...");showEclipseLogo();rgbPurple();clearEncoderEvents();}
void updateReactWait(){if(consumeEncoderPress()){reactTooEarly=true;soundFailure();state=STATE_REACT_RESULT;return;}if(millis()-reactStarted>=reactDelay){state=STATE_REACT_READY;reactStarted=millis();drawLCD("GO!","PRESS NOW!");uint8_t f[8][12];for(int y=0;y<8;y++)for(int x=0;x<12;x++)f[y][x]=1;showMatrix(f);rgbGreen();tone(PIN_BUZZER,1800,80);}}
void updateReactReady(){if(consumeEncoderPress()){reactTime=millis()-reactStarted;soundSuccess();rgbBlue();state=STATE_REACT_RESULT;}}
void updateReactResult(){if(reactTooEarly){drawLCD("TOO EARLY!","PRESS TO RETRY");rgbRed();}else{drawLCD("REACTION TIME",String(reactTime)+" ms");showEclipseLogo();rgbBlue();}if(consumeEncoderPress())returnToGamesMenu();}
