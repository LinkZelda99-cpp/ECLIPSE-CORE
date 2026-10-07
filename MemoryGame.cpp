#include "MemoryGame.h"

static uint16_t memoryScore(){
  if(memoryWon&&memoryLevel>=MEMORY_MAX_LEVEL)return MEMORY_MAX_LEVEL;
  return memoryLevel>0?memoryLevel-1:0;
}

void startMemoryGame(){
  for(uint8_t i=0;i<MEMORY_MAX_LEVEL;i++)memorySequence[i]=random(0,4);
  memoryLevel=1; memoryShowPosition=0; memoryInputPosition=0; memoryChoice=0; memoryWon=false; memoryTimer=millis();
  state=STATE_MEMORY_SHOW; rgbPurple(); clearEncoderEvents();
}

void showMemorySymbol(uint8_t s){
  uint8_t f[8][12]={};
  switch(s){
    case 0:for(int y=1;y<4;y++)for(int x=1;x<5;x++)f[y][x]=1;break;
    case 1:for(int y=1;y<4;y++)for(int x=7;x<11;x++)f[y][x]=1;break;
    case 2:for(int y=5;y<8;y++)for(int x=1;x<5;x++)f[y][x]=1;break;
    case 3:for(int y=5;y<8;y++)for(int x=7;x<11;x++)f[y][x]=1;break;
  }
  showMatrix(f);
}

void updateMemoryShow(){
  drawLCD("MEMORY","LEVEL "+fixedNumber(memoryScore(),2));
  if(memoryShowPosition>=memoryLevel){
    memoryInputPosition=0; memoryChoice=0; state=STATE_MEMORY_INPUT; clearMatrix(); return;
  }
  if(millis()-memoryTimer>=650){
    uint8_t s=memorySequence[memoryShowPosition];
    showMemorySymbol(s); tone(PIN_BUZZER,700+s*250,100);
    memoryShowPosition++; memoryTimer=millis();
  }
  if(memoryShowPosition>0&&millis()-memoryTimer<250)showMemorySymbol(memorySequence[memoryShowPosition-1]);
}

void drawMemoryInput(){
  drawLCD("MEMORY","LEVEL "+fixedNumber(memoryScore(),2));
  uint8_t f[8][12]={};
  uint8_t q=memoryChoice;
  int x0=(q%2==0)?1:7, y0=(q<2)?1:5;
  for(int y=y0;y<y0+3&&y<8;y++)for(int x=x0;x<x0+4;x++)f[y][x]=1;
  showMatrix(f);
}

void updateMemoryInput(){
  drawMemoryInput();
  int d=consumeEncoderDelta();
  if(d){
    memoryChoice+=d;
    while(memoryChoice<0)memoryChoice+=4;
    while(memoryChoice>=4)memoryChoice-=4;
    soundNavigate();
  }
  if(consumeEncoderPress()){
    uint8_t expected=memorySequence[memoryInputPosition];
    if(memoryChoice!=expected){
      memoryWon=false;
      submitGameScore(2,memoryScore());
      state=STATE_MEMORY_RESULT; soundFailure(); rgbRed(); return;
    }
    soundSelect(); memoryInputPosition++;
    if(memoryInputPosition>=memoryLevel){
      if(memoryLevel>=MEMORY_MAX_LEVEL){
        memoryWon=true;
        submitGameScore(2,memoryScore());
        state=STATE_MEMORY_RESULT; soundSuccess(); rgbGreen(); return;
      }
      memoryLevel++; memoryShowPosition=0; memoryInputPosition=0; memoryChoice=0; memoryTimer=millis(); state=STATE_MEMORY_SHOW; soundSuccess();
    }
  }
}

void updateMemoryResult(){
  drawLCD(memoryWon?"MEMORY MASTER":"MEMORY FAILED","BEST "+fixedNumber(memoryScore(),2));
  showMatrixNumber(memoryScore());
  rgbGreen();
  if(!memoryWon)rgbRed();
  if(consumeEncoderPress())returnToGamesMenu();
}
