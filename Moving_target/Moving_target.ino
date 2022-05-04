int stepMotor[] = {8, 9, 10, 11};
int steps[4][8] =
  {
    {1, 1, 0, 0, 0, 0, 0, 1},
    {0, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 1}
  };

int rychlost = 1;
int uhel = 360;

void setup() {
  for (int i = 0; i <= 4; i++) {
    pinMode(stepMotor[i], OUTPUT);
  }
}

void loop() {
  for (int i = 7; i > 0; i--) {
    digitalWrite(stepMotor[0], steps[0][i]);
    digitalWrite(stepMotor[1], steps[1][i]);
    digitalWrite(stepMotor[2], steps[2][i]);
    digitalWrite(stepMotor[3], steps[3][i]);
    delay(rychlost);
  }
}
