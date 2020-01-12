#include <Wire.h>                  // This library allows you to communicate with I2C
#include <Adafruit_GFX.h>          // Adafruit GFX graphics core library
#include <Adafruit_SSD1306.h>      // Driver library for 'monochrome' 128x64 and 128x32 OLEDs
#include <IRremote.h>              //Library for IR remote


// Define variables

int gameState = 0;                 // 0 = Home, 1 = Game, 2 = End

int paddlePositionPlayer1 = 0;
int paddlePositionPlayer2 = 0;

int scorePlayer1 = 0;
int scorePlayer2 = 0;

int ballX = 64;
int ballY = 16;
int ballSpeedX = 2;
int ballSpeedY = 2;

int dotX = 4;
int dotY = 27;

int controlType = 1;

//Define receiving IR signal PIN
int RECV_PIN = 12;




#define OLED_RESET 4

Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)

#endif

IRrecv irrecv(RECV_PIN);
decode_results results;



void setup() {

  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x64)
  display.clearDisplay();
  irrecv.enableIRIn();

}

void loop() {

  // Player 1 control

  if (irrecv.decode(&results)) {
    switch (results.value) {
      case 0xAA1000: //Keypad up
        if (paddlePositionPlayer1 < 32 - 10) //Check if touch top wall
        {
          paddlePositionPlayer1 = paddlePositionPlayer1 + 2; //Either move fast (2 tiles) or slow (1 tile)
        }
        break;
      case 0xCC4470: //Keypad down
        if (paddlePositionPlayer1 > 0) //Check if touch bottom wall
        {
          paddlePositionPlayer1 = paddlePositionPlayer1 - 2; //Either move fast (2 tiles) or slow (1 tile)
        }
        break;
      case 0xBB88BB: //Keypad up
        if (paddlePositionPlayer2 < 32 - 10) //Check if touch top wall
        {
          paddlePositionPlayer2 = paddlePositionPlayer2 + 2; //Either move fast (2 tiles) or slow (1 tile)
        }
        break;
      case 0xDDAADD: //Keypad down
        if (paddlePositionPlayer2 > 0) //Check if touch bottom wall
        {
          paddlePositionPlayer2 = paddlePositionPlayer2 - 2; //Either move fast (2 tiles) or slow (1 tile)
        }
        break;
      case 0xFFFFFF: //Keypad enter
        if (gameState == 2) {
          gameState = 0;
          scorePlayer1 = 0;
          scorePlayer2 = 0;
          ballX = 64;
          ballY = 16;
          paddlePositionPlayer1 = 0;
          paddlePositionPlayer2 = 0;
          delay(100);
        }
        break;

    }
    irrecv.resume();

  }

  switch (controlType) {
    case 1:
      //Player 2 AI
      if (ballY - 4 < paddlePositionPlayer2) {
        paddlePositionPlayer2 = paddlePositionPlayer2 - 2;
      }

      if (ballY + 4 > paddlePositionPlayer2 + 10) {
        paddlePositionPlayer2 = paddlePositionPlayer2 + 2;
      }
      break;


    case 2:
      //Player 2 control
      if (irrecv.decode(&results)) {
        switch (results.value) {
          case 0x0000AA: //Keypad up
            if (paddlePositionPlayer2 < 32 - 10) //Check if touch top wall
            {
              paddlePositionPlayer2 = paddlePositionPlayer2 + 2; //Either move fast (2 tiles) or slow (1 tile)
            }
            break;
          case 0xDDAADD: //Keypad down
            if (paddlePositionPlayer2 > 0) //Check if touch bottom wall
            {
              paddlePositionPlayer2 = paddlePositionPlayer2 - 2; //Either move fast (2 tiles) or slow (1 tile)
            }
            break;
        }
        irrecv.resume();


      }
      break;
  }
  if (gameState == 0) {
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(44, 6);
    display.println("PONG");
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(8, 24);
    display.println("1 player");
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(80, 24);
    display.println("2 player");
    display.display();
    display.clearDisplay();
    display.fillRect(dotX, dotY, 2, 2, 1);
    if (irrecv.decode(&results)) {
      switch (results.value) {
        case 0xCC4470: //Keypad down
          if (dotX == 4)  //Check if choose first player
          {
            dotX = 76;
            controlType = 2;                  //Move to 2 players mode
          }
          break;
        case 0xAA1000: //Keypad up
          if (dotX == 76) //Check if choose 2 players
          {
            dotX = 4;
            controlType = 1;                  //Move to 1 players mode
          }
          break;
        case 0xDDAADD: //Keypad down
          if (dotX == 4)  //Check if choose first player
          {
            dotX = 76;
            controlType = 2;                  //Move to 2 players mode
          }
          break;
        case 0xBB88BB: //Keypad up
          if (dotX == 76) //Check if choose 2 players
          {
            dotX = 4;
            controlType = 1;                  //Move to 1 players mode
          }
          break;

        case 0xFFFFFF: //Keypad enter
          if (gameState == 0) {
            gameState = 1;
            delay(100);
          }
          break;
      }
      irrecv.resume();
    }
  }
  if (gameState == 1) {
    drawField(scorePlayer1, scorePlayer2);

    collisionControl();
    drawBall();

    display.display();
    display.clearDisplay();

  }




  if (gameState == 2) {
    drawField(scorePlayer1, scorePlayer2);

    display.setTextSize(1);
    display.setTextColor(WHITE);

    if (scorePlayer1 == 2) {
      display.setCursor(15, 15);
    } else if (scorePlayer2 == 2) {
      display.setCursor(77, 15);
    }
    display.println("winner!");
    display.display();
    display.clearDisplay();
  }


}

void drawField(int score1, int score2) {
  display.fillRect(0, paddlePositionPlayer1, 2, 10, 1);
  display.fillRect(126, paddlePositionPlayer2, 2, 10, 1);

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(55, 0);
  display.print(score1);
  display.print(":");
  display.print(score2);

  display.fillRect(63, 12, 1, 5, 1);
  display.fillRect(63, 22, 1, 5, 1);
  display.fillRect(63, 32, 1, 5, 1);
  display.fillRect(63, 42, 1, 5, 1);
  display.fillRect(63, 52, 1, 5, 1);
  display.fillRect(63, 62, 1, 5, 1);
}



void collisionControl() {
  //bounce from top and bottom
  if (ballY <= 0) {
    ballSpeedY *= -1;
  }

  if (ballY >= 32 - 2) {
    ballSpeedY *= -1;
  }

  //score points if ball hits wall behind player
  if (ballX >= 128 - 2 || ballX <= 0) {
    if (ballSpeedX > 0) {
      scorePlayer1++;
      ballX = 128 / 4;

    }
    if (ballSpeedX < 0) {
      scorePlayer2++;
      ballX = 128 / 4 * 3;

    }
    if (scorePlayer1 == 2 || scorePlayer2 == 2) {
      gameState = 2;
    }
  }

  //bounce from player1
  if (ballX >= 0 && ballX <= 2 && ballSpeedX < 0) {
    if (ballY > paddlePositionPlayer1 - 2 && ballY < paddlePositionPlayer1 + 10) {
      ballSpeedX *= -1;
    }
  }
  //bounce from player2
  if (ballX >= 128 - 2 - 2 && ballX <= 128 - 2 && ballSpeedX > 0) {
    if (ballY > paddlePositionPlayer2 - 2 && ballY < paddlePositionPlayer2 + 10) {
      ballSpeedX *= -1;
    }

  }

}
void drawBall() {

  display.fillRect(ballX, ballY, 2, 2, 1);

  ballX += ballSpeedX;
  ballY += ballSpeedY;
}
