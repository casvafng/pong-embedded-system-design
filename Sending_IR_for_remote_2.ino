
#include <IRremote.h>


IRsend irsend;

const int b1  = 4;
const int b2  = 5;
const int b3  = 6;



int timer;
int modeCounter = 0;



void wakeUp() {
  timer = 0;
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(b1, INPUT);
  pinMode(b2, INPUT);
  pinMode(b3, INPUT);
 
}

void loop() {
  attachInterrupt(0, wakeUp, HIGH);
  while (timer < 10000) {
    if (digitalRead(b1) == HIGH) {
      timer = 0;
      delay(50);
      irsend.sendNEC(0x0000AA, 32); // up
    }

    if (digitalRead(b2) == HIGH) {
      timer = 0;
      delay(50);
      irsend.sendNEC(0xDDAADD, 32); // down
    }
     if (digitalRead(b3) == HIGH) {
      timer = 0;
      delay(50);
      irsend.sendNEC(0xFFFFFF, 32); // start
    }

    delay(1);
    timer = timer + 1;

  }

}
