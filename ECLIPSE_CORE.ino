/*

============================================================

                    ECLIPSE CORE v2.2

============================================================



Arduino UNO R4 WiFi



MAIN MENU

\---------

\> GAMES

  LIGHT

  DISTANCE

  SCORES

  CORE INFO



GAMES

\-----

\> ECLIPSE CODE

  ECLIPSE REACT

  ECLIPSE MEMORY

  SNAKE



============================================================

PIN MAP

============================================================



ROTARY ENCODER

  DT  -> D2

  CLK -> D3

  SW  -> D5



DHT11

  DATA -> D4

  NOTE: D4 IS RESERVED AND NEVER USED HERE



HC-SR04

  TRIG -> D6

  ECHO -> D7



PASSIVE BUZZER

  + -> D8

  - -> GND



D9

  UNUSED



RGB LED

  RED   -> D10

  GREEN -> D11

  BLUE  -> D12

  COMMON -> GND



LCD1602

  RS -> D13

  E  -> A1

  D4 -> A2

  D5 -> A3

  D6 -> A4

  D7 -> A5



PHOTORESISTOR

  -> A0



UNO R4 WIFI LED MATRIX

  12 x 8 built-in matrix



============================================================

*/



#include \<Arduino.h>

#include \<LiquidCrystal.h>

#include \<Arduino_LED_Matrix.h>

#include \<EEPROM.h>





// ============================================================

// PIN DEFINITIONS

// ============================================================



const uint8_t PIN_ENCODER_DT  = 2;

const uint8_t PIN_ENCODER_CLK = 3;



// D4 belongs to the DHT11.

// DO NOT CONFIGURE OR USE IT.

const uint8_t PIN_DHT11 = 4;



const uint8_t PIN_ENCODER_SW = 5;



const uint8_t PIN_TRIG = 6;

const uint8_t PIN_ECHO = 7;



const uint8_t PIN_BUZZER = 8;



// BACK BUTTON
const uint8_t PIN_BACK = 9;



const uint8_t PIN_RGB_R = 10;

const uint8_t PIN_RGB_G = 11;

const uint8_t PIN_RGB_B = 12;



const uint8_t PIN_LCD_RS = 13;



const uint8_t PIN_LIGHT = A0;



const uint8_t PIN_LCD_E  = A1;

const uint8_t PIN_LCD_D4 = A2;

const uint8_t PIN_LCD_D5 = A3;

const uint8_t PIN_LCD_D6 = A4;

const uint8_t PIN_LCD_D7 = A5;





// ============================================================

// HARDWARE OBJECTS

// ============================================================



LiquidCrystal lcd(

  PIN_LCD_RS,

  PIN_LCD_E,

  PIN_LCD_D4,

  PIN_LCD_D5,

  PIN_LCD_D6,

  PIN_LCD_D7

);



ArduinoLEDMatrix matrix;





// ============================================================

// EEPROM

// ============================================================



const int EEPROM_HIGH_SCORE_ADDRESS = 0;



uint16_t highScore = 0;





// ============================================================

// ECLIPSE LOGO

// ============================================================



uint8_t eclipseLogo[8][12] = {

  {0,0,0,1,1,1,1,1,1,0,0,0},

  {0,0,1,1,1,1,1,1,1,1,0,0},

  {0,1,1,1,1,0,0,1,1,1,1,0},

  {1,1,1,1,0,0,1,1,1,1,1,1},

  {1,1,1,1,0,0,1,1,1,1,1,1},

  {0,1,1,1,1,0,0,1,1,1,1,0},

  {0,0,1,1,1,1,1,1,1,1,0,0},

  {0,0,0,1,1,1,1,1,1,0,0,0}

};





// ============================================================

// APPLICATION STATES

// ============================================================



enum AppState {



  STATE_MENU,

  STATE_GAMES_MENU,



  STATE_CODE,

  STATE_CODE_RESULT,



  STATE_REACT_WAIT,

  STATE_REACT_READY,

  STATE_REACT_RESULT,



  STATE_MEMORY_SHOW,

  STATE_MEMORY_INPUT,

  STATE_MEMORY_RESULT,



  STATE_SNAKE_READY,

  STATE_SNAKE,

  STATE_SNAKE_GAME_OVER,



  STATE_LIGHT,

  STATE_DISTANCE,

  STATE_SCORES,

  STATE_CORE_INFO

};



AppState state = STATE_MENU;





// ============================================================

// FUNCTION PROTOTYPES

// ============================================================



void startCodeGame();

void updateCodeGame();

void updateCodeResult();



void startReactGame();

void updateReactWait();

void updateReactReady();

void updateReactResult();



void startMemoryGame();

void updateMemoryShow();

void updateMemoryInput();

void updateMemoryResult();



void startSnakeGame();

void updateSnake();

void updateSnakeGameOver();



// BACK BUTTON NAVIGATION
// ============================================================

void returnToMainMenu();
void returnToGamesMenu();

void handleBackButton() {

  switch (state) {

    case STATE_MENU:
      // Already at the top level.
      break;

    case STATE_GAMES_MENU:
      returnToMainMenu();
      break;

    case STATE_LIGHT:
    case STATE_DISTANCE:
    case STATE_SCORES:
    case STATE_CORE_INFO:
      returnToMainMenu();
      break;

    case STATE_CODE:
    case STATE_CODE_RESULT:
    case STATE_REACT_WAIT:
    case STATE_REACT_READY:
    case STATE_REACT_RESULT:
    case STATE_MEMORY_SHOW:
    case STATE_MEMORY_INPUT:
    case STATE_MEMORY_RESULT:
    case STATE_SNAKE_READY:
    case STATE_SNAKE:
    case STATE_SNAKE_GAME_OVER:
      buzzerOff();
      returnToGamesMenu();
      break;
  }
}


void handleBackButton();
void returnToMainMenu();

void returnToGamesMenu();





// ============================================================

// MAIN MENU

// ============================================================



const char* mainMenuItems[] = {

  "GAMES",

  "LIGHT",

  "DISTANCE",

  "SCORES",

  "CORE INFO"

};



const uint8_t MAIN_MENU_COUNT = 5;



int mainMenuIndex = 0;

int lastMainMenuIndex = -1;





// ============================================================

// GAMES MENU

// ============================================================



const char* gameMenuItems[] = {

  "ECLIPSE CODE",

  "ECLIPSE REACT",

  "ECLIPSE MEMORY",

  "SNAKE"

};



const uint8_t GAME_MENU_COUNT = 4;



int gameMenuIndex = 0;

int lastGameMenuIndex = -1;





// ============================================================

// ENCODER

// ============================================================



int encoderLastState = 0;

int encoderAccumulator = 0;

int encoderDelta = 0;



bool encoderButtonStable = HIGH;

bool encoderButtonLast = HIGH;



unsigned long encoderButtonTimer = 0;



const unsigned long BUTTON_DEBOUNCE_MS = 35;



bool encoderPressEvent = false;

// Back button
bool backButtonStable = HIGH;
bool backButtonLast = HIGH;
unsigned long backButtonTimer = 0;
bool backPressEvent = false;





// ============================================================

// LCD CACHE

// ============================================================



String lcdCache0 = "";

String lcdCache1 = "";





void invalidateLCD() {



  lcdCache0 = "";

  lcdCache1 = "";

}





String fitLCD(String text) {



  while (text.length() < 16) {

    text += " ";

  }



  if (text.length() > 16) {

    text = text.substring(0, 16);

  }



  return text;

}





void writeLCDLine(uint8_t row, String text) {



  text = fitLCD(text);



  if (row == 0) {



    if (text == lcdCache0) {

      return;

    }



    lcd.setCursor(0, 0);

    lcd.print(text);



    lcdCache0 = text;



  } else {



    if (text == lcdCache1) {

      return;

    }



    lcd.setCursor(0, 1);

    lcd.print(text);



    lcdCache1 = text;

  }

}





void drawLCD(String line0, String line1) {



  writeLCDLine(0, line0);

  writeLCDLine(1, line1);

}





// ============================================================

// RGB

// ============================================================



void rgbOff() {



  digitalWrite(PIN_RGB_R, LOW);

  digitalWrite(PIN_RGB_G, LOW);

  digitalWrite(PIN_RGB_B, LOW);

}





void rgbRed() {



  digitalWrite(PIN_RGB_R, HIGH);

  digitalWrite(PIN_RGB_G, LOW);

  digitalWrite(PIN_RGB_B, LOW);

}





void rgbGreen() {



  digitalWrite(PIN_RGB_R, LOW);

  digitalWrite(PIN_RGB_G, HIGH);

  digitalWrite(PIN_RGB_B, LOW);

}





void rgbBlue() {



  digitalWrite(PIN_RGB_R, LOW);

  digitalWrite(PIN_RGB_G, LOW);

  digitalWrite(PIN_RGB_B, HIGH);

}





void rgbPurple() {



  digitalWrite(PIN_RGB_R, HIGH);

  digitalWrite(PIN_RGB_G, LOW);

  digitalWrite(PIN_RGB_B, HIGH);

}





void rgbWhite() {



  digitalWrite(PIN_RGB_R, HIGH);

  digitalWrite(PIN_RGB_G, HIGH);

  digitalWrite(PIN_RGB_B, HIGH);

}





// ============================================================

// BUZZER

// ============================================================



void buzzerOff() {



  noTone(PIN_BUZZER);

  digitalWrite(PIN_BUZZER, LOW);

}





void soundNavigate() {



  tone(

    PIN_BUZZER,

    1100,

    25

  );

}





void soundSelect() {



  tone(

    PIN_BUZZER,

    1500,

    60

  );

}





void soundSuccess() {



  tone(

    PIN_BUZZER,

    1400,

    60

  );



  delay(70);



  tone(

    PIN_BUZZER,

    1800,

    80

  );

}





void soundFailure() {



  tone(

    PIN_BUZZER,

    250,

    120

  );

}





void soundScore() {



  tone(

    PIN_BUZZER,

    1250,

    35

  );

}





void soundGameOver() {



  tone(

    PIN_BUZZER,

    180,

    180

  );

}





// ============================================================

// MATRIX

// ============================================================



void showMatrix(uint8_t frame[8][12]) {



  matrix.renderBitmap(

    frame,

    8,

    12

  );

}





void clearMatrix() {



  uint8_t blank[8][12] = {};



  showMatrix(blank);

}





void showEclipseLogo() {



  showMatrix(eclipseLogo);

}





// ============================================================

// ENCODER INPUT

// ============================================================



void updateEncoder() {



  int clk =

    digitalRead(PIN_ENCODER_CLK);



  int dt =

    digitalRead(PIN_ENCODER_DT);



  int currentState =

    (clk << 1) | dt;



  if (

    currentState ==

    encoderLastState

  ) {

    return;

  }



  static const int8_t table[16] = {

     0, -1,  1,  0,

     1,  0,  0, -1,

    -1,  0,  0,  1,

     0,  1, -1,  0

  };



  int index =

    (encoderLastState << 2) |

    currentState;



  encoderAccumulator +=

    table[index];



  encoderLastState =

    currentState;



  if (

    encoderAccumulator >= 4

  ) {



    encoderDelta++;



    encoderAccumulator = 0;



  } else if (

    encoderAccumulator <= -4

  ) {



    encoderDelta--;



    encoderAccumulator = 0;

  }

}





void updateEncoderButton() {



  bool reading =

    digitalRead(PIN_ENCODER_SW);



  if (

    reading !=

    encoderButtonLast

  ) {



    encoderButtonTimer =

      millis();



    encoderButtonLast =

      reading;

  }



  if (

    millis() -

    encoderButtonTimer >=

    BUTTON_DEBOUNCE_MS

  ) {



    if (

      reading !=

      encoderButtonStable

    ) {



      encoderButtonStable =

        reading;



      if (

        encoderButtonStable ==

        LOW

      ) {



        encoderPressEvent =

          true;

      }

    }

  }

}





void updateBackButton() {

  bool reading = digitalRead(PIN_BACK);

  if (reading != backButtonLast) {
    backButtonTimer = millis();
    backButtonLast = reading;
  }

  if (millis() - backButtonTimer >= BUTTON_DEBOUNCE_MS) {
    if (reading != backButtonStable) {
      backButtonStable = reading;

      if (backButtonStable == LOW) {
        backPressEvent = true;
      }
    }
  }
}

bool consumeBackPress() {

  if (!backPressEvent) {
    return false;
  }

  backPressEvent = false;
  return true;
}

int consumeEncoderDelta() {



  int result =

    encoderDelta;



  encoderDelta = 0;



  return result;

}





bool consumeEncoderPress() {



  if (

    !encoderPressEvent

  ) {

    return false;

  }



  encoderPressEvent =

    false;



  return true;

}





void clearEncoderEvents() {



  encoderDelta = 0;

  encoderAccumulator = 0;

  encoderPressEvent = false;

  backPressEvent = false;
}





// ============================================================

// EEPROM

// ============================================================



void loadHighScore() {



  EEPROM.get(

    EEPROM_HIGH_SCORE_ADDRESS,

    highScore

  );



  if (

    highScore > 65000

  ) {

    highScore = 0;

  }

}





void saveHighScore() {



  EEPROM.put(

    EEPROM_HIGH_SCORE_ADDRESS,

    highScore

  );

}





void submitScore(

  uint16_t score

) {



  if (

    score >

    highScore

  ) {



    highScore =

      score;



    saveHighScore();

  }

}





// ============================================================

// SENSOR DATA

// ============================================================



int lightReading = 0;



long distanceReading = -1;



unsigned long lastLightRead = 0;

unsigned long lastDistanceRead = 0;





// ============================================================

// LIGHT SENSOR

// ============================================================



void updateLightSensor() {



  if (

    millis() -

    lastLightRead <

    100

  ) {

    return;

  }



  lastLightRead =

    millis();



  lightReading =

    analogRead(PIN_LIGHT);

}





// ============================================================

// ULTRASONIC SENSOR

// ============================================================



long readUltrasonic() {



  digitalWrite(

    PIN_TRIG,

    LOW

  );



  delayMicroseconds(2);



  digitalWrite(

    PIN_TRIG,

    HIGH

  );



  delayMicroseconds(10);



  digitalWrite(

    PIN_TRIG,

    LOW

  );



  unsigned long duration =

    pulseIn(

      PIN_ECHO,

      HIGH,

      18000UL

    );



  if (

    duration == 0

  ) {

    return -1;

  }



  long cm =

    duration / 58;



  if (

    cm < 2 ||

    cm > 300

  ) {

    return -1;

  }



  return cm;

}





void updateDistanceSensor() {



  if (

    millis() -

    lastDistanceRead <

    150

  ) {

    return;

  }



  lastDistanceRead =

    millis();



  distanceReading =

    readUltrasonic();

}





// ============================================================

// MENU RETURN

// ============================================================



void returnToMainMenu() {



  state =

    STATE_MENU;



  lastMainMenuIndex =

    -1;



  invalidateLCD();



  lcd.clear();



  showEclipseLogo();



  rgbPurple();



  clearEncoderEvents();

}





void returnToGamesMenu() {



  state =

    STATE_GAMES_MENU;



  lastGameMenuIndex =

    -1;



  invalidateLCD();



  lcd.clear();



  clearEncoderEvents();

}





// ============================================================

// MAIN MENU

// ============================================================



void drawMainMenu() {



  if (

    mainMenuIndex ==

    lastMainMenuIndex

  ) {

    return;

  }



  lastMainMenuIndex =

    mainMenuIndex;



  drawLCD(

    "ECLIPSE CORE",

    "> " +

    String(

      mainMenuItems[

        mainMenuIndex

      ]

    )

  );



  showEclipseLogo();



  rgbPurple();

}





void updateMainMenu() {



  int delta =

    consumeEncoderDelta();



  if (delta != 0) {



    if (delta > 0) {

      mainMenuIndex++;

    } else {

      mainMenuIndex--;

    }



    if (

      mainMenuIndex <

      0

    ) {



      mainMenuIndex =

        MAIN_MENU_COUNT - 1;

    }



    if (

      mainMenuIndex >=

      MAIN_MENU_COUNT

    ) {



      mainMenuIndex =

        0;

    }



    soundNavigate();

  }



  drawMainMenu();



  if (

    consumeEncoderPress()

  ) {



    soundSelect();



    switch (

      mainMenuIndex

    ) {



      case 0:



        state =

          STATE_GAMES_MENU;



        lastGameMenuIndex =

          -1;



        break;





      case 1:



        state =

          STATE_LIGHT;



        break;





      case 2:



        state =

          STATE_DISTANCE;



        break;





      case 3:



        state =

          STATE_SCORES;



        break;





      case 4:



        state =

          STATE_CORE_INFO;



        break;

    }



    invalidateLCD();



    lcd.clear();

  }

}





// ============================================================

// GAMES MENU

// ============================================================



void drawGamesMenu() {



  if (

    gameMenuIndex ==

    lastGameMenuIndex

  ) {

    return;

  }



  lastGameMenuIndex =

    gameMenuIndex;



  drawLCD(

    "GAMES",

    "> " +

    String(

      gameMenuItems[

        gameMenuIndex

      ]

    )

  );



  showEclipseLogo();



  rgbPurple();

}





void updateGamesMenu() {



  int delta =

    consumeEncoderDelta();



  if (

    delta != 0

  ) {



    if (

      delta > 0

    ) {

      gameMenuIndex++;

    } else {

      gameMenuIndex--;

    }



    if (

      gameMenuIndex < 0

    ) {



      gameMenuIndex =

        GAME_MENU_COUNT - 1;

    }



    if (

      gameMenuIndex >=

      GAME_MENU_COUNT

    ) {



      gameMenuIndex =

        0;

    }



    soundNavigate();

  }



  drawGamesMenu();



  if (

    consumeEncoderPress()

  ) {



    soundSelect();



    switch (

      gameMenuIndex

    ) {



      case 0:



        startCodeGame();



        break;





      case 1:



        startReactGame();



        break;





      case 2:



        startMemoryGame();



        break;





      case 3:



        startSnakeGame();



        break;

    }



    invalidateLCD();



    lcd.clear();

  }

}





// ============================================================

// ECLIPSE CODE

// ============================================================



uint8_t codeSecret[4];

uint8_t codeGuess[4];



uint8_t codePosition = 0;



bool codeWon = false;



unsigned long codeResultStarted = 0;





void startCodeGame() {



  for (

    int i = 0;

    i < 4;

    i++

  ) {



    codeSecret[i] =

      random(0, 10);



    codeGuess[i] = 0;

  }



  codePosition = 0;



  codeWon = false;



  state =

    STATE_CODE;



  rgbBlue();



  clearEncoderEvents();

}





void drawCodeGame() {



  String line0 =

    "CODE ";



  for (

    int i = 0;

    i < 4;

    i++

  ) {



    line0 +=

      String(

        codeGuess[i]

      );

  }



  drawLCD(

    line0,

    "DIGIT " +

    String(

      codePosition + 1

    ) +

    "/4"

  );



  uint8_t frame[8][12] = {};



  for (

    int x = 2;

    x < 10;

    x++

  ) {



    frame[2][x] = 1;

    frame[5][x] = 1;

  }



  frame[3][2] = 1;

  frame[4][2] = 1;



  frame[3][9] = 1;

  frame[4][9] = 1;



  showMatrix(frame);



  rgbBlue();

}





void evaluateCode() {



  codeWon = true;



  for (

    int i = 0;

    i < 4;

    i++

  ) {



    if (

      codeGuess[i] !=

      codeSecret[i]

    ) {



      codeWon = false;

      break;

    }

  }



  codeResultStarted =

    millis();



  state =

    STATE_CODE_RESULT;



  if (

    codeWon

  ) {



    soundSuccess();



    rgbGreen();



  } else {



    soundFailure();



    rgbRed();

  }

}





void updateCodeGame() {



  drawCodeGame();



  int delta =

    consumeEncoderDelta();



  if (

    delta != 0

  ) {



    int value =

      codeGuess[

        codePosition

      ];



    value += delta;



    while (

      value < 0

    ) {

      value += 10;

    }



    while (

      value > 9

    ) {

      value -= 10;

    }



    codeGuess[

      codePosition

    ] =

      value;



    soundNavigate();

  }



  if (

    consumeEncoderPress()

  ) {



    if (

      codePosition < 3

    ) {



      codePosition++;



      soundSelect();



    } else {



      evaluateCode();

    }

  }

}





void updateCodeResult() {



  if (

    codeWon

  ) {



    drawLCD(

      "ACCESS GRANTED",

      "CODE CORRECT!"

    );



    showEclipseLogo();



    rgbGreen();



  } else {



    String answer =

      String(codeSecret[0]) +

      String(codeSecret[1]) +

      String(codeSecret[2]) +

      String(codeSecret[3]);



    drawLCD(

      "ACCESS DENIED",

      answer

    );



    uint8_t frame[8][12] = {};



    for (

      int i = 0;

      i < 8;

      i++

    ) {



      frame[i][i] = 1;

      frame[i][11 - i] = 1;

    }



    showMatrix(frame);



    rgbRed();

  }



  if (

    millis() -

    codeResultStarted >=

    1800

  ) {



    returnToGamesMenu();

  }

}





// ============================================================

// ECLIPSE REACT

// ============================================================



unsigned long reactStarted = 0;

unsigned long reactDelay = 0;

unsigned long reactTime = 0;



bool reactTooEarly = false;





void startReactGame() {



  reactTooEarly = false;



  reactTime = 0;



  reactDelay =

    random(

      1500,

      4500

    );



  reactStarted =

    millis();



  state =

    STATE_REACT_WAIT;



  drawLCD(

    "ECLIPSE REACT",

    "WAIT..."

  );



  showEclipseLogo();



  rgbPurple();



  clearEncoderEvents();

}





void updateReactWait() {



  if (

    consumeEncoderPress()

  ) {



    reactTooEarly =

      true;



    soundFailure();



    state =

      STATE_REACT_RESULT;



    return;

  }



  if (

    millis() -

    reactStarted >=

    reactDelay

  ) {



    state =

      STATE_REACT_READY;



    reactStarted =

      millis();



    drawLCD(

      "GO!",

      "PRESS NOW!"

    );



    uint8_t frame[8][12];



    for (

      int y = 0;

      y < 8;

      y++

    ) {



      for (

        int x = 0;

        x < 12;

        x++

      ) {



        frame[y][x] = 1;

      }

    }



    showMatrix(frame);



    rgbGreen();



    tone(

      PIN_BUZZER,

      1800,

      80

    );

  }

}





void updateReactReady() {



  if (

    consumeEncoderPress()

  ) {



    reactTime =

      millis() -

      reactStarted;



    soundSuccess();



    rgbBlue();



    state =

      STATE_REACT_RESULT;

  }

}





void updateReactResult() {



  if (

    reactTooEarly

  ) {



    drawLCD(

      "TOO EARLY!",

      "PRESS TO RETRY"

    );



    rgbRed();



  } else {



    drawLCD(

      "REACTION TIME",

      String(

        reactTime

      ) +

      " ms"

    );



    showEclipseLogo();



    rgbBlue();

  }



  if (

    consumeEncoderPress()

  ) {



    returnToGamesMenu();

  }

}





// ============================================================

// ECLIPSE MEMORY

// ============================================================



const uint8_t MEMORY_MAX_LEVEL = 20;



uint8_t memorySequence[

  MEMORY_MAX_LEVEL

];



uint8_t memoryLevel = 1;



uint8_t memoryShowPosition = 0;



uint8_t memoryInputPosition = 0;



int memoryChoice = 0;



unsigned long memoryTimer = 0;



bool memoryWon = false;





void startMemoryGame() {



  for (

    int i = 0;

    i < MEMORY_MAX_LEVEL;

    i++

  ) {



    memorySequence[i] =

      random(0, 4);

  }



  memoryLevel = 1;



  memoryShowPosition = 0;



  memoryInputPosition = 0;



  memoryChoice = 0;



  memoryWon = false;



  memoryTimer =

    millis();



  state =

    STATE_MEMORY_SHOW;



  rgbPurple();



  clearEncoderEvents();

}





void showMemorySymbol(

  uint8_t symbol

) {



  uint8_t frame[8][12] = {};



  switch (

    symbol

  ) {



    case 0:



      for (

        int y = 1;

        y < 4;

        y++

      ) {



        for (

          int x = 1;

          x < 5;

          x++

        ) {



          frame[y][x] = 1;

        }

      }



      break;





    case 1:



      for (

        int y = 1;

        y < 4;

        y++

      ) {



        for (

          int x = 7;

          x < 11;

          x++

        ) {



          frame[y][x] = 1;

        }

      }



      break;





    case 2:



      for (

        int y = 5;

        y < 8;

        y++

      ) {



        for (

          int x = 1;

          x < 5;

          x++

        ) {



          frame[y][x] = 1;

        }

      }



      break;





    case 3:



      for (

        int y = 5;

        y < 8;

        y++

      ) {



        for (

          int x = 7;

          x < 11;

          x++

        ) {



          frame[y][x] = 1;

        }

      }



      break;

  }



  showMatrix(frame);

}





void updateMemoryShow() {



  drawLCD(

    "MEMORY",

    "LEVEL " +

    String(

      memoryLevel

    )

  );



  if (

    memoryShowPosition >=

    memoryLevel

  ) {



    memoryInputPosition = 0;



    memoryChoice = 0;



    state =

      STATE_MEMORY_INPUT;



    clearMatrix();



    return;

  }



  if (

    millis() -

    memoryTimer >=

    650

  ) {



    uint8_t symbol =

      memorySequence[

        memoryShowPosition

      ];



    showMemorySymbol(

      symbol

    );



    tone(

      PIN_BUZZER,

      700 +

      symbol * 250,

      100

    );



    memoryShowPosition++;



    memoryTimer =

      millis();

  }

}





void drawMemoryInput() {



  drawLCD(

    "MEMORY",

    "PICK " +

    String(

      memoryChoice + 1

    ) +

    " " +

    String(

      memoryInputPosition + 1

    ) +

    "/" +

    String(

      memoryLevel

    )

  );



  showMemorySymbol(

    memoryChoice

  );

}





void updateMemoryInput() {



  drawMemoryInput();



  int delta =

    consumeEncoderDelta();



  if (

    delta != 0

  ) {



    memoryChoice +=

      delta;



    while (

      memoryChoice < 0

    ) {



      memoryChoice += 4;

    }



    while (

      memoryChoice >= 4

    ) {



      memoryChoice -= 4;

    }



    soundNavigate();

  }





  if (

    consumeEncoderPress()

  ) {



    uint8_t expected =

      memorySequence[

        memoryInputPosition

      ];



    if (

      memoryChoice !=

      expected

    ) {



      memoryWon = false;



      state =

        STATE_MEMORY_RESULT;



      soundFailure();



      rgbRed();



      return;

    }



    soundSelect();



    memoryInputPosition++;



    if (

      memoryInputPosition >=

      memoryLevel

    ) {



      if (

        memoryLevel >=

        MEMORY_MAX_LEVEL

      ) {



        memoryWon = true;



        state =

          STATE_MEMORY_RESULT;



        soundSuccess();



        rgbGreen();



        return;

      }



      memoryLevel++;



      memoryShowPosition = 0;



      memoryInputPosition = 0;



      memoryChoice = 0;



      memoryTimer =

        millis();



      state =

        STATE_MEMORY_SHOW;



      soundSuccess();

    }

  }

}





void updateMemoryResult() {



  if (

    memoryWon

  ) {



    drawLCD(

      "MEMORY MASTER",

      "LEVEL " +

      String(

        memoryLevel

      )

    );



    showEclipseLogo();



    rgbGreen();



  } else {



    drawLCD(

      "MEMORY FAILED",

      "LEVEL " +

      String(

        memoryLevel

      )

    );



    rgbRed();

  }



  if (

    consumeEncoderPress()

  ) {



    returnToGamesMenu();

  }

}





// ============================================================

// SNAKE

// ============================================================



const uint8_t SNAKE_WIDTH = 12;

const uint8_t SNAKE_HEIGHT = 8;

const uint8_t SNAKE_MAX_LENGTH = 96;



struct SnakeSegment {



  int8_t x;

  int8_t y;

};



SnakeSegment snake[

  SNAKE_MAX_LENGTH

];



uint8_t snakeLength = 0;





/*

  0 = UP

  1 = RIGHT

  2 = DOWN

  3 = LEFT

*/



uint8_t snakeDirection = 1;



const int8_t snakeDX[4] = {

  0,

  1,

  0,

 -1

};



const int8_t snakeDY[4] = {

 -1,

  0,

  1,

  0

};





int8_t appleX = 8;

int8_t appleY = 4;



uint16_t snakeScore = 0;



unsigned long lastSnakeMove = 0;

unsigned long lastAppleBlink = 0;



bool appleVisible = true;





/*

  v2.1:

  Slightly slower than v2.0.

*/



const unsigned long SNAKE_MOVE_MS = 425;



const unsigned long APPLE_BLINK_MS = 300;





// ============================================================

// SNAKE APPLE

// ============================================================



void placeApple() {



  if (

    snakeLength >=

    SNAKE_MAX_LENGTH

  ) {



    return;

  }



  bool valid = false;



  while (!valid) {



    appleX =

      random(

        0,

        SNAKE_WIDTH

      );



    appleY =

      random(

        0,

        SNAKE_HEIGHT

      );



    valid = true;



    for (

      uint8_t i = 0;

      i < snakeLength;

      i++

    ) {



      if (

        snake[i].x ==

        appleX &&

        snake[i].y ==

        appleY

      ) {



        valid = false;

        break;

      }

    }

  }

}





// ============================================================

// SNAKE BODY COLLISION

// ============================================================



bool snakeHitsBody(

  int8_t x,

  int8_t y,

  bool growing

) {



  /*

    If we are NOT growing, the tail will

    move away during this tick.



    Therefore the current tail cell is safe.



    If growing, the tail remains, so we

    check the entire body.

  */



  uint8_t count =

    snakeLength;



  if (!growing && count > 0) {

    count--;

  }



  for (

    uint8_t i = 0;

    i < count;

    i++

  ) {



    if (

      snake[i].x == x &&

      snake[i].y == y

    ) {



      return true;

    }

  }



  return false;

}





// ============================================================

// SNAKE RESET

// ============================================================



void resetSnake() {



  snakeLength = 3;



  snakeScore = 0;



  snakeDirection = 1;





  /*

    Head

  */



  snake[0].x = 6;

  snake[0].y = 4;





  /*

    Body

  */



  snake[1].x = 5;

  snake[1].y = 4;



  snake[2].x = 4;

  snake[2].y = 4;





  appleX = 9;

  appleY = 4;



  placeApple();





  lastSnakeMove =

    millis();



  lastAppleBlink =

    millis();



  appleVisible = true;



  buzzerOff();



  rgbGreen();



  clearEncoderEvents();

}





// ============================================================

// START SNAKE

// ============================================================



void startSnakeGame() {



  resetSnake();



  state =

    STATE_SNAKE_READY;



  invalidateLCD();



  lcd.clear();

}





// ============================================================

// SNAKE READY

// ============================================================



void drawSnakeReady() {



  drawLCD(

    "SNAKE",

    "PRESS TO START"

  );



  uint8_t frame[8][12] = {};



  frame[4][3] = 1;

  frame[4][4] = 1;

  frame[4][5] = 1;

  frame[3][5] = 1;



  frame[2][9] = 1;



  showMatrix(frame);



  rgbGreen();

}





// ============================================================

// DRAW SNAKE

// ============================================================



void drawSnake() {



  uint8_t frame[8][12] = {};



  /*

    Draw snake.

  */



  for (

    uint8_t i = 0;

    i < snakeLength;

    i++

  ) {



    frame[

      snake[i].y

    ][

      snake[i].x

    ] = 1;

  }





  /*

    Blinking apple.

  */



  if (

    appleVisible

  ) {



    frame[

      appleY

    ][

      appleX

    ] = 1;

  }



  showMatrix(frame);





  /*

    LCD continuously shows score.

  */



  drawLCD(

    "SNAKE",

    "SCORE: " +

    String(

      snakeScore

    )

  );



  rgbGreen();

}





// ============================================================

// SNAKE TURNING

// ============================================================



void turnSnake(int turn) {



  /*

    +1 = right

    -1 = left

  */



  int newDirection =

    snakeDirection +

    turn;





  if (

    newDirection < 0

  ) {



    newDirection = 3;

  }





  if (

    newDirection > 3

  ) {



    newDirection = 0;

  }





  /*

    Because this is relative steering,

    the player cannot directly reverse

    180 degrees.

  */



  snakeDirection =

    newDirection;



  soundNavigate();

}





// ============================================================

// SNAKE GAME OVER

// ============================================================



void snakeGameOver() {



  submitScore(

    snakeScore

  );



  soundGameOver();



  rgbRed();



  state =

    STATE_SNAKE_GAME_OVER;



  invalidateLCD();

}





// ============================================================

// SNAKE MOVEMENT

// ============================================================



void updateSnakeMovement() {



  /*

    Calculate next head position.

  */



  int8_t newX =

    snake[0].x +

    snakeDX[

      snakeDirection

    ];



  int8_t newY =

    snake[0].y +

    snakeDY[

      snakeDirection

    ];





  /*

    WALL COLLISION

  */



  if (

    newX < 0 ||

    newX >= SNAKE_WIDTH ||

    newY < 0 ||

    newY >= SNAKE_HEIGHT

  ) {



    snakeGameOver();



    return;

  }





  /*

    APPLE CHECK

  */



  bool ateApple =

    (

      newX == appleX &&

      newY == appleY

    );





  /*

    BODY COLLISION



    If eating, the tail stays.

    Otherwise the tail moves away.

  */



  if (

    snakeHitsBody(

      newX,

      newY,

      ateApple

    )

  ) {



    snakeGameOver();



    return;

  }





  /*

    GROWING

  */



  if (

    ateApple &&

    snakeLength <

    SNAKE_MAX_LENGTH

  ) {



    snakeLength++;

  }





  /*

    Shift body backward.

  */



  for (

    int i =

      snakeLength - 1;

    i > 0;

    i--

  ) {



    snake[i] =

      snake[i - 1];

  }





  /*

    Insert new head.

  */



  snake[0].x =

    newX;



  snake[0].y =

    newY;





  /*

    Apple eaten.

  */



  if (

    ateApple

  ) {



    snakeScore++;



    soundScore();



    placeApple();

  }





  lastSnakeMove =

    millis();

}





// ============================================================

// UPDATE SNAKE

// ============================================================



void updateSnake() {



  /*

    Encoder controls turning.

  */



  int delta =

    consumeEncoderDelta();



  if (

    delta != 0

  ) {



    if (

      delta > 0

    ) {



      turnSnake(1);



    } else {



      turnSnake(-1);

    }

  }





  /*

    Apple blink.

  */



  if (

    millis() -

    lastAppleBlink >=

    APPLE_BLINK_MS

  ) {



    lastAppleBlink =

      millis();



    appleVisible =

      !appleVisible;

  }





  /*

    Snake movement.

  */



  if (

    millis() -

    lastSnakeMove >=

    SNAKE_MOVE_MS

  ) {



    updateSnakeMovement();

  }





  if (

    state ==

    STATE_SNAKE

  ) {



    drawSnake();

  }

}





// ============================================================

// SNAKE GAME OVER

// ============================================================



void updateSnakeGameOver() {



  drawLCD(

    "SNAKE GAME OVER",

    "SCORE: " +

    String(

      snakeScore

    )

  );





  /*

    v2.1:

    Proper large centered X across

    the 12x8 LED matrix.

  */



  uint8_t frame[8][12] = {};





  for (

    int y = 0;

    y < 8;

    y++

  ) {



    /*

      Left diagonal.

    */



    int leftX =

      1 +

      (y * 9) / 7;





    /*

      Right diagonal.

    */



    int rightX =

      10 -

      (y * 9) / 7;





    if (

      leftX >= 0 &&

      leftX < 12

    ) {



      frame[y][leftX] = 1;

    }





    if (

      rightX >= 0 &&

      rightX < 12

    ) {



      frame[y][rightX] = 1;

    }

  }





  showMatrix(frame);



  rgbRed();





  if (

    consumeEncoderPress()

  ) {



    startSnakeGame();

  }

}





// ============================================================

// LIGHT APP

// ============================================================



void updateLightApp() {



  updateLightSensor();



  int percent =

    map(

      lightReading,

      0,

      1023,

      0,

      100

    );





  drawLCD(

    "LIGHT",

    String(percent) +

    "% " +

    String(

      lightReading

    )

  );





  uint8_t frame[8][12] = {};



  int bars =

    map(

      lightReading,

      0,

      1023,

      0,

      12

    );





  bars =

    constrain(

      bars,

      0,

      12

    );





  for (

    int x = 0;

    x < bars;

    x++

  ) {



    for (

      int y = 3;

      y < 8;

      y++

    ) {



      frame[y][x] = 1;

    }

  }





  showMatrix(frame);



  rgbBlue();





  if (

    consumeEncoderPress()

  ) {



    returnToMainMenu();

  }

}





// ============================================================

// DISTANCE APP

// ============================================================



void updateDistanceApp() {



  updateDistanceSensor();





  if (

    distanceReading < 0

  ) {



    drawLCD(

      "DISTANCE",

      "NO ECHO"

    );



  } else {



    drawLCD(

      "DISTANCE",

      String(

        distanceReading

      ) +

      " cm"

    );

  }





  uint8_t frame[8][12] = {};





  if (

    distanceReading < 0

  ) {



    for (

      int i = 0;

      i < 8;

      i++

    ) {



      frame[i][i] = 1;



      frame[i][11 - i] =

        1;

    }



  } else {



    int bars =

      map(

        constrain(

          distanceReading,

          2L,

          100L

        ),

        100,

        2,

        1,

        12

      );





    bars =

      constrain(

        bars,

        1,

        12

      );





    for (

      int x = 0;

      x < bars;

      x++

    ) {



      for (

        int y = 2;

        y < 6;

        y++

      ) {



        frame[y][x] =

          1;

      }

    }

  }





  showMatrix(frame);



  rgbBlue();





  if (

    consumeEncoderPress()

  ) {



    returnToMainMenu();

  }

}





// ============================================================

// SCORES APP

// ============================================================



void updateScoresApp() {



  drawLCD(

    "HIGH SCORE",

    String(

      highScore

    ) +

    " POINTS"

  );





  uint8_t frame[8][12] = {};





  int bars =

    highScore % 13;





  if (

    highScore > 0 &&

    bars == 0

  ) {



    bars = 12;

  }





  for (

    int x = 0;

    x < bars;

    x++

  ) {



    for (

      int y = 4;

      y < 8;

      y++

    ) {



      frame[y][x] =

        1;

    }

  }





  showMatrix(frame);



  rgbPurple();





  if (

    consumeEncoderPress()

  ) {



    returnToMainMenu();

  }

}





// ============================================================

// CORE INFO

// ============================================================



void updateCoreInfo() {



  drawLCD(

    "CORE ONLINE",

    "v2.1 R4 WiFi"

  );



  showEclipseLogo();



  rgbPurple();





  if (

    consumeEncoderPress()

  ) {



    returnToMainMenu();

  }

}





// ============================================================

// SETUP

// ============================================================



void setup() {



  // ----------------------------------------------------------

  // Encoder

  // ----------------------------------------------------------



  pinMode(

    PIN_ENCODER_DT,

    INPUT_PULLUP

  );



  pinMode(

    PIN_ENCODER_CLK,

    INPUT_PULLUP

  );



  pinMode(

    PIN_ENCODER_SW,

    INPUT_PULLUP

  );





  // ----------------------------------------------------------

  // ----------------------------------------------------------
  // BACK BUTTON
  // ----------------------------------------------------------

  pinMode(
    PIN_BACK,
    INPUT_PULLUP
  );


  // HC-SR04

  // ----------------------------------------------------------



  pinMode(

    PIN_TRIG,

    OUTPUT

  );



  pinMode(

    PIN_ECHO,

    INPUT

  );



  digitalWrite(

    PIN_TRIG,

    LOW

  );





  // ----------------------------------------------------------

  // Buzzer

  // ----------------------------------------------------------



  pinMode(

    PIN_BUZZER,

    OUTPUT

  );



  buzzerOff();





  // ----------------------------------------------------------

  // RGB

  // ----------------------------------------------------------



  pinMode(

    PIN_RGB_R,

    OUTPUT

  );



  pinMode(

    PIN_RGB_G,

    OUTPUT

  );



  pinMode(

    PIN_RGB_B,

    OUTPUT

  );



  rgbOff();





  // ----------------------------------------------------------

  // LCD

  // ----------------------------------------------------------



  lcd.begin(

    16,

    2

  );



  lcd.clear();





  // ----------------------------------------------------------

  // Matrix

  // ----------------------------------------------------------



  matrix.begin();





  // ----------------------------------------------------------

  // EEPROM

  // ----------------------------------------------------------



  loadHighScore();





  // ----------------------------------------------------------

  // Random seed

  // ----------------------------------------------------------



  randomSeed(

    analogRead(PIN_LIGHT) ^

    micros()

  );





  // ----------------------------------------------------------

  // Encoder state

  // ----------------------------------------------------------



  int clk =

    digitalRead(

      PIN_ENCODER_CLK

    );



  int dt =

    digitalRead(

      PIN_ENCODER_DT

    );



  encoderLastState =

    (clk << 1) | dt;





  encoderButtonStable =

    digitalRead(

      PIN_ENCODER_SW

    );



  encoderButtonLast =

    encoderButtonStable;



  encoderButtonTimer =

    millis();


  backButtonStable =
    digitalRead(PIN_BACK);

  backButtonLast =
    backButtonStable;

  backButtonTimer =
    millis();





  // ----------------------------------------------------------

  // Startup

  // ----------------------------------------------------------



  showEclipseLogo();



  rgbPurple();



  drawLCD(

    "    ECLIPSE",

    "      CORE"

  );





  tone(

    PIN_BUZZER,

    660,

    80

  );



  delay(100);





  tone(

    PIN_BUZZER,

    880,

    80

  );



  delay(100);





  tone(

    PIN_BUZZER,

    1320,

    120

  );



  delay(350);





  buzzerOff();



  lcd.clear();



  invalidateLCD();



  state =

    STATE_MENU;

}





// ============================================================

// MAIN LOOP

// ============================================================



void loop() {



  /*

    Global input handling.

  */



  updateEncoder();



  updateEncoderButton();

  updateBackButton();

  if (consumeBackPress()) {
    handleBackButton();
    return;
  }





  switch (

    state

  ) {



    // ========================================================

    // MAIN MENU

    // ========================================================



    case STATE_MENU:



      updateMainMenu();



      break;





    // ========================================================

    // GAMES MENU

    // ========================================================



    case STATE_GAMES_MENU:



      drawGamesMenu();



      updateGamesMenu();



      break;





    // ========================================================

    // ECLIPSE CODE

    // ========================================================



    case STATE_CODE:



      updateCodeGame();



      break;





    case STATE_CODE_RESULT:



      updateCodeResult();



      break;





    // ========================================================

    // ECLIPSE REACT

    // ========================================================



    case STATE_REACT_WAIT:



      updateReactWait();



      break;





    case STATE_REACT_READY:



      updateReactReady();



      break;





    case STATE_REACT_RESULT:



      updateReactResult();



      break;





    // ========================================================

    // ECLIPSE MEMORY

    // ========================================================



    case STATE_MEMORY_SHOW:



      updateMemoryShow();



      break;





    case STATE_MEMORY_INPUT:



      updateMemoryInput();



      break;





    case STATE_MEMORY_RESULT:



      updateMemoryResult();



      break;





    // ========================================================

    // SNAKE

    // ========================================================



    case STATE_SNAKE_READY:



      drawSnakeReady();





      if (

        consumeEncoderPress()

      ) {



        state =

          STATE_SNAKE;



        invalidateLCD();



        lcd.clear();



        clearEncoderEvents();



        lastSnakeMove =

          millis();

      }



      break;





    case STATE_SNAKE:



      updateSnake();



      break;





    case STATE_SNAKE_GAME_OVER:



      updateSnakeGameOver();



      break;





    // ========================================================

    // LIGHT

    // ========================================================



    case STATE_LIGHT:



      updateLightApp();



      break;





    // ========================================================

    // DISTANCE

    // ========================================================



    case STATE_DISTANCE:



      updateDistanceApp();



      break;





    // ========================================================

    // SCORES

    // ========================================================



    case STATE_SCORES:



      updateScoresApp();



      break;





    // ========================================================

    // CORE INFO

    // ========================================================



    case STATE_CORE_INFO:



      updateCoreInfo();



      break;

  }

}