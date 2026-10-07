#include "Input.h"

int encoderLastState = 0;
int encoderAccumulator = 0;
int encoderDelta = 0;

bool encoderButtonStable = HIGH;
bool encoderButtonLast = HIGH;
unsigned long encoderButtonTimer = 0;
const unsigned long BUTTON_DEBOUNCE_MS = 35;
bool encoderPressEvent = false;

bool backButtonStable = HIGH;
bool backButtonLast = HIGH;
unsigned long backButtonTimer = 0;
bool backPressEvent = false;

// Standard quadrature transition table.
// We accumulate four valid quarter-steps into one logical encoder detent.
// This is deliberately tolerant of mechanical bounce.
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

  // The main sketch initializes encoderLastState from the real pins.
  // Do not reset it to an arbitrary quadrature state here.
}
