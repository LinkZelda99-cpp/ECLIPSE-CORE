#include "Config.h"
const uint8_t PIN_ENCODER_DT=2,PIN_ENCODER_CLK=3,PIN_DHT11=4,PIN_ENCODER_SW=5,PIN_TRIG=6,PIN_ECHO=7,PIN_BUZZER=8,PIN_BACK=9;
const uint8_t PIN_RGB_R=10,PIN_RGB_G=11,PIN_RGB_B=12,PIN_LCD_RS=13,PIN_LIGHT=A0,PIN_LCD_E=A1,PIN_LCD_D4=A2,PIN_LCD_D5=A3,PIN_LCD_D6=A4,PIN_LCD_D7=A5;
LiquidCrystal lcd(PIN_LCD_RS,PIN_LCD_E,PIN_LCD_D4,PIN_LCD_D5,PIN_LCD_D6,PIN_LCD_D7);
ArduinoLEDMatrix matrix;
uint8_t matrixFrame[8][12]={}; uint8_t matrixScanIndex=0; unsigned long matrixLastScanMicros=0; const unsigned long MATRIX_SCAN_INTERVAL_US=100;
uint8_t eclipseLogo[8][12]={
{0,0,0,1,1,1,1,1,1,0,0,0},{0,0,1,1,1,1,1,1,1,1,0,0},{0,1,1,1,1,0,0,1,1,1,1,0},
{1,1,1,1,0,0,1,1,1,1,1,1},{1,1,1,1,0,0,1,1,1,1,1,1},{0,1,1,1,1,0,0,1,1,1,1,0},
{0,0,1,1,1,1,1,1,1,1,0,0},{0,0,0,1,1,1,1,1,1,0,0,0}};
AppState state=STATE_MENU;
const char* mainMenuItems[]={"GAMES","LIGHT","DISTANCE","SCORES","SONGS","SETTINGS","CORE INFO"}; const uint8_t MAIN_MENU_COUNT=7; int mainMenuIndex=0,lastMainMenuIndex=-1;
const char* gameMenuItems[]={"ECLIPSE CODE","ECLIPSE REACT","ECLIPSE MEMORY","SNAKE"}; const uint8_t GAME_MENU_COUNT=4; int gameMenuIndex=0,lastGameMenuIndex=-1;
const char* scoreMenuItems[]={"ECLIPSE CODE","ECLIPSE REACT","ECLIPSE MEMORY","SNAKE"}; const uint8_t SCORE_MENU_COUNT=4; int scoreMenuIndex=0,lastScoreMenuIndex=-1;
int encoderLastState=0,encoderAccumulator=0,encoderDelta=0;
bool encoderButtonStable=HIGH,encoderButtonLast=HIGH; unsigned long encoderButtonTimer=0; const unsigned long BUTTON_DEBOUNCE_MS=35; bool encoderPressEvent=false;
bool backButtonStable=HIGH,backButtonLast=HIGH; unsigned long backButtonTimer=0; bool backPressEvent=false;
String lcdCache0="",lcdCache1="";
int lightReading=0; long distanceReading=-1; unsigned long lastLightRead=0,lastDistanceRead=0;
uint8_t codeSecret[4],codeGuess[4],codePosition=0,codeAttempts=0,codeExact=0,codeClose=0; bool codeWon=false,codeFinished=false; unsigned long codeResultStarted=0;
unsigned long reactStarted=0,reactDelay=0,reactTime=0; bool reactTooEarly=false;
const uint8_t MEMORY_MAX_LEVEL=20; uint8_t memorySequence[MEMORY_MAX_LEVEL],memoryLevel=1,memoryShowPosition=0,memoryInputPosition=0; int memoryChoice=0; bool memoryWon=false,memoryFlashSounded=false; unsigned long memoryTimer=0;
const uint8_t SNAKE_WIDTH=12,SNAKE_HEIGHT=8,SNAKE_MAX_LENGTH=96; SnakeSegment snake[96]; uint8_t snakeLength=0,snakeDirection=1; const int8_t snakeDX[4]={0,1,0,-1},snakeDY[4]={-1,0,1,0}; int8_t appleX=8,appleY=4; uint16_t snakeScore=0; unsigned long lastSnakeMove=0,lastAppleBlink=0; bool appleVisible=true; const unsigned long SNAKE_MOVE_MS=425,APPLE_BLINK_MS=300;