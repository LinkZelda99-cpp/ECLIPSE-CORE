#include "CoreFeatures.h"

void returnToMainMenu(){
  state=STATE_MENU;
  lastMainMenuIndex=-1;
  invalidateLCD();
  lcd.clear();
  showEclipseLogo();
  rgbPurple();
  clearEncoderEvents();
}

void returnToGamesMenu(){
  state=STATE_GAMES_MENU;
  lastGameMenuIndex=-1;
  invalidateLCD();
  lcd.clear();
  clearEncoderEvents();
}

void updateLightApp(){
  if(!sensorsEnabled()){
    drawLCD("LIGHT","SENSORS OFF");
    clearMatrix();
    rgbBlue();
    if(consumeEncoderPress()){state=STATE_SENSORS_MENU; lastSensorMenuIndex=-1; invalidateLCD(); lcd.clear(); clearEncoderEvents();}
    return;
  }
  updateLightSensor();
  int percent=map(lightReading,0,1023,0,100);
  drawLCD("LIGHT","LEVEL: "+fixedNumber(percent,3)+"%");
  uint8_t frame[8][12]={};
  int bars=constrain(map(lightReading,0,1023,0,12),0,12);
  for(int x=0;x<bars;x++)for(int y=3;y<8;y++)frame[y][x]=1;
  showMatrix(frame);
  rgbBlue();
  if(consumeEncoderPress()){state=STATE_SENSORS_MENU; lastSensorMenuIndex=-1; invalidateLCD(); lcd.clear(); clearEncoderEvents();}
}

void updateDistanceApp(){
  if(!sensorsEnabled()){
    drawLCD("DISTANCE","SENSORS OFF");
    clearMatrix();
    rgbBlue();
    if(consumeEncoderPress()){state=STATE_SENSORS_MENU; lastSensorMenuIndex=-1; invalidateLCD(); lcd.clear(); clearEncoderEvents();}
    return;
  }
  updateDistanceSensor();
  if(distanceReading<0)drawLCD("DISTANCE","NO ECHO");
  else drawLCD("DISTANCE","VALUE: "+fixedNumber((uint16_t)constrain(distanceReading,0L,9999L),3)+" CM");

  uint8_t frame[8][12]={};
  if(distanceReading<0){
    for(int i=0;i<8;i++){frame[i][i]=1;frame[i][11-i]=1;}
  }else{
    int bars=constrain(map(constrain(distanceReading,2L,100L),100,2,1,12),1,12);
    for(int x=0;x<bars;x++)for(int y=2;y<6;y++)frame[y][x]=1;
  }
  showMatrix(frame);
  rgbBlue();
  if(consumeEncoderPress()){state=STATE_SENSORS_MENU; lastSensorMenuIndex=-1; invalidateLCD(); lcd.clear(); clearEncoderEvents();}
}

void updateScoresApp(){
  state=STATE_SCORES_MENU;
  lastScoreMenuIndex=-1;
  updateScoresMenu();
}

void updateCoreInfo(){
  drawLCD("CORE ONLINE","v2.2 R4 WiFi");
  showEclipseLogo();
  rgbPurple();
  if(consumeEncoderPress())returnToMainMenu();
}
