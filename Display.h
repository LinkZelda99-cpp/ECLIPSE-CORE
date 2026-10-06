#pragma once
#include "Config.h"
void invalidateLCD(); String fitLCD(String text); void writeLCDLine(uint8_t row,String text); void drawLCD(String line0,String line1); void showMatrix(uint8_t frame[8][12]); void clearMatrix(); void showEclipseLogo();
