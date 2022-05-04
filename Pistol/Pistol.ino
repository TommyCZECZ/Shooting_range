int laser = 6;
int btn = A0;
int btnState = 0;
int lastBtnState = 0;
int shots = 0;

void setup() {
  pinMode(laser, OUTPUT);
  pinMode(btn, INPUT);
}

void loop() {
  btnState = digitalRead(btn);
  if (btnState != lastBtnState && btnState) {
    digitalWrite(laser, HIGH);
    shots++;
    delay(50);
    digitalWrite(laser, LOW);
  }
  lastBtnState = btnState;
}
