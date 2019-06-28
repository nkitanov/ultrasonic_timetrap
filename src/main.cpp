#include <Arduino.h>
#include <HID-Project.h>
#include <EasyButton.h>
#include <EEPROM.h>
#define BUTTON_PIN 9
EasyButton button(BUTTON_PIN);

int RXLED = 17;
int sensityPin = A0;
int LED_RED = 6;
int LED_GREEN = 7;
int sensity_t;
int OFF_INTERVAL = 1000;
int ON_INTERVAL = 75;
int sens5 = 245; // EEPROM value for sensitivity 5m, etc.
int sens4 = 220; // 4.5m
int sens3 = 196; // 4m
int sens2 = 147; // 3m
int sens1 = 98;  // 2m
int sens0 = 49;  // 1m
unsigned long timeMeasure = 0;
unsigned long currentTime;
unsigned long nextTime;
bool block;
bool pressed;
bool set;
int blinks;
int blink_sens_level;
byte sens_eeprom;
int sensitivity;

void two_click() {
  if (block) {
    block = false;
  }
  else {
    block = true;
  }
}

 // Blink Green LED
void blink_green_led() {
 if (millis() > 7000) {
 unsigned long currentTime = millis();
 if (currentTime > nextTime) {
   if (digitalRead(LED_GREEN)) {
     digitalWrite(LED_GREEN, LOW);
     nextTime = currentTime + OFF_INTERVAL;
   } else {
     digitalWrite(LED_GREEN, HIGH);
     nextTime = currentTime + ON_INTERVAL;
   }
  }
 }
}

void green_led_block_mode() {   //Redefine green blink during blockage of sensor
  if(pressed || block) {
    OFF_INTERVAL = 200;
    ON_INTERVAL = 50;
  }
  else {
    OFF_INTERVAL = 1000;
    ON_INTERVAL = 75;
  }
}

void blink_red_led_startup() {
  // Blink LED fast after power on for 7s
  if (millis() < 7000) {
   unsigned long currentTime = millis();
  if (currentTime > nextTime) {
   if (digitalRead(LED_RED)) {
     digitalWrite(LED_RED, LOW);
     nextTime = currentTime + 150;
   } else {
     digitalWrite(LED_RED, HIGH);
     nextTime = currentTime + 50;
   }
  }
 }
}

void display_sens_level() {
  if (millis() > 7000) {
  
    set = true;

    if (sens_eeprom == sens5) {
      blink_sens_level = 6;
    }
    else if (sens_eeprom == sens4) {
      blink_sens_level = 5;
    }
    else if (sens_eeprom == sens3) {
      blink_sens_level = 4;
    }
    else if (sens_eeprom == sens2) {
      blink_sens_level = 3;
    }
    else if (sens_eeprom == sens1) {
      blink_sens_level = 2;
    }
    else if (sens_eeprom == sens1) {
      blink_sens_level = 1;
    }
    else {
      blink_sens_level = 0;
    }

  while (blinks < blink_sens_level) {
    digitalWrite(LED_GREEN, HIGH); delay (500);
    digitalWrite(LED_GREEN, LOW); delay (1000);
    blinks++;
   } 
  }
set = false;
}

void setup_mode() {
  if (millis() < 7000) {
    set = true;
    digitalWrite(LED_RED, HIGH); delay(3000);
    digitalWrite(LED_RED, LOW);
  }
  
}

void check_sensor() {
if (millis() > 7000) {
  // Check sensor for object in sight
 if (!pressed && !block) {
  if (millis() - timeMeasure > 1000) { // Block measurement for 1sec after previous
    sensity_t = analogRead(sensityPin);
    if (sensity_t < sensitivity && sensity_t > 40) {
      Consumer.write(MEDIA_VOLUME_UP); // Command volume up to pin the time
      timeMeasure = millis();
      digitalWrite(LED_RED, HIGH);
   }
  }
 }
     //  // Turn off RED led after 200ms of measurement
    if (millis() - timeMeasure > 200) {
    digitalWrite(LED_RED, LOW);
   }
 }
}

void setup() {
  Consumer.begin();
  button.begin();
  pinMode(RXLED, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);  
  button.onSequence(2, 2000, two_click);
  button.onPressedFor(3000, setup_mode);
    sens_eeprom = EEPROM.read(0); // Read sensitivity from EEPROM
  sensitivity = sens_eeprom * 4;  // Convert sensitivity for ADC resolution
}

void loop() {
  digitalWrite(RXLED, HIGH); // Turn off RX led on MCU board
  button.read(); // Continuously read the status of the button. 
  pressed = button.isPressed();
  blink_red_led_startup();
  display_sens_level();
  
  if(!set) {
    green_led_block_mode();
    blink_green_led();
    check_sensor();
  }
}

