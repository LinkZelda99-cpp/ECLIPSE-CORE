#include "Storage.h"
namespace { const int EEPROM_MAGIC_ADDRESS=0; const uint16_t EEPROM_MAGIC=0xEC42; const int EEPROM_SCORES_ADDRESS=4; }
struct ScoreRecord { uint16_t code; uint16_t react; uint16_t memory; uint16_t snake; };
ScoreRecord scores={0,0,0,0};

void loadHighScores() {
  uint16_t magic=0;
  EEPROM.get(EEPROM_MAGIC_ADDRESS,magic);
  if (magic!=EEPROM_MAGIC) {
    scores={0,0,0,0};
    EEPROM.put(EEPROM_MAGIC_ADDRESS,EEPROM_MAGIC);
    EEPROM.put(EEPROM_SCORES_ADDRESS,scores);
    return;
  }
  EEPROM.get(EEPROM_SCORES_ADDRESS,scores);
  if(scores.code>1000)scores.code=0; if(scores.react>1000)scores.react=0; if(scores.memory>20)scores.memory=0; if(scores.snake>96)scores.snake=0;
}
uint16_t getHighScore(uint8_t game){switch(game){case 0:return scores.code;case 1:return scores.react;case 2:return scores.memory;case 3:return scores.snake;default:return 0;}}
void submitGameScore(uint8_t game,uint16_t score){
  bool changed=false;
  switch(game){
    case 0: if(score>scores.code){scores.code=score;changed=true;} break;
    case 1: if(score>scores.react){scores.react=score;changed=true;} break;
    case 2: if(score>scores.memory){scores.memory=score;changed=true;} break;
    case 3: if(score>scores.snake){scores.snake=score;changed=true;} break;
  }
  if(changed) EEPROM.put(EEPROM_SCORES_ADDRESS,scores);
}