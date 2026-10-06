#include "Storage.h"
void loadHighScore(){EEPROM.get(EEPROM_HIGH_SCORE_ADDRESS,highScore);if(highScore>65000)highScore=0;}
void saveHighScore(){EEPROM.put(EEPROM_HIGH_SCORE_ADDRESS,highScore);}
void submitScore(uint16_t score){if(score>highScore){highScore=score;saveHighScore();}}
