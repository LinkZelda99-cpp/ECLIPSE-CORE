#include "Input.h"

static const uint8_t R_START=0x00;
static const uint8_t R_CW_FINAL=0x01;
static const uint8_t R_CW_BEGIN=0x02;
static const uint8_t R_CW_NEXT=0x03;
static const uint8_t R_CCW_BEGIN=0x04;
static const uint8_t R_CCW_FINAL=0x05;
static const uint8_t R_CCW_NEXT=0x06;

static const uint8_t DIR_CW=0x10;
static const uint8_t DIR_CCW=0x20;

// Full-step quadrature decoder.
// It emits exactly one event when a complete detent is reached and
// rejects invalid/bouncing transitions instead of silently losing steps.
static const uint8_t encoderStateTable[7][4]={
  {R_START, R_CW_BEGIN, R_CCW_BEGIN, R_START},
  {R_CW_NEXT, R_START, R_CW_FINAL, R_START|DIR_CW},
  {R_CW_NEXT, R_CW_BEGIN, R_START, R_START},
  {R_CW_NEXT, R_CW_BEGIN, R_CW_FINAL, R_START},
  {R_CCW_NEXT, R_START, R_CCW_BEGIN, R_START},
  {R_CCW_NEXT, R_CCW_FINAL, R_START, R_START|DIR_CCW},
  {R_CCW_NEXT, R_CCW_FINAL, R_CCW_BEGIN, R_START}
};

static uint8_t encoderDecoderState=R_START;

void updateEncoder(){
  uint8_t pinState=(digitalRead(PIN_ENCODER_DT)<<1)|digitalRead(PIN_ENCODER_CLK);
  encoderDecoderState=encoderStateTable[encoderDecoderState&0x0F][pinState];

  if(encoderDecoderState&DIR_CW)encoderDelta++;
  else if(encoderDecoderState&DIR_CCW)encoderDelta--;

  encoderLastState=pinState;
}

void updateEncoderButton(){
  bool r=digitalRead(PIN_ENCODER_SW);
  if(r!=encoderButtonLast){
    encoderButtonTimer=millis();
    encoderButtonLast=r;
  }
  if(millis()-encoderButtonTimer>=BUTTON_DEBOUNCE_MS&&r!=encoderButtonStable){
    encoderButtonStable=r;
    if(!r)encoderPressEvent=true;
  }
}

void updateBackButton(){
  bool r=digitalRead(PIN_BACK);
  if(r!=backButtonLast){
    backButtonTimer=millis();
    backButtonLast=r;
  }
  if(millis()-backButtonTimer>=BUTTON_DEBOUNCE_MS&&r!=backButtonStable){
    backButtonStable=r;
    if(!r)backPressEvent=true;
  }
}

bool consumeBackPress(){
  if(!backPressEvent)return false;
  backPressEvent=false;
  return true;
}

int consumeEncoderDelta(){
  int r=encoderDelta;
  encoderDelta=0;
  return r;
}

bool consumeEncoderPress(){
  if(!encoderPressEvent)return false;
  encoderPressEvent=false;
  return true;
}

void clearEncoderEvents(){
  encoderDelta=0;
  encoderAccumulator=0;
  encoderPressEvent=false;
  backPressEvent=false;

  // Re-synchronize the decoder with the physical encoder position.
  uint8_t pinState=(digitalRead(PIN_ENCODER_DT)<<1)|digitalRead(PIN_ENCODER_CLK);
  encoderLastState=pinState;
  encoderDecoderState=(pinState==0)?R_START:R_START;
}
