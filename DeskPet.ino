#include <LedControl.h>

const char DIN = 12;
const char CS =  11;
const char CLK = 10;

const char LButton = 51;
char LButtonState = 0;
const char MButton = 52;
char MButtonState = 0;
const char RButton = 53;
char RButtonState = 0;

LedControl lc = LedControl(DIN, CLK, CS, 0);

byte faceType[][8] = {
    {0x42, 0x24, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00}, //dead
    {0x00, 0x24, 0x66, 0x81, 0x18, 0xA5, 0x00, 0x00}, //cry
    {0x24, 0x66, 0x66, 0x00, 0x18, 0x24, 0x00, 0x00}, //sad
    {0x24, 0x66, 0x66, 0x00, 0x18, 0x00, 0x00, 0x00}, //unhappy
    {0x66, 0x66, 0x66, 0x00, 0x18, 0x00, 0x00, 0x00}, //neutral
    {0x66, 0x66, 0x66, 0x00, 0x24, 0x18, 0x00, 0x00}  //happy
  };

byte* faceStatus(char lifeLevel, char hungerLevel) {

  return faceType[(lifeLevel + hungerLevel)/2];
}

void lifeStatus(byte level) {
  byte lifeLevel[6][8] = {
    {0x00, 0x66, 0x99, 0x81, 0x81, 0x42, 0x24, 0x18}, //Empty
    {0x00, 0x66, 0x99, 0x81, 0x81, 0x42, 0x3C, 0x18},
    {0x00, 0x66, 0x99, 0x81, 0x81, 0x7E, 0x3C, 0x18},
    {0x00, 0x66, 0x99, 0x81, 0xFF, 0x7E, 0x3C, 0x18},
    {0x00, 0x66, 0x99, 0xFF, 0xFF, 0x7E, 0x3C, 0x18},
    {0x00, 0x66, 0xFF, 0xFF, 0xFF, 0x7E, 0x3C, 0x18}  //Full
  };

  displayLED(lifeLevel[level]);
  delay(700);
  lc.clearDisplay(0);
  delay(80);
  displayLED(lifeLevel[level]);
  delay(80);
  lc.clearDisplay(0);
  delay(80);
  displayLED(lifeLevel[level]);
  delay(80);
  lc.clearDisplay(0);
  delay(80);
  displayLED(lifeLevel[level]);
}

void hungerStatus(byte level) {
  byte hungerLevel[6][8] = {
    {0x0E, 0x1B, 0x24, 0x24, 0x42, 0x42, 0x42, 0x3C}, //Empty
    {0x0E, 0x1B, 0x24, 0x24, 0x42, 0x42, 0x7E, 0x3C},
    {0x0E, 0x1B, 0x24, 0x24, 0x42, 0x7E, 0x7E, 0x3C},
    {0x0E, 0x1B, 0x24, 0x24, 0x7E, 0x7E, 0x7E, 0x3C},
    {0x0E, 0x1B, 0x24, 0x3C, 0x7E, 0x7E, 0x7E, 0x3C},
    {0x0E, 0x1B, 0x3C, 0x3C, 0x7E, 0x7E, 0x7E, 0x3C}  //Full
  };

  displayLED(hungerLevel[level]);
  delay(700);
  lc.clearDisplay(0);
  delay(80);
  displayLED(hungerLevel[level]);
  delay(80);
  lc.clearDisplay(0);
  delay(80);
  displayLED(hungerLevel[level]);
  delay(80);
  lc.clearDisplay(0);
  delay(80);
  displayLED(hungerLevel[level]);

}

byte getMenu(char pos) {

  return (byte)(1 << (8 - pos));

}

void displayStatus() {

  lifeStatus(5);
  hungerStatus(5);
}

void heal() {

  byte options[][8] = {
    {0x48, 0x24, 0x00, 0xFF, 0xFF, 0xFF, 0x7E, 0x3C}, //Soup
    {0x00, 0x00, 0x7E, 0xF1, 0xF1, 0x7E, 0x00, 0x00}, //Medicine
    {0x3C, 0x24, 0xE7, 0x81, 0x81, 0xE7, 0x24, 0x3C}, //Visit the doctor
    {0x00, 0x00, 0x3D, 0xFF, 0x3D, 0x00, 0x00, 0x00}, //Vaccine
    {0x3C, 0x3C, 0xFF, 0xFF, 0xFF, 0xFF, 0x3C, 0x3C}, //Go to the hospital
  };

  char opt = 0;

  char backState = 0, enterState = 0, forwardState = 0;

  while (!enterState) {
    backState = digitalRead(LButton);
    forwardState = digitalRead(RButton);

    if (backState)
      opt--;
    if (forwardState)
      opt++;
    if (opt >= sizeof(options) / 8)
      opt = 0;
    if (opt < 0)
      opt = sizeof(options) / 8 - 1;

    displayLED(options[opt]);
    enterState = digitalRead(MButton);
  }
}

void feed() {

  byte options[][8] = {
    {0x08, 0x10, 0x6C, 0xFE, 0xFE, 0xFE, 0xFE, 0x7C}, //Apple
    {0x00, 0x00, 0x66, 0x99, 0x99, 0xA5, 0x7E, 0x00}, //Pretzel
    {0x18, 0x18, 0x34, 0x2C, 0x7E, 0x5A, 0xF7, 0xFF}, //Pizza
    {0x1C, 0x3E, 0x36, 0x36, 0x30, 0x30, 0x30, 0x30}, //Candy
    {0x00, 0x3C, 0x6E, 0x7A, 0x5E, 0x7E, 0x18, 0x18}, //Mushroom
    {0x00, 0x00, 0xFF, 0xFD, 0xFF, 0xFC, 0x78, 0x00}, //Coffe
    {0x00, 0xFC, 0xFF, 0xFD, 0xFD, 0xFE, 0xFC, 0xFC}, //Milk
  };

  char opt = 0;

  char backState = 0, enterState = 0, forwardState = 0;

  while (!enterState) {
    backState = digitalRead(LButton);
    forwardState = digitalRead(RButton);

    if (backState)
      opt--;
    if (forwardState)
      opt++;
    if (opt >= sizeof(options) / 8)
      opt = 0;
    if (opt < 0)
      opt = sizeof(options) / 8 - 1;

    displayLED(options[opt]);
    enterState = digitalRead(MButton);
  }

}

void displayLED(byte displ [])
{
  for (char i = 0; i < 8; i++)
  {
    lc.setColumn(0, i, displ[7 - i]);
  }
}

char menuPos;
byte* face;

void setup() {
  lc.shutdown(0, false);      //The MAX72XX is in power-saving mode on startup
  lc.setIntensity(0, 3);     // Set the brightness to maximum value
  lc.clearDisplay(0);         // and clear the display
  menuPos = 1;
}

void loop() {

  face = faceStatus(5,5);

  LButtonState = digitalRead(LButton);
  MButtonState = digitalRead(MButton);
  RButtonState = digitalRead(RButton);

  if (LButtonState) {
    menuPos--;
  }
  if (RButtonState) {
    menuPos++;
  }
  if (MButtonState) {
    switch (menuPos) {
      case 1:
        displayStatus();
        break;
      case 2:
        feed();
        break;
      case 3:
        heal();
        break;
      case 4:
        break;
      case 5:
        break;
      case 6:
        break;
      case 7:
        break;
      case 8:
        break;
      default:
        menuPos = 1;
        break;
    }
  }
  if (menuPos > 8)
    menuPos = 1;
  if (menuPos < 1)
    menuPos = 8;

  face[7] = getMenu(menuPos);
  displayLED(face);
}
