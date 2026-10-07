#include "Input.h"

// Input state is defined once in Config.cpp and declared extern in Config.h.
// Keeping the storage there avoids duplicate-definition linker errors.

static const int8_t encoderTransitionTable[16] = {
   0, -1,  1,  0,
   1,  0,  0, -1,
  -1,  0,  0,  1,
   0,  1, -1,  0
};

void updateEncoder(){
  int clk=digitalRead(PIN_ENCODER_CLK);
  int dt=digitalRead(PIN_ENCODER_DT);
  int currentState=(clk<<1)|dt;

  if(currentState==encoderLastState)return;

  int index=(encoderLastState<<2)|currentState;
  encoderAccumulator+=encoderTransitionTable[index];
  encoderLastState=currentState;

  if(encoderAccumulator>=4){
    encoderDelta++;
    encoderAccumulator=0;
  }else if(encoderAccumulator<=-4){
    encoderDelta--;
    encoderAccumulator=0;
  }
}

void updateEncoderButton(){
  bool reading=digitalRead(PIN_ENCODER_SW);

  if(reading!=encoderButtonLast){
    encoderButtonTimer=millis();
    encoderButtonLast=reading;
  }

  if(millis()-encoderButtonTimer>=BUTTON_DEBOUNCE_MS){
    if(reading!=encoderButtonStable){
      encoderButtonStable=reading;
      if(encoderButtonStable==LOW)encoderPressEvent=true;
    }
  }
}

void updateBackButton(){
  bool reading=digitalRead(PIN_BACK);

  if(reading!=backButtonLast){
    backButtonTimer=millis();
    backButtonLast=reading;
  }

  if(millis()-backButtonTimer>=BUTTON_DEBOUNCE_MS){
    if(reading!=backButtonStable){
      backButtonStable=reading;
      if(backButtonStable==LOW)backPressEvent=true;
    }
  }
}

bool consumeBackPress(){
  if(!backPressEvent)return false;
  backPressEvent=false;
  return true;
}

int consumeEncoderDelta(){
  int result=encoderDelta;
  encoderDelta=0;
  return result;
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

  // The main sketch synchronizes encoderLastState with the real pins.
  // Do not create a second copy of the input state here.
}
