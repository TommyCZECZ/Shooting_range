#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

int btnUp = 2;
int btnDown = 3;
int btnSubmit = 5;
int btnBack = 4;

int btnUpState, lastBtnUpState, btnDownState, lastBtnDownState, btnSubmitState, lastBtnSubmitState, btnBackState, lastBtnBackState, mode;
int cursorPosition = 1;
int difficulty = 1;
bool labelWrited, modesWrited, selected = false;

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

LiquidCrystal_I2C lcd(0x27,20,4);

void setup() {
  Serial.begin(9600);
  
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

  menu();
}

void loop() {
  
}

void menu() {
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
        lcd.setCursor(0, cursorPosition);
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
      }
      lastBtnSubmitState = btnSubmitState;
      btnBackState = digitalRead(btnBack);
      if (btnBackState && btnBackState != lastBtnBackState) {
        mode = 0;
        labelWrited = false;
      }
    }
  }
}
