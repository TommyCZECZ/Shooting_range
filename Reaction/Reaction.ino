#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include "WiFiEsp.h"
#include "ThingSpeak.h"

#ifndef HAVE_HWSERIAL3
#define ESP_BAUDRATE  19200
#else
#define ESP_BAUDRATE  115200
#endif

char ssid[] = "KTJ CR s.r.o. 2.4G";
char pass[] = "8211182309";
int keyIndex = 0;

unsigned long myChannelNumber = 1584483;
const char * myWriteAPIKey = "CE8YP6BADOJDZD3W";

int servosPins[] = {40, 44, 48, 52};
int laserSensors[] = {38, 42, 46, 50};

int levelSpeed = 1;
int hit, lastMillis = 0;
int currentMillis = 0;
bool targetHit = false;
int level = 1;
int servoUp = 125;
int servoDown = 40;

int btnUp = 2;
int btnDown = 3;
int btnSubmit = 5;
int btnBack = 4;

int btnUpState, lastBtnUpState, btnDownState, lastBtnDownState, btnSubmitState, lastBtnSubmitState, btnBackState, lastBtnBackState, mode, lastMode;
int shots = 0;
int cursorPosition = 1;
int difficulty = 1;
int raisedTargets = 0;
bool labelWrited, modesWrited, paused, selected = false;

byte arrow[] = {
  B00000,
  B00100,
  B00010,
  B11111,
  B00010,
  B00100,
  B00000,
  B00000,
};

String modes[] = {"", "Postreh", "Pohyblivy terc", "Rychlost strelby"};
String difficulties[] = {"", "Nizka  ", "Stredni", "Vysoka "};

Servo servos[4];
LiquidCrystal_I2C lcd(0x27, 20, 4); // SDA, SCL
WiFiEspClient  client;

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < sizeof(servos) / sizeof(Servo); i++) {
    servos[i].attach(servosPins[i]);
    pinMode(laserSensors[i], INPUT);
    servos[i].write(servoDown);
  }

  /* Displej */
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, arrow);
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Vyukovy simulator");
  lcd.setCursor(2, 2);
  lcd.print("zavodni strelby");

  pinMode(btnUp, INPUT);
  pinMode(btnDown, INPUT);
  pinMode(btnSubmit, INPUT);
  pinMode(btnBack, INPUT);

  setEspBaudRate(ESP_BAUDRATE);
  WiFi.init(&Serial3);
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }
  Serial.println("found it!");
    
  ThingSpeak.begin(client);
  
  menu();
}

void loop() {
  Serial.println("Mode: " + String(mode));
  switch (mode) {
    case 1:
      reaction();
      break;
  }
  scoreStatement();
  btnBackState = digitalRead(btnBack);
  if (btnBackState) {
    lastBtnBackState = btnBackState;
    paused = true;
    labelWrited = false;
    while (paused) {
      if (!labelWrited) {
        lcd.clear();
        lcd.print("- - - Pauza - - - -");
        lcd.setCursor(2, 1);
        lcd.print("Pokracovat");
        lcd.setCursor(2, 2);
        lcd.print("Ukoncit");
        lcd.setCursor(2, 3);
        lcd.print("Odeslat skore");
        cursorPosition = 1;
        lcd.setCursor(0, 1);
        lcd.write(byte(0));
        labelWrited = true;
      }
      btnUpState = digitalRead(btnUp);
      if (btnUpState && btnUpState != lastBtnUpState && cursorPosition > 1) {
        lcd.setCursor(0, cursorPosition);
        lcd.print(" ");
        cursorPosition--;
        lcd.setCursor(0, cursorPosition);
        lcd.write(byte(0));
      }
      lastBtnUpState = btnUpState;
      btnDownState = digitalRead(btnDown);
      if (btnDownState && btnDownState != lastBtnDownState && cursorPosition < 3) {
        lcd.setCursor(0, cursorPosition);
        lcd.print(" ");
        cursorPosition++;
        lcd.setCursor(0, cursorPosition);
        lcd.write(byte(0));
      }
      lastBtnDownState = btnDownState;
      btnSubmitState = digitalRead(btnSubmit);
      if (btnSubmitState && btnSubmitState != lastBtnSubmitState) {
        paused = false;
        switch (cursorPosition) {
          case 1:
            lastMode = 0;
            break;
          case 3:
            sendToDatabase();
          case 2:
            lastBtnSubmitState = btnSubmitState;
            menu();
            break; 
        }
      }
      lastBtnSubmitState = btnSubmitState;
      btnBackState = digitalRead(btnBack);
      if (btnBackState && btnBackState != lastBtnBackState) {
        paused = false;
        lastMode = 0;
      }
      lastBtnBackState = btnBackState;
    }
  }
}
  
void reaction() {
  int Random;
  Serial.println("Difficulty: " + String(difficulty));
  switch(difficulty) {
    case 1:
      difficulty = 3000;
      break;
    case 2:
      difficulty = 2000;
      break;
    case 3:
      difficulty = 500;
      break;
  }
  
  currentMillis = millis();
  if((currentMillis - lastMillis) > difficulty || targetHit) {
    targetHit = false;
    Servos(40);
    delay(250);
    Random = random(0, 4);
    Serial.println(Random);
    servos[Random].write(servoUp);
    raisedTargets++;
    lastMillis = millis();
  }
  
  if (digitalRead(laserSensors[Random]) == HIGH && !targetHit) {
    Serial.println("Hit");
    hit++;
    targetHit = true;
    Servos(40);
  }
}

void scoreStatement() {
  if (mode != lastMode) {
    lcd.clear();
    lcd.print(modes[mode]);
    lcd.setCursor(0, 1);
    lcd.print("Zvednute terce:");
    lcd.setCursor(0, 2);
    lcd.print("Zasahy:");
  }
  lastMode = mode;
  lcd.setCursor(16, 1);
  lcd.print(raisedTargets);
  lcd.setCursor(8, 2);
  lcd.print(String(hit));
}

void Servos(int targetPosition) {
  for (int i = 0; i < sizeof(servos); i++) {
    servos[i].write(targetPosition);
  }
}

void menu() {
  labelWrited = false;
  modesWrited = false;
  selected = false;
  difficulty = 1;
  mode = 0;
  while (!selected) {
    if (!labelWrited) {
      lcd.clear();
      lcd.print("- - - - Menu - - - -");
      labelWrited = true;
    }
    if (!mode) {
      if (!modesWrited) {
        for (int i = 1; i < sizeof(modes) / sizeof(String); i++) {
          lcd.setCursor(2, i);
          lcd.print(modes[i]);
        }
        cursorPosition = 1;
        lcd.setCursor(0, 1);
        lcd.write(byte(0));
        modesWrited = true;
      }
      
      btnUpState = digitalRead(btnUp);
      if (btnUpState && btnUpState != lastBtnUpState && cursorPosition > 1) {
        lcd.setCursor(0, cursorPosition);
        lcd.print(" ");
        cursorPosition--;
        lcd.setCursor(0, cursorPosition);
        lcd.write(byte(0));
      }
      lastBtnUpState = btnUpState;
      btnDownState = digitalRead(btnDown);
      if (btnDownState && btnDownState != lastBtnDownState && cursorPosition < 3) {
        lcd.setCursor(0, cursorPosition);
        lcd.print(" ");
        cursorPosition++;
        lcd.setCursor(0, cursorPosition);
        lcd.write(byte(0));
      }
      lastBtnDownState = btnDownState;
      btnSubmitState = digitalRead(btnSubmit);
      if (btnSubmitState && btnSubmitState != lastBtnSubmitState) {
        mode = cursorPosition;
        labelWrited = false;
        modesWrited = false;
      }
      lastBtnSubmitState = btnSubmitState;
    } else {
      lcd.setCursor(0, 1);
      lcd.print(modes[mode]);
      lcd.setCursor(0, 3);
      lcd.print("Obtiznost:");
      lcd.setCursor(11, 3);
      lcd.print(difficulties[difficulty]);
  
      btnUpState = digitalRead(btnUp);
      if (btnUpState && btnUpState != lastBtnUpState && difficulty < 3) {
        difficulty++;
      }
      lastBtnUpState = btnUpState;
      btnDownState = digitalRead(btnDown);
      if (btnDownState && btnDownState != lastBtnDownState && difficulty > 1) {
        difficulty--;
      }
      lastBtnDownState = btnDownState;
      btnSubmitState = digitalRead(btnSubmit);
      if (btnSubmitState && btnSubmitState != lastBtnSubmitState) {
        selected = true;
        labelWrited = false;
      }
      lastBtnSubmitState = btnSubmitState;
      btnBackState = digitalRead(btnBack);
      if (btnBackState && btnBackState != lastBtnBackState) {
        mode = 0;
        labelWrited = false;
      }
      lastBtnBackState = btnBackState;
    }
  }
}

void sendToDatabase() {
  lcd.clear();
  lcd.print("Pripojovani");
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }
  lcd.clear();
  lcd.print("Odesilani: 1");
  int x = ThingSpeak.writeField(myChannelNumber, 1, raisedTargets, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
    lcd.setCursor(0, 1);
    lcd.print("1 - Odeslano");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
    lcd.setCursor(0, 1);
    lcd.print("1 - Neodeslano");
  }
  lcd.setCursor(0, 0);
  lcd.print("Odesilani: 2");
  delay(15000);
  x = ThingSpeak.writeField(myChannelNumber, 2, hit, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
    lcd.setCursor(0, 2);
    lcd.print("2 - Odeslano");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
    lcd.setCursor(0, 2);
    lcd.print("2 - Neodeslano");
  }
  delay(1000);
}

void setEspBaudRate(unsigned long baudrate){
  long rates[6] = {115200,74880,57600,38400,19200,9600};

  Serial.print("Setting ESP8266 baudrate to ");
  Serial.print(baudrate);
  Serial.println("...");

  for(int i = 0; i < 6; i++){
    Serial3.begin(rates[i]);
    delay(100);
    Serial3.print("AT+UART_DEF=");
    Serial3.print(baudrate);
    Serial3.print(",8,1,0,0\r\n");
    delay(100);  
  }
    
  Serial3.begin(baudrate);
}
