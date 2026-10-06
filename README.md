# ECLIPSE CORE v2.2

Modular Arduino UNO R4 WiFi embedded system with LCD UI, LED matrix graphics, sensors, sound, EEPROM storage, and games.

## Open in Arduino IDE

The repository name is `ECLIPSE-CORE-`, so the primary Arduino sketch is intentionally named:

```
ECLIPSE-CORE-.ino
```

Arduino requires the primary `.ino` filename to match the sketch folder name.

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

The project targets the Arduino UNO R4 WiFi and its built-in 12×8 LED matrix. The UNO R4 WiFi officially provides the 12×8 matrix and the Arduino LED Matrix library in its board package.

## Current structure

```text
ECLIPSE-CORE-/
├── ECLIPSE-CORE-.ino
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
├── CoreFeatures.cpp
├── CoreFeatures.h
└── README.md
```

The obsolete duplicate monolithic `Core.cpp/Core.h` implementation has been removed so the modular source files are the only implementations being compiled.

The source has been cleaned up from the earlier generated split, including duplicate-definition problems and the sketch entry-point naming problem. The repository has **not** been represented as compiler-verified because an Arduino UNO R4 WiFi compiler environment is not available in this workspace.
