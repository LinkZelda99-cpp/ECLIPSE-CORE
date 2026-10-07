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

bool readDHT11(){
  if(!sensorsEnabled())return false;

  uint8_t data[5]={0,0,0,0,0};

  noInterrupts();

  pinMode(PIN_DHT11,OUTPUT);
  digitalWrite(PIN_DHT11,LOW);
  delayMicroseconds(20000);

  digitalWrite(PIN_DHT11,HIGH);
  delayMicroseconds(40);
  pinMode(PIN_DHT11,INPUT_PULLUP);

  unsigned long timeoutStart=micros();
  while(digitalRead(PIN_DHT11)==HIGH){
    if((unsigned long)(micros()-timeoutStart)>200) {
      interrupts();
      return false;
    }
  }

  timeoutStart=micros();
  while(digitalRead(PIN_DHT11)==LOW){
    if((unsigned long)(micros()-timeoutStart)>200) {
      interrupts();
      return false;
    }
  }

  timeoutStart=micros();
  while(digitalRead(PIN_DHT11)==HIGH){
    if((unsigned long)(micros()-timeoutStart)>200) {
      interrupts();
      return false;
    }
  }

  for(uint8_t i=0;i<40;i++){
    timeoutStart=micros();
    while(digitalRead(PIN_DHT11)==LOW){
      if((unsigned long)(micros()-timeoutStart)>100) {
        interrupts();
        return false;
      }
    }

    unsigned long highStart=micros();

    timeoutStart=highStart;
    while(digitalRead(PIN_DHT11)==HIGH){
      if((unsigned long)(micros()-timeoutStart)>100) {
        interrupts();
        return false;
      }
    }

    unsigned long highTime=micros()-highStart;

    if(highTime>45){
      data[i/8]|=(uint8_t)(1<<(7-(i%8)));
    }
  }

  interrupts();

  if((uint8_t)(data[0]+data[1]+data[2]+data[3])!=data[4]){
    return false;
  }

  dhtHumidity=data[0];
  dhtTempC=data[2];
  dhtTempF=(dhtTempC*9+2)/5+32;
  dhtValid=true;
  return true;
}
