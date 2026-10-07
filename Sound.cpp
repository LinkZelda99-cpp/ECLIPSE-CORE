#include "Sound.h"
void rgbOff(){digitalWrite(PIN_RGB_R,LOW);digitalWrite(PIN_RGB_G,LOW);digitalWrite(PIN_RGB_B,LOW);}
void rgbRed(){digitalWrite(PIN_RGB_R,HIGH);digitalWrite(PIN_RGB_G,LOW);digitalWrite(PIN_RGB_B,LOW);}
void rgbGreen(){digitalWrite(PIN_RGB_R,LOW);digitalWrite(PIN_RGB_G,HIGH);digitalWrite(PIN_RGB_B,LOW);}
void rgbBlue(){digitalWrite(PIN_RGB_R,LOW);digitalWrite(PIN_RGB_G,LOW);digitalWrite(PIN_RGB_B,HIGH);}
void rgbPurple(){digitalWrite(PIN_RGB_R,HIGH);digitalWrite(PIN_RGB_G,LOW);digitalWrite(PIN_RGB_B,HIGH);}
void rgbWhite(){digitalWrite(PIN_RGB_R,HIGH);digitalWrite(PIN_RGB_G,HIGH);digitalWrite(PIN_RGB_B,HIGH);}
void buzzerOff(){noTone(PIN_BUZZER);digitalWrite(PIN_BUZZER,LOW);}
void playTone(uint16_t frequency,uint16_t duration){if(soundEnabled())tone(PIN_BUZZER,frequency,duration);}
void soundNavigate(){playTone(1100,25);}
void soundSelect(){playTone(1500,60);}
void soundSuccess(){playTone(1400,60);delay(70);playTone(1800,80);}
void soundFailure(){playTone(250,120);}
void soundScore(){playTone(1250,35);}
void soundGameOver(){playTone(180,180);}
