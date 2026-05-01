  // ============================================================
  //  Robot Dog – Bluetooth Controlled (no button)
  //  Forward / Left / Right gaits via serial commands
  //  Pin 5 = right_fore, Pin 2 = right_hind
  //  Pin 4 = left_fore,  Pin 3 = left_hind
  //  Bluetooth: RX→10, TX→11 (SoftwareSerial)
  // ============================================================

  #include <Servo.h>
  #include <SoftwareSerial.h>

  // Servo objects
  Servo right_fore_servo, right_hind_servo, left_fore_servo, left_hind_servo;

  // Angle variables
  int right_fore, right_hind, left_fore, left_hind;

  // Gait mode: 1=Forward, 2=Left turn, 3=Right turn
  int mode;                     // start with forward

  // Pin definitions for servos
  #define RIGHT_FORE_PIN  5
  #define RIGHT_HIND_PIN  2
  #define LEFT_FORE_PIN   4
  #define LEFT_HIND_PIN   3

  // Bluetooth serial on pins 10 (RX) and 11 (TX)
  SoftwareSerial bluetooth(10, 11); // RX, TX

  // ============================================================
  //  Setup
  // ============================================================
  void setup() {
    // Attach servos
    Serial.begin(9600);
    right_fore_servo.attach(RIGHT_FORE_PIN);
    right_hind_servo.attach(RIGHT_HIND_PIN);
    left_fore_servo.attach(LEFT_FORE_PIN);
    left_hind_servo.attach(LEFT_HIND_PIN);

    // Start Bluetooth communication
    bluetooth.begin(9600);          // match your HC‑05/HC‑06 baud rate

    // Set initial home angles
    home_position();
    convert();
  }

  // ============================================================
  //  Main Loop – listen for Bluetooth, run current gait
  // ============================================================
  void loop() {
    // Check for incoming Bluetooth commands
    if (bluetooth.available()) {
      char cmd = bluetooth.read();
      Serial.println(cmd);
      switch (cmd) {
        case 'F': case 'f': mode = 1; break;   // Forward
        case 'L': case 'l': mode = 2; break;   // Left turn
        case 'R': case 'r': mode = 3; break;   // Right turn
        case 'B': case 'b': mode = 4; break;   // Right turn
        case 'S': case 's': mode = 5; break;   // Stop
      }
    }

    // Execute the currently selected gait continuously
    switch (mode) {
      case 1: forwardGait();  break;
      case 2: leftTurnGait(); break;
      case 3: rightTurnGait(); break;
      case 4: backwardGait(); break;
      case 5: home_position(); break;
    }
  }

  // ============================================================
  //  Gait Implementations
  // ============================================================

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

  // ============================================================
  //  Helper Functions
  // ============================================================
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