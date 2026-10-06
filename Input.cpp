#include "Input.h"
void updateEncoder(){int clk=digitalRead(PIN_ENCODER_CLK),dt=digitalRead(PIN_ENCODER_DT),cur=(clk<<1)|dt;if(cur==encoderLastState)return;static const int8_t table[16]={0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};encoderAccumulator+=table[(encoderLastState<<2)|cur];encoderLastState=cur;if(encoderAccumulator>=4){encoderDelta++;encoderAccumulator=0;}else if(encoderAccumulator<=-4){encoderDelta--;encoderAccumulator=0;}}
void updateEncoderButton(){bool r=digitalRead(PIN_ENCODER_SW);if(r!=encoderButtonLast){encoderButtonTimer=millis();encoderButtonLast=r;}if(millis()-encoderButtonTimer>=BUTTON_DEBOUNCE_MS&&r!=encoderButtonStable){encoderButtonStable=r;if(!r)encoderPressEvent=true;}}
void updateBackButton(){bool r=digitalRead(PIN_BACK);if(r!=backButtonLast){backButtonTimer=millis();backButtonLast=r;}if(millis()-backButtonTimer>=BUTTON_DEBOUNCE_MS&&r!=backButtonStable){backButtonStable=r;if(!r)backPressEvent=true;}}
bool consumeBackPress(){if(!backPressEvent)return false;backPressEvent=false;return true;}
int consumeEncoderDelta(){int r=encoderDelta;encoderDelta=0;return r;}
bool consumeEncoderPress(){if(!encoderPressEvent)return false;encoderPressEvent=false;return true;}
void clearEncoderEvents(){encoderDelta=0;encoderAccumulator=0;encoderPressEvent=false;backPressEvent=false;}
