#include "SettingsApp.h"

namespace {
const int SETTINGS_MAGIC_ADDRESS=12;
const int SETTINGS_DATA_ADDRESS=14;
const uint16_t SETTINGS_MAGIC=0xE551;
struct SettingsRecord { uint8_t sound; uint8_t sensors; };
SettingsRecord settings={1,1};
int settingsIndex=0;
}

bool soundEnabled(){return settings.sound!=0;}
bool sensorsEnabled(){return settings.sensors!=0;}

void loadSettings(){
  uint16_t magic=0;
  EEPROM.get(SETTINGS_MAGIC_ADDRESS,magic);
  if(magic!=SETTINGS_MAGIC){
    settings={1,1};
    EEPROM.put(SETTINGS_MAGIC_ADDRESS,SETTINGS_MAGIC);
    EEPROM.put(SETTINGS_DATA_ADDRESS,settings);
    return;
  }
  EEPROM.get(SETTINGS_DATA_ADDRESS,settings);
  settings.sound=settings.sound?1:0;
  settings.sensors=settings.sensors?1:0;
}

void saveSettings(){EEPROM.put(SETTINGS_DATA_ADDRESS,settings);}

void startSettings(){
  settingsIndex=0;
  state=STATE_SETTINGS;
  invalidateLCD();
  lcd.clear();
  clearEncoderEvents();
}

void returnToSettings(){
  buzzerOff();
  state=STATE_SETTINGS;
  invalidateLCD();
  lcd.clear();
  clearEncoderEvents();
}

void updateSettings(){
  int d=consumeEncoderDelta();
  if(d){
    settingsIndex+=d>0?1:-1;
    if(settingsIndex<0)settingsIndex=1;
    if(settingsIndex>1)settingsIndex=0;
    soundNavigate();
  }

  if(settingsIndex==0){
    drawLCD("SETTINGS > SOUND",settings.sound?"ON":"OFF");
  }else{
    drawLCD("SETTINGS > SENSORS",settings.sensors?"ON":"OFF");
  }

  showEclipseLogo();
  rgbPurple();

  if(consumeEncoderPress()){
    if(settingsIndex==0)settings.sound=!settings.sound;
    else settings.sensors=!settings.sensors;
    saveSettings();
    buzzerOff();
    if(settings.sound) soundSelect();
    invalidateLCD();
    lcd.clear();
  }
}
