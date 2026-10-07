#include "MemoryGame.h"

static const unsigned long MEMORY_FLASH_MS = 600;
static const unsigned long MEMORY_GAP_MS = 150;
static const unsigned long MEMORY_INPUT_DELAY_MS = 1000;
static const unsigned int MEMORY_TONES[4] = {700, 950, 1200, 1450};

static uint16_t memoryScore(){
  if(memoryWon && memoryLevel>=MEMORY_MAX_LEVEL)return MEMORY_MAX_LEVEL;
  return memoryLevel>0?memoryLevel-1:0;
}

static void playMemoryTone(uint8_t quadrant){
  tone(PIN_BUZZER, MEMORY_TONES[quadrant & 3], 100);
}

void startMemoryGame(){
  // One complete random pattern is generated at the start.
  // Level N uses the first N entries of that pattern.
  for(uint8_t i=0;i<MEMORY_MAX_LEVEL;i++){
    memorySequence[i]=random(0,4);
  }

  memoryLevel=1;
  memoryShowPosition=0;
  memoryInputPosition=0;
  memoryChoice=0;
  memoryWon=false;
  memoryFlashSounded=false;
  memoryTimer=millis();

  state=STATE_MEMORY_SHOW;
  rgbPurple();
  clearEncoderEvents();
}

static void drawMemoryQuadrants(uint8_t frame[8][12]){
  // Four identical rectangles.
  // The one-pixel gaps keep all four quadrants visually separate.
  for(int y=1;y<=3;y++){
    for(int x=1;x<=4;x++) frame[y][x]=1;
    for(int x=7;x<=10;x++) frame[y][x]=1;
  }

  for(int y=5;y<=7;y++){
    for(int x=1;x<=4;x++) frame[y][x]=1;
    for(int x=7;x<=10;x++) frame[y][x]=1;
  }
}

void showMemorySymbol(uint8_t symbol){
  uint8_t frame[8][12]={};

  // Exactly ONE quadrant is illuminated during the memory phase.
  switch(symbol & 3){
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

static void drawMemoryCursor(uint8_t quadrant){
  // The cursor is only four blinking corner brackets.
  // It sits outside the filled rectangle, so it cannot be mistaken
  // for another remembered flash.
  if(((millis()/350)&1)!=0)return;

  int x0=(quadrant%2==0)?0:6;
  int x1=x0+5;
  int y0=(quadrant<2)?0:4;
  int y1=(quadrant<2)?4:7;

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
  drawLCD("MEMORY","REMEMBER LEVEL "+fixedNumber(memoryLevel,2));

  // After the final flash, leave the matrix blank for a full second
  // before giving the player control.
  if(memoryShowPosition>=memoryLevel){
    clearMatrix();

    if(millis()-memoryTimer>=MEMORY_INPUT_DELAY_MS){
      memoryInputPosition=0;
      memoryChoice=0;
      state=STATE_MEMORY_INPUT;
      clearEncoderEvents();
    }

    return;
  }

  unsigned long elapsed=millis()-memoryTimer;

  if(elapsed<MEMORY_FLASH_MS){
    // One quadrant stays on for the entire flash.
    showMemorySymbol(memorySequence[memoryShowPosition]);

    // Its distinctive sound happens once, at the beginning of the flash.
    if(!memoryFlashSounded){
      playMemoryTone(memorySequence[memoryShowPosition]);
      memoryFlashSounded=true;
    }

    return;
  }

  if(elapsed<MEMORY_FLASH_MS+MEMORY_GAP_MS){
    // Small blank separation: only one quadrant is ever visible at once.
    clearMatrix();
    return;
  }

  // Move to the next flash.
  memoryShowPosition++;
  memoryFlashSounded=false;
  memoryTimer=millis();

  // If that was the last flash, the next phase is the one-second delay.
  if(memoryShowPosition>=memoryLevel){
    clearMatrix();
  }
}

void drawMemoryInput(){
  drawLCD(
    "MEMORY",
    "SELECT "+String(memoryChoice+1)+"/4"
  );

  uint8_t frame[8][12]={};
  drawMemoryQuadrants(frame);
  showMatrix(frame);

  // Add the cursor AFTER the four rectangles.
  drawMemoryCursor((uint8_t)memoryChoice);
}

void updateMemoryInput(){
  // Every encoder step moves exactly one quadrant.
  // Rotation uses only the normal navigation sound.
  int delta=consumeEncoderDelta();

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

  drawMemoryInput();

  if(!consumeEncoderPress())return;

  // The quadrant-specific sound is ONLY played when the player
  // actually selects a quadrant.
  playMemoryTone((uint8_t)memoryChoice);

  uint8_t expected=memorySequence[memoryInputPosition];

  if(memoryChoice!=expected){
    memoryWon=false;
    memoryFlashSounded=false;
    memoryTimer=millis();
    submitGameScore(2,memoryScore());
    state=STATE_MEMORY_RESULT;
    rgbRed();
    return;
  }

  // Correct selection.
  memoryInputPosition++;

  if(memoryInputPosition>=memoryLevel){
    // Every completed level adds exactly one point.
    if(memoryLevel>=MEMORY_MAX_LEVEL){
      memoryWon=true;
      submitGameScore(2,memoryScore());
      state=STATE_MEMORY_RESULT;
      rgbGreen();
      return;
    }

    memoryLevel++;
    memoryShowPosition=0;
    memoryInputPosition=0;
    memoryChoice=0;
    memoryFlashSounded=false;
    memoryTimer=millis();
    state=STATE_MEMORY_SHOW;
    rgbPurple();
  }
}

void updateMemoryResult(){
  drawLCD(
    memoryWon?"MEMORY MASTER":"MEMORY FAILED",
    "SCORE "+fixedNumber(memoryScore(),2)
  );

  showMatrixNumber(memoryScore());

  if(memoryWon)rgbGreen();
  else rgbRed();

  if(consumeEncoderPress()){
    returnToGamesMenu();
  }
}
