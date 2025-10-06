#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int redPin = 13;
const int yellowPin = 12;
const int greenPin = 11;
const int interruptPin = 2;

volatile bool pressed = false;
volatile bool tickFlag = false;
volatile int countdownSeconds = 0;

volatile bool buttonPressed = false;

int totalTime = 10;
int walkTime = totalTime / 2;

int VERSION = 1;

int pressCounter = 0;

void setup() {
  if (EEPROM.read(0) == VERSION) {
    totalTime = EEPROM.read(1);
    pressCounter = EEPROM.read(2);
  } else {
    EEPROM.update(0, VERSION);
    EEPROM.update(1, totalTime);
    EEPROM.update(2, pressCounter);
  }

  walkTime = totalTime / 2;

  lcd.init();
  lcd.backlight();

  pinMode(redPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(greenPin, OUTPUT);

  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), toggleLights, FALLING);

  cli();//stop interrupts

  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei();//allow interrupts
}

void loop() {
  if (buttonPressed) {
    buttonPressed = false;

    if (!pressed && millis() > 2000) {
      pressed = true;
      countdownSeconds = totalTime;

      pressCounter++;
      EEPROM.update(2, pressCounter);
    }
  }

  if (!pressed) {
    digitalWrite(redPin, HIGH);
    digitalWrite(yellowPin, LOW);
    digitalWrite(greenPin, LOW);

    lcd.setCursor(0, 0);
    lcd.print("Wait");

    lcd.setCursor(7, 0);
    lcd.print(pressCounter);
  } else {
    if (tickFlag) {
      tickFlag = false;

      if (countdownSeconds > walkTime) {
        digitalWrite(redPin, HIGH);
        if (countdownSeconds <= totalTime - walkTime / 2) {
          digitalWrite(yellowPin, HIGH);
        }
        digitalWrite(greenPin, LOW);

        lcd.setCursor(0, 0);
        lcd.print("Wait");

        lcd.setCursor(7, 0);
        lcd.print(pressCounter);

        lcd.setCursor(0, 1);
        lcd.print(countdownSeconds - walkTime);
        lcd.print("          ");
      }
      else if (countdownSeconds > 0) {
        digitalWrite(redPin, LOW);
        digitalWrite(yellowPin, LOW);
        digitalWrite(greenPin, HIGH);

        lcd.setCursor(0, 0);
        lcd.print("Walk");
        lcd.setCursor(0, 1);
        lcd.print(countdownSeconds);
        lcd.print("          ");
      }
      else {
        pressed = false;
        lcd.clear();
      }
    }
  }
}

void toggleLights() {
  buttonPressed = true;
}

ISR(TIMER1_COMPA_vect) {
  if (pressed && countdownSeconds > 0) {
    countdownSeconds--;
    tickFlag = true;
  }
}
