#include <LedControl.h>
#include <arduino-timer.h>
#include <EEPROM.h>
#include "pitches.h"

const char DIN = 12;
const char CS =  11;
const char CLK = 10;

const char LButton = 51;
char LButtonState = 0;
const char MButton = 52;
char MButtonState = 0;
const char RButton = 53;
char RButtonState = 0;

const char buzzer = 50;

Timer<1> timer;
char statsUpdateTime = 15;

LedControl lc = LedControl(DIN, CLK, CS, 0);

char menuPos;
byte* face;
char lifeLevel;
char hungerLevel;
char happinessLevel;

byte faceType[][8] = {
  {0x42, 0x24, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00}, //dead
  {0x00, 0x24, 0x66, 0x81, 0x18, 0xA5, 0x00, 0x00}, //cry
  {0x24, 0x66, 0x66, 0x00, 0x18, 0x24, 0x00, 0x00}, //sad
  {0x24, 0x66, 0x66, 0x00, 0x18, 0x00, 0x00, 0x00}, //unhappy
  {0x66, 0x66, 0x66, 0x00, 0x18, 0x00, 0x00, 0x00}, //neutral
  {0x66, 0x66, 0x66, 0x00, 0x24, 0x18, 0x00, 0x00}  //happy
};

byte* faceStatus(char lifeLevel, char hungerLevel, char happinessLevel) {

  if (lifeLevel == 0 || hungerLevel == 0) {
    return faceType[0];
  } else {
    return faceType[(lifeLevel + hungerLevel + happinessLevel) / 3];
  }
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

  if (pos != 0) {
    return (byte)(3 << (8 - pos * 2));
  } else {
    return 0x00;
  }

}

void displayStatus() {

  lifeStatus(lifeLevel);
  hungerStatus(hungerLevel);
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

  if (lifeLevel < 5) {
    lifeLevel++;
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

  if (hungerLevel < 5) {
    hungerLevel++;
  }

}

void sing() {
  byte options[][8] = {
    {0x00, 0x46, 0xC5, 0x45, 0x4C, 0xEC, 0x00, 0x00},
    {0x00, 0xC6, 0x25, 0x45, 0x8C, 0xEC, 0x00, 0x00},
    {0x00, 0xE6, 0x25, 0x45, 0x2C, 0xEC, 0x00, 0x00},
    {0x00, 0xA6, 0xA5, 0xE5, 0x2C, 0x2C, 0x00, 0x00},
    {0x00, 0xE6, 0x85, 0xC5, 0x2C, 0xCC, 0x00, 0x00},
    {0x00, 0xE6, 0x85, 0xE5, 0xAC, 0xEC, 0x00, 0x00}
  };

  int song1[] = {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, REST, NOTE_B3, NOTE_C4};
  int song2[] = {NOTE_FS5, NOTE_FS5, NOTE_D5, NOTE_B4, NOTE_B4, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_GS5, NOTE_GS5, NOTE_A5, NOTE_B5, NOTE_A5, NOTE_A5, NOTE_A5, NOTE_E5, NOTE_D5, NOTE_FS5, NOTE_FS5, NOTE_FS5, NOTE_E5, NOTE_E5, NOTE_FS5, NOTE_E5};
  int song3[] = {NOTE_D4, NOTE_G4, NOTE_AS4, NOTE_A4, NOTE_G4, NOTE_D5, NOTE_C5, NOTE_A4, NOTE_G4, NOTE_AS4, NOTE_A4, NOTE_F4, NOTE_GS4, NOTE_D4, NOTE_D4};
  int song4[] = {NOTE_E5, NOTE_D5, NOTE_FS4, NOTE_GS4, NOTE_CS5, NOTE_B4, NOTE_D4, NOTE_E4, NOTE_B4, NOTE_A4, NOTE_CS4, NOTE_E4, NOTE_A4};
  int song5[] = {NOTE_DS4, NOTE_E4, REST, NOTE_FS4, NOTE_G4, REST, NOTE_DS4, NOTE_E4, NOTE_FS4, NOTE_G4, NOTE_C5, NOTE_B4, NOTE_E4, NOTE_G4, NOTE_B4, NOTE_AS4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_D4, NOTE_E4};
  int song6[] = {NOTE_C5, NOTE_F5, NOTE_F5, NOTE_G5, NOTE_F5, NOTE_E5, NOTE_D5, NOTE_D5, NOTE_D5, NOTE_G5, NOTE_G5, NOTE_A5, NOTE_G5, NOTE_F5, NOTE_E5, NOTE_C5, NOTE_C5, NOTE_A5, NOTE_A5, NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F5, NOTE_D5, NOTE_C5, NOTE_C5, NOTE_D5, NOTE_G5, NOTE_E5, NOTE_F5};
  int* melodies[] = {song1, song2, song3, song4, song5, song6};

  // note durations: 4 = quarter note, 8 = eighth note, etc.:
  char duration1[] = {4, 8, 8, 4, 4, 4, 4, 4};
  char duration2[] = {8, 8, 8, 4, 4, 4, 4, 5, 8, 8, 8, 8, 8, 8, 8, 4, 4, 4, 4, 5, 8, 8, 8, 8};
  char duration3[] = {4, 4, 8, 4, 2, 4, 2, 2, 4, 8, 4, 2, 4, 1, 4};
  char duration4[] = {8, 8, 4, 4, 8, 8, 4, 4, 8, 8, 4, 4, 2};
  char duration5[] = {8, 4, 8, 8, 4, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 2, 16, 16, 16, 16, 2};
  char duration6[] = {4, 4, 8, 8, 8, 8, 4, 4, 4, 4, 8, 8, 8, 8, 4, 4, 4, 4, 8, 8, 8, 8, 4, 4, 8, 8, 4, 4, 4, 2};
  char* durations[] = {duration1, duration2, duration3, duration4, duration5, duration6};

  int lengths[] = {8, 24, 15, 13, 21, 30};

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

  int* melody = melodies[opt];
  char* noteDurations = durations[opt];
  int songLength = lengths[opt];

  for (int thisNote = 0; thisNote < songLength; thisNote++) {

    int noteDuration = 1000 / noteDurations[thisNote];
    tone(buzzer, melody[thisNote], noteDuration);

    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);

    noTone(buzzer);
  }

  if (happinessLevel < 5) {
    happinessLevel++;
  }

}

/// PLAY
char ballDir;
char xBall, yBall, xBallPrev, yBallPrev;
char cpu;
char player;

void moveBall() {
  xBallPrev = xBall;
  yBallPrev = yBall;

  bounceBall();

  if (ballDir == 1) {
    xBall--;
    yBall++;
  }
  if (ballDir == 2) {
    xBall++;
    yBall++;
  }
  if (ballDir == 3) {
    xBall++;
    yBall--;
  }
  if (ballDir == 4) {
    xBall--;
    yBall--;
  }
}

void bounceBall() {

  if ((xBall == 0 || xBall == 7) && yBall == 6 && player <= xBall && player + 2 >= xBall) {
    tone(buzzer, NOTE_F5, 100);
    ballDir += 2;
  }
  else if ((xBall == 0 || xBall == 7) && yBall == 1 && cpu <= xBall && cpu + 2 >= xBall) {
    tone(buzzer, NOTE_G5, 100);
    ballDir -= 2;
  } else {
    switch (ballDir) {
      case 1:
        if (xBall == 0)
          ballDir++;
        if (yBall == 6 && (player <= xBall && player + 2 >= xBall)) {
          tone(buzzer, NOTE_F5, 100);
          ballDir = 4;
        }
        break;
      case 2:
        if (xBall == 7)
          ballDir--;
        if (yBall == 6 && (player <= xBall && player + 2 >= xBall)) {
          tone(buzzer, NOTE_F5, 100);
          ballDir++;
        }
        break;
      case 3:
        if (xBall == 7)
          ballDir++;
        if (yBall == 1 && (cpu <= xBall && cpu + 2 >= xBall)) {
          tone(buzzer, NOTE_G5, 100);
          ballDir--;
        }
        break;
      case 4:
        if (xBall == 0)
          ballDir--;
        if (yBall == 1 && (cpu <= xBall && cpu + 2 >= xBall)) {
          tone(buzzer, NOTE_G5, 100);
          ballDir = 1;
        }
        break;
      default:
        break;
    }
  }
}

void play() {
  cpu = 3;
  player = 2;
  ballDir = random(1, 2);
  xBall = 4;
  yBall = 1;
  xBallPrev = 4;
  yBallPrev = 2;

  byte displ[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  displ[0] = 7 << cpu;
  displ[7] = 7 << player;

  bool finishGame = false;
  bool cpuRight = true;

  char backState = 0, enterState = 0, forwardState = 0;

  while (!finishGame) {
    displayLED(displ);
    backState = digitalRead(LButton);
    enterState = digitalRead(MButton);
    forwardState = digitalRead(RButton);

    if (backState && player < 5) {
      player++;
    }
    if (forwardState && player > 0) {
      player--;
    }
    displ[7] = 7 << player;

    int ezMode = random(1, 5);

    if (cpuRight && cpu > 0) {
      cpu--;
    }
    if (!cpuRight && cpu < 5) {
      cpu++;
    }
    if (cpu == 0 || (ballDir == 3 && ezMode == 2)) {
      cpuRight = false;
    }
    if (cpu == 5 || (ballDir == 4 && ezMode == 4)) {
      cpuRight = true;
    }
    displ[0] = 7 << cpu;

    moveBall();
    displ[yBall] += 1 << xBall;
    displ[yBallPrev] = 0x00;

    if (yBall < 0) {
      finishGame = true;
      tone(buzzer, NOTE_F5, 500);
    }
    if (yBall > 7) {
      finishGame = true;
      tone(buzzer, NOTE_G5, 500);
    }
  }

  if (happinessLevel < 5) {
    happinessLevel++;
  }
}

bool updateStats() {
  
  char rnd = random(1, 5);
  if (rnd > 3) {
    rnd = random(1, 5);
  }
  switch (random(1, 5)) {
    case 1:
      if (lifeLevel > 1)
        lifeLevel--;
      break;
    case 2:
      if (hungerLevel > 0)
        hungerLevel--;
      break;
    case 3:
      if (happinessLevel > 0)
        happinessLevel--;
      break;
    case 4:
      if (lifeLevel > 0 && hungerLevel > 0) {
        lifeLevel--;
        hungerLevel--;
      }
      break;
    case 5:
      if (happinessLevel > 0 && hungerLevel > 0) {
        hungerLevel--;
        happinessLevel--;
      }
      break;
    default:
      break;
  }

  EEPROM.write(0, lifeLevel);
  EEPROM.write(1, hungerLevel);
  EEPROM.write(2, happinessLevel);
  return false;
}

void reset(){

  LButtonState = digitalRead(LButton);
  MButtonState = digitalRead(MButton);
  RButtonState = digitalRead(RButton);

  if (MButtonState) {
    lifeLevel = 5;
    hungerLevel = 5;
    happinessLevel = 5;
    EEPROM.write(0, lifeLevel);
    EEPROM.write(1, hungerLevel);
    EEPROM.write(2, happinessLevel);
  }

}

void displayLED(byte displ [])
{
  for (char i = 0; i < 8; i++)
  {
    lc.setColumn(0, i, displ[7 - i]);
  }
}

void setup() {
  Serial.begin(9600);
  lc.shutdown(0, false);      // The MAX72XX is in power-saving mode on startup
  lc.setIntensity(0, 3);      // Set the brightness to maximum value
  lc.clearDisplay(0);         // and clear the display

  menuPos = 0;
  lifeLevel = EEPROM.read(0);
  hungerLevel = EEPROM.read(1);
  happinessLevel = EEPROM.read(2);
}

void loop() {

  face = faceStatus(lifeLevel, hungerLevel, happinessLevel);

  timer.tick();

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
      case 0:
        displayStatus();
        break;
      case 1:
        feed();
        break;
      case 2:
        heal();
        break;
      case 3:
        play();
        break;
      case 4:
        sing();
        break;
      default:
        menuPos = 0;
        break;
    }
  }
  if (menuPos > 4)
    menuPos = 1;
  if (menuPos < 0)
    menuPos = 4;

  face[7] = getMenu(menuPos);
  displayLED(face);

  if(lifeLevel == 0){
    reset();
  }

  timer.every(statsUpdateTime / 2.0 * 60000, [](void*) -> bool { return updateStats(); });
}
