# ECLIPSE CORE

ECLIPSE CORE is an expandable embedded system built for the Arduino UNO R4 WiFi, combining a custom LCD interface, built-in LED matrix graphics, sensors, sound, EEPROM storage, and a small collection of games.

## Included
- Main menu and GAMES submenu
- ECLIPSE CODE
- ECLIPSE REACT
- ECLIPSE MEMORY
- SNAKE with blinking apple and EEPROM high score
- Light sensor, distance sensor, scores, and core information apps
- Rotary encoder input and D9 back button
- RGB status LED and intentional buzzer feedback
- UNO R4 WiFi 12x8 Eclipse logo

## Hardware
Target board: **Arduino UNO R4 WiFi**.

## Arduino IDE
1. Clone this repository.
2. Open the repository folder in Arduino IDE.
3. Open `ECLIPSE_CORE.ino`.
4. Select **Arduino UNO R4 WiFi** as the board.
5. Compile and upload.

The project is split into modules so new apps, games, hardware drivers, and storage features can be added without turning the main sketch into one huge file.
