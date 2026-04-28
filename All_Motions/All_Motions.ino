// ============================================================
//  Robot Dog – Forward / Left / Right Gaits (continuous turn)
//  Pin 5 = right_fore, Pin 2 = right_hind
//  Pin 4 = left_fore,  Pin 3 = left_hind
//  Button on pin 12 (active-low) toggles mode
// ============================================================

#include <Servo.h>

Servo right_fore_servo, right_hind_servo, left_fore_servo, left_hind_servo;

int right_fore, right_hind, left_fore, left_hind,count;

#define RIGHT_FORE_PIN  5
#define RIGHT_HIND_PIN  2
#define LEFT_FORE_PIN   4
#define LEFT_HIND_PIN   3
#define BUTTON_PIN      12

void home_position() {
  right_fore = 95;
  right_hind = 95;
  left_fore  = 90;
  left_hind  = 90;
}

void convert() {
  right_fore_servo.write(right_fore);
  right_hind_servo.write(right_hind);
  left_fore_servo.write(left_fore);
  left_hind_servo.write(left_hind);
  delayMicroseconds(8200);
}

// -------------------------------------------------------------
// FORWARD GAIT (original 8‑phase diagonal trot)
// -------------------------------------------------------------
void forwardGait() {
  for (int i = 0; i < 20; i++) { right_fore++; left_hind--; convert(); }
  for (int i = 0; i < 10; i++) { right_hind--; left_fore++; convert(); }
  for (int i = 0; i < 20; i++) { right_fore--; left_hind++; convert(); }
  for (int i = 0; i < 10; i++) { right_hind++; left_fore--; convert(); }
  for (int i = 0; i < 20; i++) { right_hind++; left_fore--; convert(); }
  for (int i = 0; i < 10; i++) { right_fore--; left_hind++; convert(); }
  for (int i = 0; i < 20; i++) { right_hind--; left_fore++; convert(); }
  for (int i = 0; i < 10; i++) { right_fore++; left_hind--; convert(); }
}

void leftTurnGait() {
  for (int i = 0; i < 20; i++) { left_fore++; convert(); }
  for (int i = 0; i < 20; i++) { right_fore++; convert(); }
  for (int i = 0; i < 20; i++) { left_fore--; convert(); }
  for (int i = 0; i < 20; i++) { right_fore--; convert(); }
}

void rightTurnGait() {
  for (int i = 0; i < 20; i++) { left_fore--; convert(); }
  for (int i = 0; i < 20; i++) { right_fore--; convert(); }
  for (int i = 0; i < 20; i++) { left_fore++; convert(); }
  for (int i = 0; i < 20; i++) { right_fore++; convert(); }
}

void backwardGait(){
  for (int i = 1; i <= 20; i++) { right_fore--; left_hind++; convert(); }
  for (int i = 1; i <= 10; i++) { right_hind++; left_fore--; convert(); }
  for (int i = 1; i <= 20; i++) { right_fore++; left_hind--; convert(); }
  for (int i = 1; i <= 10; i++) { right_hind--; left_fore++; convert(); }
  for (int i = 1; i <= 20; i++) { right_hind--; left_fore++; convert(); }
  for (int i = 1; i <= 10; i++) { right_fore++; left_hind--; convert(); }
  for (int i = 1; i <= 20; i++) { right_hind++; left_fore--; convert(); }
  for (int i = 1; i <= 10; i++) { right_fore--; left_hind++; convert(); }
}

// -------------------------------------------------------------
// SETUP & LOOP
// -------------------------------------------------------------
void setup() {
  right_fore_servo.attach(RIGHT_FORE_PIN);
  right_hind_servo.attach(RIGHT_HIND_PIN);
  left_fore_servo.attach(LEFT_FORE_PIN);
  left_hind_servo.attach(LEFT_HIND_PIN);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  home_position();
  convert();
  delay(50);
}

void loop() {
  if (digitalRead(BUTTON_PIN) == HIGH) {
    count++;
    delay(50);
  }

  if(count==1){
    forwardGait();
  }
  else if (count==2){
    leftTurnGait();
  }
  else if (count==3){
    rightTurnGait();
  }
  else if (count==4){
    backwardGait();
  }
  else if (count==5){
    count=1;
  }
}