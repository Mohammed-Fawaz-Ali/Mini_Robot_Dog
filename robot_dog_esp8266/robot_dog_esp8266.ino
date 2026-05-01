// ============================================================
//  Robot Dog – ESP8266 Web Dashboard with Stretch, Sit, Lean
//  Connect to "RobotDog" WiFi → open 192.168.4.1
//
//  D1 (GPIO5) = right_fore
//  D2 (GPIO4) = right_hind
//  D3 (GPIO0) = left_fore   ⚠ boot pin – keep servo unpowered at boot
//  D4 (GPIO2) = left_hind   ⚠ boot pin – keep servo unpowered at boot
// ============================================================

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

const char* AP_SSID = "RobotDog";
const char* AP_PASS = "12345678";

ESP8266WebServer server(80);

Servo right_fore_servo, right_hind_servo, left_fore_servo, left_hind_servo;

int right_fore, right_hind, left_fore, left_hind;
volatile int mode = 5;  // 1=F 2=L 3=R 4=B 5=Stop 6=Stretch 7=Sit 8=LeanFwd 9=LeanBwd 10=LeanRock 11=Happy 12=Bow 13=ShakeHand

#define RIGHT_FORE_PIN  5
#define RIGHT_HIND_PIN  4
#define LEFT_FORE_PIN   0
#define LEFT_HIND_PIN   2

// ========== HTML Dashboard with 10 buttons ==========
const char DASHBOARD[] PROGMEM = R"====(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1.0,user-scalable=no">
<title>Robot Dog</title>
<style>
  @import url('https://fonts.googleapis.com/css2?family=Share+Tech+Mono&family=Orbitron:wght@700;900&display=swap');
  :root {
    --bg:      #080b10;
    --panel:   #0f1318;
    --border:  #1c2535;
    --accent:  #00e5ff;
    --warn:    #ff3d71;
    --text:    #b0c4d8;
    --dim:     #3a4a5c;
  }
  * { box-sizing:border-box; margin:0; padding:0; }
  body {
    background: var(--bg);
    color: var(--text);
    font-family: 'Share Tech Mono', monospace;
    min-height: 100vh;
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    padding: 24px 16px;
  }
  h1 {
    font-family: 'Orbitron', sans-serif;
    font-size: clamp(1.6rem, 6vw, 2.4rem);
    font-weight: 900;
    letter-spacing: .18em;
    color: var(--accent);
    text-shadow: 0 0 20px rgba(0,229,255,.5);
    text-align: center;
    margin-bottom: 4px;
  }
  .status {
    display: flex;
    align-items: center;
    gap: 10px;
    background: var(--panel);
    border: 1px solid var(--border);
    border-radius: 6px;
    padding: 8px 20px;
    margin-bottom: 28px;
    font-size: .72rem;
  }
  .dot {
    width: 8px; height: 8px;
    border-radius: 50%;
    background: var(--accent);
    box-shadow: 0 0 8px var(--accent);
    animation: blink 1.8s ease-in-out infinite;
  }
  @keyframes blink { 0%,100%{opacity:1} 50%{opacity:.2} }
  #lbl { color: var(--accent); font-weight:bold; min-width:90px; }

  .grid {
    display: grid;
    grid-template-columns: repeat(3, 90px);
    gap: 10px;
    margin-bottom: 20px;
  }
  .btn {
    background: var(--panel);
    border: 1px solid var(--border);
    border-radius: 12px;
    color: var(--text);
    font-family: 'Share Tech Mono', monospace;
    font-size: .65rem;
    letter-spacing: .1em;
    cursor: pointer;
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    gap: 6px;
    padding: 12px 0;
    transition: all 0.1s;
  }
  .btn:active, .btn.active {
    background: rgba(0,229,255,.12);
    border-color: var(--accent);
    transform: scale(.94);
    color: var(--accent);
  }
  .stop:active, .stop.active {
    background: rgba(255,61,113,.12);
    border-color: var(--warn);
    color: var(--warn);
  }
  .footer {
    margin-top: 20px;
    font-size: .6rem;
    color: var(--dim);
    text-align: center;
  }
</style>
</head>
<body>
<h1>🐕 ROBOT DOG</h1>
<div class="status">
  <div class="dot"></div>
  <span>MODE:&nbsp;</span>
  <span id="lbl">STOPPED</span>
</div>

<div class="grid">
  <div></div>
  <button class="btn" id="btn-F" onclick="send('F')">▲<br>FORWARD</button>
  <div></div>
  <button class="btn" id="btn-L" onclick="send('L')">◀<br>LEFT</button>
  <button class="btn stop" id="btn-S" onclick="send('S')">■<br>STOP</button>
  <button class="btn" id="btn-R" onclick="send('R')">▶<br>RIGHT</button>
  <div></div>
  <button class="btn" id="btn-B" onclick="send('B')">▼<br>BACK</button>
  <div></div>
</div>

<div class="grid" style="grid-template-columns: repeat(3, 90px); margin-top: 10px;">
  <button class="btn" id="btn-ST" onclick="send('T')">⬆️<br>STRETCH</button>
  <button class="btn" id="btn-SI" onclick="send('I')">🪑<br>SIT</button>
  <button class="btn" id="btn-LF" onclick="send('E')">⏩<br>LEAN→</button>
  <button class="btn" id="btn-LB" onclick="send('Q')">⏪<br>←LEAN</button>
</div>

<div class="grid" style="grid-template-columns: repeat(3, 90px); margin-top: 10px;">
  <button class="btn" id="btn-RK" onclick="send('K')">🔄<br>LEAN ROCK</button>
  <button class="btn" id="btn-HP" onclick="send('H')">🐾<br>HAPPY</button>
  <div></div>
</div>

<!-- NEW BUTTONS ROW for Bow and Shake Hand -->
<div class="grid" style="grid-template-columns: repeat(3, 90px); margin-top: 10px;">
  <button class="btn" id="btn-BW" onclick="send('O')">🙇<br>BOW</button>
  <button class="btn" id="btn-SH" onclick="send('P')">🖐️<br>SHAKE</button>
  <div></div>
</div>

<div class="footer">TAP – new pose | Movement buttons override</div>

<script>
  const labels = {
    F:'FORWARD', B:'BACKWARD', L:'LEFT TURN', R:'RIGHT TURN', S:'STOPPED',
    T:'STRETCH', I:'SIT', E:'LEAN FWD', Q:'LEAN BWD',
    K:'LEAN ROCK', H:'HAPPY',
    O:'BOW', P:'SHAKE HAND'
  };
  const btnIds = { F:'btn-F', B:'btn-B', L:'btn-L', R:'btn-R', S:'btn-S',
                   T:'btn-ST', I:'btn-SI', E:'btn-LF', Q:'btn-LB',
                   K:'btn-RK', H:'btn-HP',
                   O:'btn-BW', P:'btn-SH' };
  let activeBtn = null;

  function send(cmd) {
    if (activeBtn) document.getElementById(activeBtn).classList.remove('active');
    activeBtn = btnIds[cmd];
    if (activeBtn) document.getElementById(activeBtn).classList.add('active');
    document.getElementById('lbl').textContent = labels[cmd] || cmd;
    fetch('/cmd?v=' + cmd).catch(() => {});
  }

  document.addEventListener('keydown', e => {
    if      (e.key==='ArrowUp'||e.key==='w') send('F');
    else if (e.key==='ArrowDown'||e.key==='s') send('B');
    else if (e.key==='ArrowLeft'||e.key==='a') send('L');
    else if (e.key==='ArrowRight'||e.key==='d') send('R');
    else if (e.key===' ') send('S');
    else if (e.key==='t') send('T');
    else if (e.key==='i') send('I');
    else if (e.key==='e') send('E');
    else if (e.key==='q') send('Q');
    else if (e.key==='k') send('K');
    else if (e.key==='h') send('H');
    else if (e.key==='o') send('O');   // bow
    else if (e.key==='p') send('P');   // shake hand
  });
</script>
</body>
</html>
)====";

// ========== Web Routes ==========
void handleRoot() {
  server.send_P(200, "text/html", DASHBOARD);
}

void handleCmd() {
  if (server.hasArg("v")) {
    char cmd = server.arg("v")[0];
    switch (cmd) {
      case 'F': case 'f': mode = 1; break;
      case 'L': case 'l': mode = 2; break;
      case 'R': case 'r': mode = 3; break;
      case 'B': case 'b': mode = 4; break;
      case 'S': case 's': mode = 5; home_position(); convert_once(); break;
      case 'T': case 't': mode = 6; break;
      case 'I': case 'i': mode = 7; break;
      case 'E': case 'e': mode = 8; break;
      case 'Q': case 'q': mode = 9; break;
      case 'K': case 'k': mode = 10; break;
      case 'H': case 'h': mode = 11; break;
      case 'O': case 'o': mode = 12; break;   // NEW: bow
      case 'P': case 'p': mode = 13; break;   // NEW: shake hand
    }
  }
  server.send(200, "text/plain", "OK");
}

// ========== Setup ==========
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  right_fore_servo.attach(RIGHT_FORE_PIN);
  right_hind_servo.attach(RIGHT_HIND_PIN);
  left_fore_servo.attach(LEFT_FORE_PIN);
  left_hind_servo.attach(LEFT_HIND_PIN);

  home_position();
  convert_once();

  server.on("/", handleRoot);
  server.on("/cmd", handleCmd);
  server.begin();
  Serial.println("HTTP started");
}

// ========== Main Loop ==========
void loop() {
  server.handleClient();
  switch (mode) {
    case 1: forwardGait(); backwardGait();   break;
    case 2: rightTurnGait();  break;
    case 3: leftTurnGait(); break;
    case 4: forwardGait();  break;
    case 5: /* stopped */    break;
    case 6: stretchPose();   break;
    case 7: sitPose();       break;
    case 8: leanForward();   break;
    case 9: leanBackward();  break;
    case 10: leanRock();     break;
    case 11: happyWiggle();  break;
    case 12: bow();          break;   // NEW
    case 13: shakeHand();    break;   // NEW
  }
}

// ========== Gaits (unchanged from your working version) ==========
void forwardGait() {
  const int step = 5;
  const int iterA = 4, iterB = 2;
  for (int i = 0; i < iterA; i++) { right_fore += step; left_hind -= step; convert(); }
  for (int i = 0; i < iterB; i++) { right_hind -= step; left_fore += step; convert(); }
  for (int i = 0; i < iterA; i++) { right_fore -= step; left_hind += step; convert(); }
  for (int i = 0; i < iterB; i++) { right_hind += step; left_fore -= step; convert(); }
  for (int i = 0; i < iterA; i++) { right_hind += step; left_fore -= step; convert(); }
  for (int i = 0; i < iterB; i++) { right_fore -= step; left_hind += step; convert(); }
  for (int i = 0; i < iterA; i++) { right_hind -= step; left_fore += step; convert(); }
  for (int i = 0; i < iterB; i++) { right_fore += step; left_hind -= step; convert(); }
}

void leftTurnGait() {
  const int step = 5;
  const int iter = 8;
  for (int i = 0; i < iter; i++) { left_fore  += step; convert(); }
  for (int i = 0; i < iter; i++) { right_fore += step; convert(); }
  for (int i = 0; i < iter; i++) { left_fore  -= step; convert(); }
  for (int i = 0; i < iter; i++) { right_fore -= step; convert(); }
}

void rightTurnGait() {
  const int step = 5;
  const int iter = 6;
  for (int i = 0; i < iter; i++) { left_fore -= step; left_hind -= step; right_fore -= step; right_hind -= step; convert(); }
  for (int i = 0; i < iter; i++) { left_fore += step; left_hind += step; right_fore += step; right_hind += step; convert(); }
}

void backwardGait() {
  const int step = 5;
  const int iterA = 4, iterB = 2;
  for (int i = 0; i < iterA; i++) { right_fore -= step; left_hind += step; convert(); }
  for (int i = 0; i < iterB; i++) { right_hind += step; left_fore -= step; convert(); }
  for (int i = 0; i < iterA; i++) { right_fore += step; left_hind -= step; convert(); }
  for (int i = 0; i < iterB; i++) { right_hind -= step; left_fore += step; convert(); }
  for (int i = 0; i < iterA; i++) { right_hind -= step; left_fore += step; convert(); }
  for (int i = 0; i < iterB; i++) { right_fore += step; left_hind -= step; convert(); }
  for (int i = 0; i < iterA; i++) { right_hind += step; left_fore -= step; convert(); }
  for (int i = 0; i < iterB; i++) { right_fore -= step; left_hind += step; convert(); }
}

// ========== New Poses ==========
void stretchPose() {
  right_fore = 180;
  right_hind = 0;
  left_fore  = 0;
  left_hind  = 180;
  convert();
}

void sitPose() {
  right_fore = 0;
  right_hind = 90;
  left_fore  = 180;
  left_hind  = 90;
  convert();
}

void leanForward() {
  int offset = 90;
  right_fore = 95 + offset;
  right_hind = 95 - offset;
  convert();
}

void leanBackward() {
  int offset = 90;
  left_fore  = 90 - offset;
  left_hind  = 90 + offset;
  convert();
}

void leanRock() {
  const int slowDelay = 800;  // milliseconds – adjust to your liking
  for (int i = 0; i < 3; i++) {
    // Lean forward (front legs extend, hind legs contract)
    right_fore = 95 - 45; right_hind = 95 - 45;
    left_fore  = 90 + 45; left_hind  = 90 + 45;
    convert();
    delay(slowDelay);
    
    // Lean backward (opposite)
    right_fore = 95 + 45; right_hind = 95 + 45;
    left_fore  = 90 - 45; left_hind  = 90 - 45;
    convert();
    delay(slowDelay);
  }
  home_position();
  convert_once();
  mode = 5;
}

void happyWiggle() {
  for (int i = 0; i < 4; i++) {
    right_fore = 120; left_fore = 120; convert(); delay(80);
    right_fore = 70;  left_fore = 70; convert(); delay(80);
  }
  home_position(); convert_once(); mode = 5;
}

// ========== NEW FUNCTIONS ==========
void bow() {
  // Bow: front legs go forward/down, hind legs go up slightly
  // Adjust these angles to match your robot's mechanics
  int target_rf = 90;   // right fore forward
  int target_rh = 0;    // right hind up (smaller angle = up? depends on mounting)
  int target_lf = 90;    // left fore forward (asymmetric? adjust)
  int target_lh = 180;   // left hind up
  
  // Move to bow pose
  right_fore = target_rf; right_hind = target_rh;
  left_fore = target_lf;  left_hind = target_lh;
  convert();
  delay(500);  // hold bow for 0.5 sec
  
  // Return to home
  home_position();
  convert_once();
  mode = 5;
}

void shakeHand() {
  // Shake right front leg (right_fore) back and forth 3 times
  int original = right_fore;
  left_hind=135;
  right_hind=105;
  left_fore=80;
  convert();
  delay(15);
  for (int i = 0; i < 3; i++) {
    left_hind = original + 90;  // forward/up
    convert();
    delay(100);
    left_hind = original + 80;  // back/down
    convert();
    delay(100);
  }
  // Return to original position
  right_fore = original;
  convert();
  delay(50);
  mode = 5;
}

// ========== Helpers ==========
void home_position() {
  right_fore = 90;
  right_hind = 90;
  left_fore  = 90;
  left_hind  = 90;
}

void convert() {
  right_fore_servo.write(right_fore);
  right_hind_servo.write(right_hind);
  left_fore_servo.write(left_fore);
  left_hind_servo.write(left_hind);
  delay(15);
  server.handleClient();
}

void convert_once() {
  right_fore_servo.write(right_fore);
  right_hind_servo.write(right_hind);
  left_fore_servo.write(left_fore);
  left_hind_servo.write(left_hind);
  delay(15);
}