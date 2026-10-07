#include "TemperatureApp.h"

namespace {
const unsigned long DHT_READ_INTERVAL_MS=2200;
const unsigned long DISPLAY_SWAP_MS=1800;
bool showTemperature=true;
unsigned long lastDisplaySwap=0;
}

void startTemperatureApp(){
  state=STATE_TEMPERATURE;
  showTemperature=true;
  lastDisplaySwap=millis();
  lastDHTRead=0;
  dhtValid=false;
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

  if(millis()-lastDHTRead>=DHT_READ_INTERVAL_MS || lastDHTRead==0){
    lastDHTRead=millis();
    readDHT11();
  }

  if(millis()-lastDisplaySwap>=DISPLAY_SWAP_MS){
    lastDisplaySwap=millis();
    showTemperature=!showTemperature;
  }

  if(!dhtValid){
    drawLCD("TEMP/HUMIDITY","READING...");
    clearMatrix();
  }else if(showTemperature){
    drawLCD("TEMPERATURE","VALUE: "+String(dhtTempF)+" F");
    showMatrixNumber((uint16_t)constrain(dhtTempF,0,9999));
  }else{
    drawLCD("HUMIDITY","VALUE: "+String(dhtHumidity)+"%");
    showMatrixNumber((uint16_t)dhtHumidity);
  }

  rgbBlue();

  if(consumeEncoderPress()){
    returnToMainMenu();
  }
}
