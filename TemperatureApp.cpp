#include "TemperatureApp.h"

namespace {
const unsigned long DHT_READ_INTERVAL_MS=2500;
const unsigned long DISPLAY_SWAP_MS=1800;
bool showTemperature=true;
unsigned long lastDisplaySwap=0;
bool dhtHasAttempted=false;
}

void startTemperatureApp(){
  state=STATE_TEMPERATURE;
  showTemperature=true;
  lastDisplaySwap=millis();
  lastDHTRead=millis();
  dhtValid=false;
  dhtHasAttempted=false;
  invalidateLCD();
  lcd.clear();
  clearEncoderEvents();
}

void updateTemperatureApp(){
  if(!sensorsEnabled()){
    drawLCD("TEMP/HUMIDITY","SENSORS OFF");
    clearMatrix();
    rgbBlue();
    if(consumeEncoderPress())returnToMainMenu();
    return;
  }

  if((unsigned long)(millis()-lastDHTRead)>=DHT_READ_INTERVAL_MS){
    lastDHTRead=millis();
    dhtHasAttempted=true;
    readDHT11();
  }

  if((unsigned long)(millis()-lastDisplaySwap)>=DISPLAY_SWAP_MS){
    lastDisplaySwap=millis();
    showTemperature=!showTemperature;
  }

  if(!dhtHasAttempted){
    drawLCD("TEMP/HUMIDITY","STARTING...");
    clearMatrix();
  }else if(!dhtValid){
    drawLCD("TEMP/HUMIDITY","SENSOR ERROR");
    uint8_t frame[8][12]={};
    for(uint8_t i=0;i<8;i++){
      frame[i][i]=1;
      frame[i][11-i]=1;
    }
    showMatrix(frame);
  }else if(showTemperature){
    drawLCD("TEMPERATURE","VALUE: "+String(dhtTempF)+" F");
    showMatrixNumber((uint16_t)constrain(dhtTempF,0,9999));
  }else{
    drawLCD("HUMIDITY","VALUE: "+String(dhtHumidity)+"%");
    showMatrixNumber((uint16_t)dhtHumidity);
  }

  rgbBlue();

  if(consumeEncoderPress()){
    state=STATE_SENSORS_MENU;
    lastSensorMenuIndex=-1;
    invalidateLCD();
    lcd.clear();
    clearEncoderEvents();
  }
}
