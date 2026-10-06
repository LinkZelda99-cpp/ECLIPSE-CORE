#include "CoreFeatures.h"
void updateLightSensor(){if(millis()-lastLightRead<100)return;lastLightRead=millis();lightReading=analogRead(PIN_LIGHT);}
long readUltrasonic(){digitalWrite(PIN_TRIG,LOW);delayMicroseconds(2);digitalWrite(PIN_TRIG,HIGH);delayMicroseconds(10);digitalWrite(PIN_TRIG,LOW);unsigned long d=pulseIn(PIN_ECHO,HIGH,18000UL);if(!d)return -1;long cm=d/58;if(cm<2||cm>300)return -1;return cm;}
void updateDistanceSensor(){if(millis()-lastDistanceRead<150)return;lastDistanceRead=millis();distanceReading=readUltrasonic();}
void returnToMainMenu(){state=STATE_MENU;lastMainMenuIndex=-1;invalidateLCD();lcd.clear();showEclipseLogo();rgbPurple();clearEncoderEvents();}
void returnToGamesMenu(){state=STATE_GAMES_MENU;lastGameMenuIndex=-1;invalidateLCD();lcd.clear();clearEncoderEvents();}
void updateLightApp(){updateLightSensor();int p=map(lightReading,0,1023,0,100);drawLCD("LIGHT",String(p)+"% "+String(lightReading));uint8_t f[8][12]={};int bars=constrain(map(lightReading,0,1023,0,12),0,12);for(int x=0;x<bars;x++)for(int y=3;y<8;y++)f[y][x]=1;showMatrix(f);rgbBlue();if(consumeEncoderPress())returnToMainMenu();}
void updateDistanceApp(){updateDistanceSensor();if(distanceReading<0)drawLCD("DISTANCE","NO ECHO");else drawLCD("DISTANCE",String(distanceReading)+" cm");uint8_t f[8][12]={};if(distanceReading<0){for(int i=0;i<8;i++){f[i][i]=1;f[i][11-i]=1;}}else{int bars=constrain(map(constrain(distanceReading,2L,100L),100,2,1,12),1,12);for(int x=0;x<bars;x++)for(int y=2;y<6;y++)f[y][x]=1;}showMatrix(f);rgbBlue();if(consumeEncoderPress())returnToMainMenu();}
void updateScoresApp(){drawLCD("HIGH SCORE",String(highScore)+" POINTS");uint8_t f[8][12]={};int bars=highScore%13;if(highScore>0&&bars==0)bars=12;for(int x=0;x<bars;x++)for(int y=4;y<8;y++)f[y][x]=1;showMatrix(f);rgbPurple();if(consumeEncoderPress())returnToMainMenu();}
void updateCoreInfo(){drawLCD("CORE ONLINE","v2.1 R4 WiFi");showEclipseLogo();rgbPurple();if(consumeEncoderPress())returnToMainMenu();}
