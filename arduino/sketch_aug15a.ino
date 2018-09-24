
#include <TFT.h> // Hardware-specific library

#define CS   10
#define DC   9
#define RESET  8
#define SIZE 16

char str_buffer[50];
int inByte;
unsigned int board[SIZE];
unsigned long highScore;

TFT myScreen = TFT(CS, DC, RESET);

void drawBoard() {
  myScreen.setTextSize(1);
  myScreen.background(0, 0, 0);
  myScreen.stroke(0, 0, 0);
  for(int i=0; i<4; i++) {
    for(int j=0; j<4; j++) {
      unsigned int tmp = board[i*4 + j] % 255;
      myScreen.fill(0, (tmp *3)%255, (tmp *4)%255);
      myScreen.rect(j*41, i*21, 40, 20);
    }
  }
  
  myScreen.stroke(255, 255, 255);
  for (int i = 0; i < 4; i++) {
    sprintf(str_buffer, "%4d   %4d   %4d   %4d", board[i * 4], board[i * 4 + 1], board[i * 4 + 2], board[i * 4 + 3]);
      myScreen.text(str_buffer, 0, i*21+3);
  }
  myScreen.setTextSize(2);
  myScreen.stroke(100, highScore%255 , (highScore*2)%255); 
  sprintf(str_buffer, "Score: %d", highScore);
  myScreen.text(str_buffer, 10, 100);
}

/**
   Used in the begining or after every move.
   Adds a 2 or 4 block on a random position.

   Returns 0 if OK.
   Returns 1 if GameOver.
 * */
int afterMove() {
  int c = 0;
  for (int i = 0; i < 16; i++)
    if (board[i] == 0)
      c++;
  
  if (c == 0) {
    return 1;
  }

  int randVal = (int) random(0, c);

  for (int i = 0; i < 16; i++) {
    if (randVal == 0 && board[i] == 0) {
      board[i] = ((int) random(1, 3)) * 2;
      break;
    }
    if (randVal != 0 && board[i] == 0)
      randVal--;
  }

  // TODO: Check if a move is possible
  drawBoard();
  return 0;
}

bool moveLeft() {
    bool change = false;
    for(int n=0; n<3; n++) {
        for(int i=0; i<4; i++) {
            for(int j=0; j<3; j++) {
                if( (board[i*4 + j] == 0 && board[i*4 + j+1] != 0) ||
                    (board[i*4 + j] != 0 && board[i*4 + j] == board[i*4 + j+1])) {
                        change = true;
                        if(board[i*4 + j] != 0) {
                          highScore += board[i*4 + j] + board[i*4 + j+1]; 
                        }
                        board[i*4 + j] += board[i*4 + j+1];
                        board[i*4 + j+1] = 0;
                    }
            }
        }
    }
  return change;
}

bool moveRight() {
    bool change = false;
    for(int n = 0; n<3; n++) {
        for(int i = 0; i < 4; i++) {
            for(int j=3; j > 0; j--) {
                if((board[i*4 + j] == 0 && board[i*4 - 1 + j] != 0) ||
                    (board[i*4 + j] != 0 && board[i*4 +j] == board[i*4 - 1 + j])){
                        change = true;
                        if(board[i*4+j] != 0) {
                          highScore += board[i*4+j] * 2;
                        }
                        board[i*4 + j] += board[i*4 - 1 + j];
                        board[i*4 -1 + j] = 0;
                    }
            }
        }
    }
  return change;
}
bool moveUp() {
    bool change = false;
    for(int n=0; n<3; n++) {
        for(int i=0; i<4; i++) {
            for(int j=0; j<3; j++) {
                if( (board[j*4 + i] == 0 && board[j*4 + 4 + i] != 0) ||
                    (board[j*4 + i] != 0 && board[j*4 + i] == board[j*4 + 4 + i])) {
                        change = true;
                        if(board[j*4 + i] != 0) {
                          highScore += board[j*4 + i]*2;
                        }
                        board[j*4 + i] += board[j*4 + 4 + i];
                        board[j*4 + 4 + i] = 0;
                    }
            }
        }
    }
  return change;
}

bool moveDown() {
      bool change = false;
    for(int n = 0; n<3; n++) {
        for(int i = 0; i < 4; i++) {
            for(int j=3; j > 0; j--) {
                if((board[j*4 + i] == 0 && board[j*4 - 4 + i] != 0) ||
                    (board[j*4 + i] != 0 && board[j*4 +i] == board[j*4 - 4 + i])){
                        change = true;
                        if(board[j*4 + i] != 0) {
                          highScore += board[j*4 + i]*2;
                        }
                        board[j*4 + i] += board[j*4 - 4 + i];
                        board[j*4 -4 + i] = 0;
                    }
            }
        }
    }
  return change;
}

void gameOver() {
  myScreen.setTextSize(3);
  myScreen.background(250, 204, 0);

  myScreen.stroke(0, 0, 0);
  myScreen.text("2048", 50, 50);

  Serial.println("GAME OVER");
}

void setup()
{
  // Set up serial. TODO: Replace with bluetooth
  Serial.begin(9600);

  myScreen.begin();

  // Set up random
  randomSeed(analogRead(0));

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  // Set up board
  gameOver();
  delay(5000);
  startGame();
}

void startGame() {
  for (int i = 0; i < SIZE; i++) {
    board[i] = 0;
  }
  highScore = 0;
  afterMove();
}

void loop()
{
  bool validMove = false;

  while (!validMove) {
    while (inByte == 0) {
      while (!Serial.available()) {

      }
      inByte = (int) Serial.read() - '0';

    }
    //Serial.println(inByte);

    if (inByte == 8) {
      validMove = moveUp();
    }
    else if (inByte == 2) {
      validMove = moveDown();
    }
    else if (inByte == 4) {
      validMove = moveLeft();
    }
    else if (inByte == 6) {
      validMove = moveRight();
    }
    else if (inByte == 66) {
      gameOver();
      delay(1000);
      startGame();
    }
    else {
      Serial.println("ERROR");
    }
    inByte = 0;
  }
  Serial.println(highScore);
  validMove = false;
  inByte = 0;

  if (afterMove() == 1) {
    gameOver();
  }
}
