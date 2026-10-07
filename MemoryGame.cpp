#include "MemoryGame.h"

static uint16_t memoryScore(){
  if(memoryWon&&memoryLevel>=MEMORY_MAX_LEVEL)return MEMORY_MAX_LEVEL;
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

void showMemorySymbol(uint8_t s){
  uint8_t f[8][12]={};

  switch(s){
    case 0:
      for(int y=1;y<4;y++)for(int x=1;x<5;x++)f[y][x]=1;
      break;
    case 1:
      for(int y=1;y<4;y++)for(int x=7;x<11;x++)f[y][x]=1;
      break;
    case 2:
      for(int y=5;y<8;y++)for(int x=1;x<5;x++)f[y][x]=1;
      break;
    case 3:
      for(int y=5;y<8;y++)for(int x=7;x<11;x++)f[y][x]=1;
      break;
  }

  showMatrix(f);
}

void updateMemoryShow(){
  drawLCD("MEMORY","LEVEL "+fixedNumber(memoryScore(),2));

  if(memoryShowPosition>=memoryLevel){
    memoryInputPosition=0;
    memoryChoice=0;
    state=STATE_MEMORY_INPUT;
    clearMatrix();
    return;
  }

  if(millis()-memoryTimer>=650){
    uint8_t s=memorySequence[memoryShowPosition];

    showMemorySymbol(s);
    tone(PIN_BUZZER,700+s*250,100);

    memoryShowPosition++;
    memoryTimer=millis();
  }

  // Keep the most recently shown symbol visible briefly.
  if(
    memoryShowPosition>0 &&
    millis()-memoryTimer<250
  ){
    showMemorySymbol(memorySequence[memoryShowPosition-1]);
  }
}

// A very obvious cursor: a blinking rectangular frame around the
// selected quadrant. The memorized symbol itself never changes,
// so the player can clearly distinguish "what to remember" from
// "what I am currently selecting".
static void drawMemoryCursor(uint8_t q){
  if(((millis()/350)%2)!=0)return;

  int x0=(q%2==0)?0:6;
  int x1=x0+5;
  int y0=(q<2)?0:4;
  int y1=(q<2)?4:7;

  // Top and bottom edges.
  for(int x=x0;x<=x1;x++){
    matrixFrame[y0][x]=1;
    matrixFrame[y1][x]=1;
  }

  // Side edges.
  for(int y=y0;y<=y1;y++){
    matrixFrame[y][x0]=1;
    matrixFrame[y][x1]=1;
  }

  // Moving cursor spark makes encoder movement feel alive.
  uint8_t phase=(millis()/90)%16;
  int sx=x0;
  int sy=y0;

  switch(phase){
    case 0: sx=x0+1; sy=y0; break;
    case 1: sx=x0+2; sy=y0; break;
    case 2: sx=x0+3; sy=y0; break;
    case 3: sx=x0+4; sy=y0; break;
    case 4: sx=x1; sy=y0+1; break;
    case 5: sx=x1; sy=y0+2; break;
    case 6: sx=x1; sy=y0+3; break;
    case 7: sx=x1; sy=y1; break;
    case 8: sx=x0+4; sy=y1; break;
    case 9: sx=x0+3; sy=y1; break;
    case 10: sx=x0+2; sy=y1; break;
    case 11: sx=x0+1; sy=y1; break;
    case 12: sx=x0; sy=y1-1; break;
    case 13: sx=x0; sy=y1-2; break;
    case 14: sx=x0; sy=y1-3; break;
    default: sx=x0; sy=y1-4; break;
  }

  matrixFrame[sy][sx]=1;
}

void drawMemoryInput(){
  drawLCD("MEMORY","LEVEL "+fixedNumber(memoryScore(),2));

  uint8_t f[8][12]={};
  uint8_t q=memoryChoice;

  int x0=(q%2==0)?1:7;
  int y0=(q<2)?1:5;

  // Draw the four selectable symbols.
  for(int y=y0;y<y0+3&&y<8;y++){
    for(int x=x0;x<x0+4;x++){
      f[y][x]=1;
    }
  }

  showMatrix(f);

  // Add the cursor after the game board is drawn, so the cursor
  // can never become part of the remembered symbol.
  drawMemoryCursor(q);
}

void updateMemoryInput(){
  // Consume encoder movement BEFORE drawing so the visual cursor
  // responds on the same loop iteration.
  int d=consumeEncoderDelta();

  if(d){
    // Treat each encoder event as one deliberate quadrant move.
    // Direction is preserved, and wraparound makes all four choices
    // equally reachable.
    memoryChoice += (d>0)?1:-1;

    if(memoryChoice<0)memoryChoice=3;
    if(memoryChoice>3)memoryChoice=0;

    soundNavigate();
  }

  drawMemoryInput();

  if(consumeEncoderPress()){
    uint8_t expected=memorySequence[memoryInputPosition];

    if(memoryChoice!=expected){
      memoryWon=false;
      submitGameScore(2,memoryScore());
      state=STATE_MEMORY_RESULT;
      soundFailure();
      rgbRed();
      return;
    }

    // Correct selection: keep the sounds because they give useful
    // timing feedback without obscuring the visual game state.
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
}

void updateMemoryResult(){
  drawLCD(
    memoryWon?"MEMORY MASTER":"MEMORY FAILED",
    "BEST "+fixedNumber(memoryScore(),2)
  );

  showMatrixNumber(memoryScore());

  if(memoryWon)rgbGreen();
  else rgbRed();

  if(consumeEncoderPress()){
    returnToGamesMenu();
  }
}
