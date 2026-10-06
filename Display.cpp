#include "Display.h"
void invalidateLCD(){lcdCache0="";lcdCache1="";}
String fitLCD(String text){while(text.length()<16)text+=" ";if(text.length()>16)text=text.substring(0,16);return text;}
void writeLCDLine(uint8_t row,String text){text=fitLCD(text);if(row==0){if(text==lcdCache0)return;lcd.setCursor(0,0);lcd.print(text);lcdCache0=text;}else{if(text==lcdCache1)return;lcd.setCursor(0,1);lcd.print(text);lcdCache1=text;}}
void drawLCD(String a,String b){writeLCDLine(0,a);writeLCDLine(1,b);}
void showMatrix(uint8_t frame[8][12]){matrix.renderBitmap(frame,8,12);}
void clearMatrix(){uint8_t blank[8][12]={};showMatrix(blank);}
void showEclipseLogo(){showMatrix(eclipseLogo);}
