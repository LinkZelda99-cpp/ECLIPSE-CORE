#include "SensorsApp.h"

void drawSensorsMenu(){
  if(sensorMenuIndex==lastSensorMenuIndex)return;
  lastSensorMenuIndex=sensorMenuIndex;
  drawLCD("SENSORS","> "+String(sensorMenuItems[sensorMenuIndex]));
  showEclipseLogo();
  rgbBlue();
}

void updateSensorsMenu(){
  int d=consumeEncoderDelta();

  if(d){
    sensorMenuIndex+=d>0?1:-1;
    if(sensorMenuIndex<0)sensorMenuIndex=SENSOR_MENU_COUNT-1;
    if(sensorMenuIndex>=SENSOR_MENU_COUNT)sensorMenuIndex=0;
    soundNavigate();
  }

  drawSensorsMenu();

  if(consumeEncoderPress()){
    soundSelect();

    switch(sensorMenuIndex){
      case 0:
        state=STATE_LIGHT;
        break;

      case 1:
        state=STATE_DISTANCE;
        break;

      case 2:
        startTemperatureApp();
        break;
    }

    invalidateLCD();
    lcd.clear();
  }
}
