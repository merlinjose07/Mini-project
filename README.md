# Tic-Tac-Toe Game Using Arduino

A two-player Tic-Tac-Toe game built using an Arduino, a 4x4 matrix keypad and a 20x4 I2C LCD.

## Components Required

- Arduino UNO
- 4x4 matrix keypad
- 20x4 LCD with I2C module
- Jumper wires

## Connections

### Keypad to Arduino UNO

| Keypad pin | Arduino pin |
|---|---|
| Row 1 | D9 |
| Row 2 | D8 |
| Row 3 | D7 |
| Row 4 | D6 |
| Column 1 | D5 |
| Column 2 | D4 |
| Column 3 | D3 |
| Column 4 | D2 |

### I2C LCD to Arduino UNO

| LCD pin | Arduino pin |
|---|---|
| SDA | A4 |
| SCL | A5 |
| VCC | 5V |
| GND | GND |

The sketch uses I2C address `0x27`. Change it in the code if your LCD module uses a different address.

## How to Play

1. The LCD displays the 3x3 game board.
2. Player X starts. Players take turns entering a number from 1 to 9 on the keypad.
3. The number selects the matching position on the board.
4. The first player to complete a row, column or diagonal wins.
5. If all nine positions are filled without a winner, the game is a draw.
6. Press **0** to restart the game.

## Simple Code Explanation

1. The program includes the LCD and keypad libraries and defines the game settings.
2. The `setup()` function starts the LCD and prepares the game.
3. The `loop()` function reads keypad input and processes each move.
4. The `resetBoard()` function clears all nine positions.
5. The `updateDisplay()` function shows the board and game status on the LCD.
6. The `symbolForPlayer()` function displays X, O or a blank space.
7. The `switchPlayers()` function changes the turn after a valid move.
8. The `checkWinCondition()` function checks rows, columns, diagonals and draws.
9. Pressing 0 starts a new game.

## Libraries Required

Install these libraries through the Arduino IDE Library Manager:

- Keypad
- LiquidCrystal_I2C

## Source Code

The Arduino sketch is available in [Code/tic_tac_toe.ino](Code/tic_tac_toe.ino).

## Possible Improvements

LED indicators or a buzzer can be added to provide extra visual or audio feedback.

---
*Academic mini project in Electrical and Electronics Engineering.*
