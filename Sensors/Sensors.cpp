#include "Sensors.h"

void updateLightSensor(){
  if(!sensorsEnabled())return;
  if(millis()-lastLightRead<100)return;
  lastLightRead=millis();
  lightReading=analogRead(PIN_LIGHT);
}

long readUltrasonic(){
  digitalWrite(PIN_TRIG,LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG,HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG,LOW);

  unsigned long duration=pulseIn(PIN_ECHO,HIGH,18000UL);
  if(!duration)return -1;

  long cm=duration/58;
  if(cm<2||cm>300)return -1;
  return cm;
}

void updateDistanceSensor(){
  if(!sensorsEnabled())return;
  if(millis()-lastDistanceRead<150)return;
  lastDistanceRead=millis();
  distanceReading=readUltrasonic();
}
