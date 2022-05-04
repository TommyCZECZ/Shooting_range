#include <Servo.h>
#include <LiquidCrystal_I2C.h>

int servosPins[] = {2, 3, 24, 25, 26};
int laserSensors[] = {27, 28, 29, 30, 31};

/* Leds */
int dataPin = 2; // Pin 14
int latchPin = 3; // Pin 12
int clockPin = 4; // Pin 11
int LED = 5;

bool targetHit = false;

byte data[] =
  {
    B10000000,
    B01000000,
    B00100000,
    B00010000,
    B00000000,
    B00001000,
    B00000100,
    B00000010,
    B00000001,
    B00000010,
    B00000100,
    B00001000,
    B00000000,
    B00010000,
    B00100000,
    B01000000,
  };

Servo servos[5];
LiquidCrystal_I2C LCD(0x27, 20, 4); // SDA, SCL

void setup() {
  for (int i = 0; i < sizeof(servos); i++) {
    servos[i].attach(servosPins[i]);
    pinMode(laserSensors[i], INPUT);
  }

  /* Displej */
  LCD.begin();
  LCD.backlight();
}

void loop() {
  LEDs();
}

void LEDs() {
  currentMillis = millis(); 
  if (currentMillis - lastMillis > LEDsSpeed) {
    targetHit = false;
    digitalWrite(LED, LOW);
    if (i == 15) {
      i = 0;
    } else if (i == 3 || i == 11) {
      digitalWrite(LED, HIGH);
      i++;
    } else {
      i++;
    }
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, data[i]);
    digitalWrite(latchPin, HIGH);
    lastMillis = millis();
  }

  if (target == sizeof(servos)) {
    Servos(servoUp);
  }

  if ((i == 4 || i == 12) && !targetHit) {
    if (target == sizeof(servos)) {
      target = 0;
    }
    if (digitalRead(lasersSensors[target]) {
      servos[target].write(servoDown);
      target++;
      hit++;
      targetHit = true;
    }
  }
}

void Servos(int targetPosition) {
  for (int i = 0; i < sizeof(servos); i++) {
    servos[i].write(targetPosition);
  }
}
