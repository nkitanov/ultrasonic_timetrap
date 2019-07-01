#include <Arduino.h>
#include <HID-Project.h>
#include <EasyButton.h>
#include <EEPROM.h>
#define BUTTON_PIN 9
EasyButton button(BUTTON_PIN);

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
int blinks;
int blink_sens_level;
int sensitivity;
int clicks;

unsigned long timeMeasure;
unsigned long currentTime;
unsigned long nextTime;
unsigned long exitSet;

bool block;
bool pressed;
bool set;
bool display_sens_run;
bool _setup;

byte sens_eeprom;


void two_click() {
  if (!set && !_setup) {  // Do not run during initial phase and setup
    if (block) {          // Toggle measure hold block with 2 click of the button
      block = false;
    } else {
      block = true;
    }
  }
}

 // Blink Green LED constanlty while working
void blink_green_led() {
  if (millis() > 5000) {
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

// Shorter green blink intervals during blockage of sensor
void green_led_block_mode() {
  if (pressed || block) {
    OFF_INTERVAL = 200;
    ON_INTERVAL = 50;
  } else {
    OFF_INTERVAL = 1000;
    ON_INTERVAL = 75;
  }
}

// Blink LED fast after power on for 5s
void blink_red_led_startup() {
  if (millis() < 5000) {
    currentTime = millis();
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

// Display sensitivity level
// 1 blink --> 1m
// 2 blinks --> 2m
// 3 blinks --> 3m
// 4 blinks --> 4m
// 5 blinks --> 4.5m
// 4 blinks --> 5m
// no blinks --> unset
void display_sens_level() {
  if (millis() > 5000 && !display_sens_run && !_setup) {
    set = true;
    display_sens_run = true;  //Run it only one time

    if (sens_eeprom == sens5) {
      blink_sens_level = 6;
    } else if (sens_eeprom == sens4) {
      blink_sens_level = 5;
    } else if (sens_eeprom == sens3) {
      blink_sens_level = 4;
    } else if (sens_eeprom == sens2) {
      blink_sens_level = 3;
    } else if (sens_eeprom == sens1) {
      blink_sens_level = 2;
    } else if (sens_eeprom == sens0) {
      blink_sens_level = 1;
    } else {
      blink_sens_level = 0;
    }

    // Indicate sensitivity level with number of blinks
    while (blinks < blink_sens_level) {
      digitalWrite(LED_GREEN, HIGH); delay (500);
      digitalWrite(LED_GREEN, LOW); delay (1000);
      blinks++;
    } 
    delay(1000);
  }
  set = false;
}

 // Count button clicks during sensitivity setup
 void onPressed() {
   if (_setup) {
    clicks++;
   }
 }

// Set sensitivity and store it in EEPROM pepmanently (reversed for failsafe)
// 0 clicks --> 5m
// 1 clicks --> 4.5m
// 2 clicks --> 4m
// 3 clicks --> 3m
// 4 clicks --> 2m
// 5 clicks --> 1m
// other    --> 5m
 void set_sensitivity() {
  if (_setup) {
    set = true;   // Keep in set mode to block run main until diplay of sense levels complete in main loop
    currentTime = millis();
    if (clicks == 5) {
      sens_eeprom = sens0;
    } else if (clicks == 4) {
      sens_eeprom = sens1;
    } else if (clicks == 3) {
      sens_eeprom = sens2;
    } else if (clicks == 2) {
      sens_eeprom = sens3;
    } else if (clicks == 1) {
      sens_eeprom = sens4;
    } else {
      sens_eeprom = sens5;
    }
    if (currentTime > exitSet) {    // Wait 10 seconds for input, set and exit
      sensitivity = sens_eeprom * 4;
      EEPROM.update(0, sens_eeprom);
      digitalWrite(LED_RED, HIGH); delay(3000);
      digitalWrite(LED_RED, LOW); delay(1000);
      _setup = false;
    }
  } 
}

void setup_mode() {
  if (millis() < 5000) {
    _setup = true;
    digitalWrite(LED_RED, HIGH); delay(3000);
    digitalWrite(LED_RED, LOW);
    exitSet = currentTime + 10000; 
  }
}

// Check sensor for object in front
void check_sensor() {
  if (millis() > 5000) {
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
    // Turn off RED led after 200ms of measurement
    if (millis() - timeMeasure > 200) {
      digitalWrite(LED_RED, LOW);
    }
  }
}

void setup() {
  Consumer.begin();
  button.begin();
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);  
  button.onSequence(2, 2000, two_click);
  button.onPressedFor(3000, setup_mode);
  button.onPressed(onPressed);
  sens_eeprom = EEPROM.read(0);   // Read sensitivity from EEPROM
  sensitivity = sens_eeprom * 4;  // Convert sensitivity for ADC
}

void loop() {
  button.read(); // Continuously read the status of the button. 
  pressed = button.isPressed();
  blink_red_led_startup();
  display_sens_level();
  set_sensitivity();
    
  // Run main program
  if (!set && !_setup) {
    green_led_block_mode();
    blink_green_led();
    check_sensor();
  }
}