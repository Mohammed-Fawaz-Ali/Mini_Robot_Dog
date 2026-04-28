#include <Servo.h>
//
// Pin definitions (original PORTD mapping)
#define RIGHT_FORE_PIN  5
#define RIGHT_HIND_PIN  2
#define LEFT_FORE_PIN   4
#define LEFT_HIND_PIN   3
#define BUTTON_PIN      12

// Servo objects
Servo right_fore;
Servo right_hind;
Servo left_fore;
Servo left_hind;

// Angle variables (°)
int right_fore_angle = 95;
int right_hind_angle = 95;
int left_fore_angle  = 90;
int left_hind_angle  = 90;

void setup() {
  // Attach servos
  right_fore.attach(RIGHT_FORE_PIN);
  right_hind.attach(RIGHT_HIND_PIN);
  left_fore.attach(LEFT_FORE_PIN);
  left_hind.attach(LEFT_HIND_PIN);

  // Home position
  home_position();

  // Wait for button press (HIGH)
  pinMode(BUTTON_PIN, INPUT);
  while (digitalRead(BUTTON_PIN) == LOW) {
    // wait
  }
  delay(50);   // debounce
}

void loop() {
  // left_fore -20°
  for (int i = 0; i < 20; i++) {
    left_fore_angle--;
    update_servos();
    delayMicroseconds(8200);
  }

  // right_fore -20°
  for (int i = 0; i < 20; i++) {
    right_fore_angle--;
    update_servos();
    delayMicroseconds(8200);
  }

  // left_fore +20° (back)
  for (int i = 0; i < 20; i++) {
    left_fore_angle++;
    update_servos();
    delayMicroseconds(8200);
  }

  // right_fore +20° (back)
  for (int i = 0; i < 20; i++) {
    right_fore_angle++;
    update_servos();
    delayMicroseconds(8200);
  }
}

// Write all four servo angles
void update_servos() {
  right_fore.write(right_fore_angle);
  right_hind.write(right_hind_angle);
  left_fore.write(left_fore_angle);
  left_hind.write(left_hind_angle);
}

// Set all servos to 90°
void home_position() {
  right_fore_angle = 95;
  right_hind_angle = 95;
  left_fore_angle  = 90;
  left_hind_angle  = 90;
  update_servos();
}