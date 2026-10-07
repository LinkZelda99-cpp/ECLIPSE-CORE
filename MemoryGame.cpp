#include "MemoryGame.h"

static uint16_t memoryScore(){
  if(memoryWon && memoryLevel>=MEMORY_MAX_LEVEL)return MEMORY_MAX_LEVEL;
  return memoryLevel>0?memoryLevel-1:0;
}

void startMemoryGame(){
  for(uint8_t i=0;i<MEMORY_MAX_LEVEL;i++)memorySequence[i]=random(0,4);

  memoryLevel=1;
  memoryShowPosition=0;
  memoryInputPosition=0;
  memoryChoice=0;
  memoryWon=false;
  memoryTimer=millis();

  state=STATE_MEMORY_SHOW;
  rgbPurple();
  clearEncoderEvents();
}

// Four distinct symbols. They are deliberately smaller than their
// quadrants so the selection cursor can never be mistaken for them.
static void drawMemorySymbols(uint8_t frame[8][12]){
  // 0 = single square
  frame[1][2]=1; frame[1][3]=1;
  frame[2][2]=1; frame[2][3]=1;

  // 1 = horizontal bar
  frame[1][8]=1; frame[1][9]=1; frame[1][10]=1;

  // 2 = triangle/arrow
  frame[5][2]=1; frame[6][1]=1; frame[6][2]=1; frame[6][3]=1;

  // 3 = plus
  frame[5][8]=1; frame[5][9]=1;
  frame[4][9]=1; frame[6][9]=1;
}

void showMemorySymbol(uint8_t symbol){
  uint8_t frame[8][12]={};

  switch(symbol){
    case 0:
      frame[1][2]=1; frame[1][3]=1;
      frame[2][2]=1; frame[2][3]=1;
      break;

    case 1:
      frame[1][8]=1; frame[1][9]=1; frame[1][10]=1;
      break;

    case 2:
      frame[5][2]=1;
      frame[6][1]=1; frame[6][2]=1; frame[6][3]=1;
      break;

    case 3:
      frame[5][8]=1; frame[5][9]=1;
      frame[4][9]=1; frame[6][9]=1;
      break;
  }

  showMatrix(frame);
}

void updateMemoryShow(){
  drawLCD("MEMORY","REMEMBER "+fixedNumber(memoryLevel,2));

  if(memoryShowPosition>=memoryLevel){
    // Give the player a short clean transition before input.
    memoryInputPosition=0;
    memoryChoice=0;
    memoryTimer=millis();
    state=STATE_MEMORY_INPUT;
    return;
  }

  // Each symbol gets a clear ON period followed by a short blank
  // period. The sound occurs exactly when the symbol appears.
  unsigned long elapsed=millis()-memoryTimer;

  if(elapsed<500){
    showMemorySymbol(memorySequence[memoryShowPosition]);
    return;
  }

  if(elapsed<700){
    clearMatrix();
    return;
  }

  memoryShowPosition++;
  memoryTimer=millis();

  if(memoryShowPosition<memoryLevel){
    showMemorySymbol(memorySequence[memoryShowPosition]);
    tone(PIN_BUZZER,700+memorySequence[memoryShowPosition]*250,100);
  }
}

// Four small corner marks around the selected quadrant.
// They are intentionally steady for most of the time and only
// blink softly every 700 ms — no moving animation.
static void drawMemoryCursor(uint8_t q){
  if(((millis()/700)&1)!=0)return;

  int x0=(q%2==0)?0:6;
  int x1=x0+5;
  int y0=(q<2)?0:4;
  int y1=(q<2)?3:7;

  // Only four corners. Nothing crosses the symbol.
  matrixFrame[y0][x0]=1;
  matrixFrame[y0][x0+1]=1;

  matrixFrame[y0][x1]=1;
  matrixFrame[y0][x1-1]=1;

  matrixFrame[y1][x0]=1;
  matrixFrame[y1][x0+1]=1;

  matrixFrame[y1][x1]=1;
  matrixFrame[y1][x1-1]=1;
}

void drawMemoryInput(){
  drawLCD(
    "MEMORY",
    "SELECT "+String(memoryChoice+1)+"/4"
  );

  uint8_t frame[8][12]={};

  // All four choices stay visible throughout the input phase.
  drawMemorySymbols(frame);

  // Cursor is drawn last, but only in the empty corner pixels.
  drawMemoryCursor(memoryChoice);

  showMatrix(frame);
}

void updateMemoryInput(){
  // IMPORTANT: read the encoder before drawing. This makes the
  // selected quadrant update on the same loop iteration.
  int delta=consumeEncoderDelta();

  if(delta!=0){
    // The encoder can occasionally deliver more than one detent
    // between loop iterations. Apply every detent instead of
    // collapsing them into a single movement.
    while(delta>0){
      memoryChoice++;
      if(memoryChoice>3)memoryChoice=0;
      soundNavigate();
      delta--;
    }

    while(delta<0){
      memoryChoice--;
      if(memoryChoice<0)memoryChoice=3;
      soundNavigate();
      delta++;
    }
  }

  drawMemoryInput();

  if(!consumeEncoderPress())return;

  uint8_t expected=memorySequence[memoryInputPosition];

  if(memoryChoice!=expected){
    memoryWon=false;
    submitGameScore(2,memoryScore());
    state=STATE_MEMORY_RESULT;
    soundFailure();
    rgbRed();
    return;
  }

  soundSelect();
  memoryInputPosition++;

  if(memoryInputPosition>=memoryLevel){
    if(memoryLevel>=MEMORY_MAX_LEVEL){
      memoryWon=true;
      submitGameScore(2,memoryScore());
      state=STATE_MEMORY_RESULT;
      soundSuccess();
      rgbGreen();
      return;
    }

    memoryLevel++;
    memoryShowPosition=0;
    memoryInputPosition=0;
    memoryChoice=0;
    memoryTimer=millis();
    state=STATE_MEMORY_SHOW;
    soundSuccess();
  }
}

void updateMemoryResult(){
  drawLCD(
    memoryWon?"MEMORY MASTER":"MEMORY FAILED",
    "BEST "+fixedNumber(memoryScore(),2)
  );

  showMatrixNumber(memoryScore());

  if(memoryWon)rgbGreen();
  else rgbRed();

  if(consumeEncoderPress())returnToGamesMenu();
}
