#pragma once
#include "Config.h"
void loadHighScores();
uint16_t getHighScore(uint8_t game);
void submitGameScore(uint8_t game,uint16_t score);