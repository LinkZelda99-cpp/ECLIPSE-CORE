# ECLIPSE CORE v2.4

Modular Arduino UNO R4 WiFi embedded system with LCD UI, LED matrix graphics, sensors, sound, EEPROM storage, and games.

## Open in Arduino IDE

The repository is **ECLIPSE-CORE**, and the primary Arduino sketch is:

```
ECLIPSE-CORE.ino
```

There is only one primary `.ino` sketch in the repository. This prevents Arduino IDE from compiling duplicate `setup()` and `loop()` definitions.

Select **Arduino UNO R4 WiFi** as the board.

## Modules

- Config
- Input
- Display
- Sound
- Storage
- Menu
- CoreFeatures
- Games
- CodeGame
- ReactGame
- MemoryGame
- SnakeGame

## Hardware

The project targets the Arduino UNO R4 WiFi and its built-in 12×8 LED matrix. Matrix rendering uses the `Arduino_LED_Matrix` library provided by the UNO R4 board package.

### Pin map

| Pin | Function |
|---|---|
| D2 | Rotary encoder DT |
| D3 | Rotary encoder CLK |
| D4 | DHT11 data |
| D5 | Rotary encoder button |
| D6 | HC-SR04 TRIG |
| D7 | HC-SR04 ECHO |
| D8 | Passive buzzer |
| D9 | Back button |
| D10 | RGB LED red |
| D11 | RGB LED green |
| D12 | RGB LED blue |
| D13 | LCD RS |
| A0 | Photoresistor |
| A1 | LCD E |
| A2 | LCD D4 |
| A3 | LCD D5 |
| A4 | LCD D6 |
| A5 | LCD D7 |

The DHT11 is wired to D4 but is not currently used by the application.

## Features

### Main menu

1. GAMES
2. LIGHT
3. DISTANCE
4. SCORES
5. SONGS
6. CORE INFO

### Games

- **ECLIPSE CODE** — four-digit code guessing game.
- **ECLIPSE REACT** — reaction-time game.
- **ECLIPSE MEMORY** — sequence memory game using the LED matrix.
- **SNAKE** — 12×8 matrix Snake with rotary-encoder turning, blinking apple, collision detection, scoring, and EEPROM high score storage.
- **SONGS** — built-in music player containing SOLUNE, the uploaded HAVEN MIDI adaptation, and public-domain arrangements of Canon in D and Für Elise.

The D9 back button returns from feature/game screens to the appropriate menu.

## Current structure

```text
ECLIPSE-CORE/
├── ECLIPSE-CORE.ino
├── Config.cpp
├── Config.h
├── Input.cpp
├── Input.h
├── Display.cpp
├── Display.h
├── Sound.cpp
├── Sound.h
├── Storage.cpp
├── Storage.h
├── Menu.cpp
├── Menu.h
├── Games.cpp
├── Games.h
├── CodeGame.cpp
├── CodeGame.h
├── ReactGame.cpp
├── ReactGame.h
├── MemoryGame.cpp
├── MemoryGame.h
├── SnakeGame.cpp
├── SnakeGame.h
├── SoluneApp.cpp
├── SoluneApp.h
├── SongsApp.cpp
├── SongsApp.h
├── CoreFeatures.cpp
├── CoreFeatures.h
└── README.md
```

The old `Core.cpp`, `Core.h`, and obsolete `ECLIPSE-CORE-.ino` implementations are not part of the current repository structure.

## Matrix implementation

ECLIPSE CORE uses a manual 8×12 matrix scanner so application frames remain continuously refreshed while the main loop runs. The startup sequence also displays a short X-shaped matrix self-test before entering the application.

The repository has been checked for duplicate sketch entry points and the current source structure. It has **not** been represented as compiler-verified because an Arduino UNO R4 WiFi compiler environment is not available in this workspace.
