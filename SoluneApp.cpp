#include "SoluneApp.h"

struct SoluneNote {
  uint16_t frequency;
  uint16_t duration;
  uint16_t waitBefore;
};

static const SoluneNote soluneTheme[] = {
  {311, 350, 0},
  {466, 350, 50},
  {349, 350, 50},
  {415, 350, 100},
  {392, 350, 50},
  {311, 350, 50},
  {349, 350, 100},
  {466, 350, 50},
  {523, 350, 50},
  {466, 350, 150},
  {622, 390, 650},
  {392, 350, 100},
  {466, 350, 50},
  {466, 350, 50},
  {415, 350, 50},
  {349, 350, 50},
  {311, 350, 100},
  {311, 350, 50},
  {392, 350, 50},
  {392, 350, 50},
  {392, 350, 100},
  {311, 350, 50},
  {293, 800, 50},
  {311, 180, 0},
  {392, 180, 70},
  {392, 350, 10},
  {392, 180, 500},
  {392, 180, 70},
  {466, 180, 10},
  {392, 350, 10},
  {311, 800, 150},
  {523, 350, 300},
  {783, 350, 50},
  {587, 350, 50},
  {698, 350, 650},
  {659, 350, 50},
  {523, 350, 50},
  {587, 350, 100},
  {783, 350, 50},
  {880, 350, 50},
  {783, 350, 150},
  {783, 350, 650},
  {739, 350, 50},
  {783, 350, 100}
};

static const uint8_t SOLUNE_NOTE_COUNT = sizeof(soluneTheme) / sizeof(soluneTheme[0]);

static uint8_t soluneIndex = 0;
static unsigned long soluneNextEvent = 0;
static bool solunePlaying = false;

void startSolune(){
  soluneIndex = 0;
  soluneNextEvent = millis();
  solunePlaying = true;
  buzzerOff();
  clearEncoderEvents();
  state = STATE_SOLUNE;
  invalidateLCD();
  lcd.clear();
  showEclipseLogo();
  rgbPurple();
}

void updateSoluneApp(){
  drawLCD("SOLUNE","MAIN THEME");
  showEclipseLogo();
  rgbPurple();

  unsigned long now = millis();

  if(solunePlaying && (long)(now - soluneNextEvent) >= 0){
    if(soluneIndex >= SOLUNE_NOTE_COUNT){
      soluneIndex = 0;
      soluneNextEvent = now + 700;
      buzzerOff();
      return;
    }

    const SoluneNote &note = soluneTheme[soluneIndex];

    if(note.waitBefore > 0){
      soluneNextEvent = now + note.waitBefore;
      tone(PIN_BUZZER, note.frequency, note.duration);
    }else{
      tone(PIN_BUZZER, note.frequency, note.duration);
      soluneNextEvent = now + note.duration;
    }

    soluneIndex++;
  }

  if(consumeEncoderPress()){
    buzzerOff();
    solunePlaying = false;
    returnToMainMenu();
  }
}
