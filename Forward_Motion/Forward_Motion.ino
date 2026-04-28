// ============================================================
//  Robot Dog — Forward Gait (Arduino Version)
//  Pin 4 = right_fore  Pin 5 = right_hind
//  Pin 6 = left_fore   Pin 7 = left_hind
//  Pin 12 = start button (active-low)
// ============================================================

#include <Servo.h>

// ── Servo objects ─────────────────────────────────────────────
Servo right_fore_servo;
Servo right_hind_servo;
Servo left_fore_servo;
Servo left_hind_servo;

// ── Servo angle variables ─────────────────────────────────────
int right_fore, right_hind, left_fore, left_hind;

// ── Pin definitions ───────────────────────────────────────────
#define RIGHT_FORE_PIN  5
#define RIGHT_HIND_PIN  2
#define LEFT_FORE_PIN   4
#define LEFT_HIND_PIN   3
#define BUTTON_PIN      12

// ── Prototypes ────────────────────────────────────────────────
void home_position();
void convert();

// =============================================================
//  SETUP
// =============================================================
void setup() {
  right_fore_servo.attach(RIGHT_FORE_PIN);
  right_hind_servo.attach(RIGHT_HIND_PIN);
  left_fore_servo .attach(LEFT_FORE_PIN);
  left_hind_servo .attach(LEFT_HIND_PIN);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  home_position();
  convert();

  // Wait for button press to start
  while (digitalRead(BUTTON_PIN) == LOW);
  delay(50);
}

// =============================================================
//  LOOP — Forward diagonal trot gait
// =============================================================
void loop() {

  // Phase 1 — RF + LH swing forward (20 steps)
  for (int i = 1; i <= 20; i++) {
    right_fore++;
    left_hind--;
    convert();
  }

  // Phase 2 — RH + LF pull through (10 steps)
  for (int i = 1; i <= 10; i++) {
    right_hind--;
    left_fore++;
    convert();
  }

  // Phase 3 — RF + LH return (20 steps)
  for (int i = 1; i <= 20; i++) {
    right_fore--;
    left_hind++;
    convert();
  }

  // Phase 4 — RH + LF return (10 steps)
  for (int i = 1; i <= 10; i++) {
    right_hind++;
    left_fore--;
    convert();
  }

  // Phase 5 — RH + LF swing forward (20 steps)
  for (int i = 1; i <= 20; i++) {
    right_hind++;
    left_fore--;
    convert();
  }

  // Phase 6 — RF + LH pull through (10 steps)
  for (int i = 1; i <= 10; i++) {
    right_fore--;
    left_hind++;
    convert();
  }

  // Phase 7 — RH + LF return (20 steps)
  for (int i = 1; i <= 20; i++) {
    right_hind--;
    left_fore++;
    convert();
  }

  // Phase 8 — RF + LH settle (10 steps)
  for (int i = 1; i <= 10; i++) {
    right_fore++;
    left_hind--;
    convert();
  }
}

// =============================================================
//  home_position()
// =============================================================
void home_position() {
  right_fore = 95;
  right_hind = 95;
  left_fore  = 90;
  left_hind  = 90;
}

// =============================================================
//  convert() — writes angles to all 4 servos
// =============================================================
void convert() {
  right_fore_servo.write(right_fore);
  right_hind_servo.write(right_hind);
  left_fore_servo .write(left_fore);
  left_hind_servo .write(left_hind);
  delayMicroseconds(8200);
}