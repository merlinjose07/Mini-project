#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// LCD: I2C address 0x27, 20 columns and 4 rows
LiquidCrystal_I2C lcd(0x27, 20, 4);

// 3x3 game board
const byte BOARD_SIZE = 3;
const byte PLAYER_NONE = 0;
const byte PLAYER_X = 1;
const byte PLAYER_O = 2;

// 4x4 keypad configuration
const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Keypad rows: D9, D8, D7, D6
// Keypad columns: D5, D4, D3, D2
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys),
                             rowPins, colPins, ROWS, COLS);

byte board[BOARD_SIZE][BOARD_SIZE];
byte currentPlayer = PLAYER_X;
bool gameOver = false;

void setup() {
  lcd.begin(20, 4);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Tic-Tac-Toe Game");
  delay(1500);

  resetGame();
}

void loop() {
  char key = customKeypad.getKey();

  // Press 0 at any time to restart the game
  if (key == '0') {
    resetGame();
    return;
  }

  if (gameOver || key < '1' || key > '9') {
    return;
  }

  // Convert keypad number 1-9 to board row and column
  byte position = key - '1';
  byte row = position / BOARD_SIZE;
  byte col = position % BOARD_SIZE;

  // Accept a move only if the selected cell is empty
  if (board[row][col] == PLAYER_NONE) {
    board[row][col] = currentPlayer;

    if (checkWinCondition()) {
      gameOver = true;
      updateDisplay();
      showMessage(currentPlayer == PLAYER_X ? "Player X wins!" : "Player O wins!");
    } else if (isBoardFull()) {
      gameOver = true;
      updateDisplay();
      showMessage("It's a draw!");
    } else {
      currentPlayer = (currentPlayer == PLAYER_X) ? PLAYER_O : PLAYER_X;
      updateDisplay();
    }
  }
}

void resetGame() {
  resetBoard();
  currentPlayer = PLAYER_X;
  gameOver = false;
  updateDisplay();
}

void resetBoard() {
  for (byte row = 0; row < BOARD_SIZE; row++) {
    for (byte col = 0; col < BOARD_SIZE; col++) {
      board[row][col] = PLAYER_NONE;
    }
  }
}

void updateDisplay() {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Turn: ");
  lcd.print(currentPlayer == PLAYER_X ? 'X' : 'O');
  lcd.print("  0 = Reset");

  for (byte row = 0; row < BOARD_SIZE; row++) {
    lcd.setCursor(0, row + 1);

    for (byte col = 0; col < BOARD_SIZE; col++) {
      lcd.print('|');
      lcd.print(symbolForPlayer(board[row][col]));
    }
    lcd.print('|');
  }
}

char symbolForPlayer(byte player) {
  if (player == PLAYER_X) return 'X';
  if (player == PLAYER_O) return 'O';
  return ' ';
}

bool checkWinCondition() {
  // Check rows and columns
  for (byte i = 0; i < BOARD_SIZE; i++) {
    if (board[i][0] != PLAYER_NONE &&
        board[i][0] == board[i][1] &&
        board[i][1] == board[i][2]) {
      return true;
    }

    if (board[0][i] != PLAYER_NONE &&
        board[0][i] == board[1][i] &&
        board[1][i] == board[2][i]) {
      return true;
    }
  }

  // Check both diagonals
  if (board[0][0] != PLAYER_NONE &&
      board[0][0] == board[1][1] &&
      board[1][1] == board[2][2]) {
    return true;
  }

  if (board[0][2] != PLAYER_NONE &&
      board[0][2] == board[1][1] &&
      board[1][1] == board[2][0]) {
    return true;
  }

  return false;
}

bool isBoardFull() {
  for (byte row = 0; row < BOARD_SIZE; row++) {
    for (byte col = 0; col < BOARD_SIZE; col++) {
      if (board[row][col] == PLAYER_NONE) {
        return false;
      }
    }
  }
  return true;
}

void showMessage(const char *message) {
  lcd.setCursor(0, 0);
  lcd.print("                    "); // Clear 20-character status row
  lcd.setCursor(0, 0);
  lcd.print(message);
  lcd.setCursor(0, 3);
  lcd.print("Press 0 to restart");
}
