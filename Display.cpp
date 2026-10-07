#include "Display.h"
#include <string.h>

void invalidateLCD(){lcdCache0="";lcdCache1="";}

String fitLCD(String text){
  while(text.length()<16) text+=" ";
  if(text.length()>16) text=text.substring(0,16);
  return text;
}

String fixedNumber(uint16_t value,uint8_t width){
  String s=String(value);
  while(s.length()<width) s="0"+s;
  if(s.length()>width) s=s.substring(s.length()-width);
  return s;
}

void writeLCDLine(uint8_t row,String text){
  text=fitLCD(text);
  if(row==0){
    if(text==lcdCache0)return;
    lcd.setCursor(0,0); lcd.print(text); lcdCache0=text;
  }else{
    if(text==lcdCache1)return;
    lcd.setCursor(0,1); lcd.print(text); lcdCache1=text;
  }
}

void drawLCD(String line0,String line1){writeLCDLine(0,line0);writeLCDLine(1,line1);}

void matrixService(){
  unsigned long now=micros();
  if((unsigned long)(now-matrixLastScanMicros)<MATRIX_SCAN_INTERVAL_US)return;
  matrixLastScanMicros=now;
  uint8_t index=matrixScanIndex;
  uint8_t y=index/12,x=index%12;
  if(matrixFrame[y][x])matrix.on(index);else matrix.off(index);
  matrixScanIndex=(index+1)%96;
}

void matrixDelay(unsigned long milliseconds){
  unsigned long start=millis();
  while((unsigned long)(millis()-start)<milliseconds)matrixService();
}

void showMatrix(uint8_t frame[8][12]){memcpy(matrixFrame,frame,sizeof(matrixFrame));}
void clearMatrix(){memset(matrixFrame,0,sizeof(matrixFrame));}
void showEclipseLogo(){showMatrix(eclipseLogo);}

// Four 2x5 digits fit exactly across the 12x8 matrix.
// Every digit occupies the same width and position, so scores never shift.
const uint8_t smallDigitFont[10][5]={
  {0b11,0b11,0b11,0b11,0b11},
  {0b01,0b11,0b01,0b01,0b11},
  {0b11,0b01,0b11,0b10,0b11},
  {0b11,0b01,0b11,0b01,0b11},
  {0b11,0b11,0b11,0b01,0b01},
  {0b11,0b10,0b11,0b01,0b11},
  {0b11,0b10,0b11,0b11,0b11},
  {0b11,0b01,0b01,0b01,0b01},
  {0b11,0b11,0b11,0b11,0b11},
  {0b11,0b11,0b11,0b01,0b11}
};

void showMatrixNumber(uint16_t value){
  if(value>9999)value=9999;
  uint8_t digits[4]={
    (uint8_t)((value/1000)%10),
    (uint8_t)((value/100)%10),
    (uint8_t)((value/10)%10),
    (uint8_t)(value%10)
  };
  uint8_t frame[8][12]={};
  for(uint8_t digit=0;digit<4;digit++){
    uint8_t x0=digit*3;
    for(uint8_t y=0;y<5;y++){
      for(uint8_t x=0;x<2;x++){
        if(smallDigitFont[digits[digit]][y]&(1<<(1-x)))frame[y+1][x0+x]=1;
      }
    }
  }
  showMatrix(frame);
}
