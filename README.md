# Arduino Pedestrian Traffic Light

A simple Arduino project simulating a pedestrian traffic light with 3 LEDs, a button, and a 16×2 I²C LCD.
When the button is pressed, the light runs a timed cycle: red → red+yellow → green.
The number of button presses is saved in EEPROM.

## Build:

Open in Arduino IDE
Board: Arduino Uno
Libraries: LiquidCrystal_I2C, EEPROM (built-in)
Upload and run

## How it Works:

Idle: red LED on, LCD shows “Wait” and press count
Button press: starts countdown (default 10 s, walk = 5 s)
Timer1 ISR generates 1 Hz ticks to update countdown
EEPROM stores press counter across resets

## EEPROM:

Byte 0 → Version
Byte 1 → Total time
Byte 2 → Press counter
