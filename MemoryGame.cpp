#include "MemoryGame.h"

static uint16_t memoryScore(){
  if(memoryWon && memoryLevel>=MEMORY_MAX_LEVEL)return MEMORY_MAX_LEVEL;
  return memoryLevel>0?memoryLevel-1:0;
}

void startMemoryGame(){
  for(uint8_t i=0;i<MEMORY_MAX_LEVEL;i++) memorySequence[i]=random(0,4);

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

// Draw the four identical rectangular memory targets.
// The rectangles are the game itself; the cursor is added separately.
static void drawMemoryBoard(uint8_t frame[8][12]){
  // Top-left
  for(int y=1;y<=3;y++) for(int x=1;x<=4;x++) frame[y][x]=1;

  // Top-right
  for(int y=1;y<=3;y++) for(int x=7;x<=10;x++) frame[y][x]=1;

  // Bottom-left
  for(int y=5;y<=7;y++) for(int x=1;x<=4;x++) frame[y][x]=1;

  // Bottom-right
  for(int y=5;y<=7;y++) for(int x=7;x<=10;x++) frame[y][x]=1;
}

void showMemorySymbol(uint8_t symbol){
  uint8_t frame[8][12]={};

  // During the memorize phase, light only the selected rectangle.
  switch(symbol){
    case 0:
      for(int y=1;y<=3;y++) for(int x=1;x<=4;x++) frame[y][x]=1;
      break;

    case 1:
      for(int y=1;y<=3;y++) for(int x=7;x<=10;x++) frame[y][x]=1;
      break;

    case 2:
      for(int y=5;y<=7;y++) for(int x=1;x<=4;x++) frame[y][x]=1;
      break;

    case 3:
      for(int y=5;y<=7;y++) for(int x=7;x<=10;x++) frame[y][x]=1;
      break;
  }

  showMatrix(frame);
}

static void drawMemoryCursor(uint8_t q){
  // Gentle blink: slow enough to be noticeable, not distracting.
  if(((millis()/700)&1)!=0)return;

  int x0=(q%2==0)?0:6;
  int x1=x0+5;
  int y0=(q<2)?0:4;
  int y1=(q<2)?4:7;

  // Four corner brackets. They sit OUTSIDE the filled rectangle,
  // so the cursor cannot be mistaken for part of the remembered target.
  matrixFrame[y0][x0]=1;
  matrixFrame[y0][x0+1]=1;

  matrixFrame[y0][x1]=1;
  matrixFrame[y0][x1-1]=1;

  matrixFrame[y1][x0]=1;
  matrixFrame[y1][x0+1]=1;

  matrixFrame[y1][x1]=1;
  matrixFrame[y1][x1-1]=1;
}

void updateMemoryShow(){
  drawLCD("MEMORY","REMEMBER "+fixedNumber(memoryLevel,2));

  if(memoryShowPosition>=memoryLevel){
    memoryInputPosition=0;
    memoryChoice=0;
    memoryTimer=millis();
    state=STATE_MEMORY_INPUT;
    return;
  }

  unsigned long elapsed=millis()-memoryTimer;

  // Show one whole rectangle for 500 ms.
  if(elapsed<500){
    showMemorySymbol(memorySequence[memoryShowPosition]);
    return;
  }

  // Brief blank gap between sequence items.
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

void drawMemoryInput(){
  drawLCD("MEMORY","SELECT "+String(memoryChoice+1)+"/4");

  uint8_t frame[8][12]={};
  drawMemoryBoard(frame);
  showMatrix(frame);

  // Add cursor AFTER the board so the selection indicator is always
  // visually separate from the four rectangles.
  drawMemoryCursor(memoryChoice);
}

void updateMemoryInput(){
  int delta=consumeEncoderDelta();

  if(delta!=0){
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
