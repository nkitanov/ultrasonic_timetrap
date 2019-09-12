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
unsigned long ONE_SECOND = 1000;
unsigned long THREE_SECONDS = 3000;
unsigned long FIVE_SECONDS = 5000;
int sens5 = 245; // EEPROM value for sensitivity 5m, etc.
int sens4 = 220; // 4.5m
int sens3 = 196; // 4m
int sens2 = 147; // 3m
int sens1 = 98;  // 2m
int sens0 = 49;  // 1m
int blinks;
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


// void two_click() {
//   if (!set && !_setup) {  // Do not run during initial phase and setup
//     if (block) {          // Toggle measure hold block with 2 click of the button
//       block = false;
//     } else {
//       block = true;
//     }
//   }
// }

 // Blink Green LED constanlty while working
void blink_green_led() {
  if (millis() > FIVE_SECONDS) {
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
  if (millis() < FIVE_SECONDS) {
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

// Get current sensitivity level
int get_sens_level() {
  if (sens_eeprom == sens5) return 6;
  if (sens_eeprom == sens4) return 5;
  if (sens_eeprom == sens3) return 4;
  if (sens_eeprom == sens2) return 3;
  if (sens_eeprom == sens1) return 2;
  if (sens_eeprom == sens0) return 1;
  return 0;
}

// Indicate sensitivity level with number of blinks
void show_sensitivity(int level) {
  while (blinks < level) {
    digitalWrite(LED_GREEN, HIGH); delay(500);
    digitalWrite(LED_GREEN, LOW); delay(ONE_SECOND);
    blinks++;
  }
}

// Display sensitivity level
// 1 blink --> 1m
// 2 blinks --> 2m
// 3 blinks --> 3m
// 4 blinks --> 4m
// 5 blinks --> 4.5m
// 6 blinks --> 5m
// no blinks --> unset
void display_sens_level() {
  if (millis() > FIVE_SECONDS && !display_sens_run && !_setup) {
    set = true;
    display_sens_run = true;  //Run it only one time

    int blink_sens_level = get_sens_level();
    show_sensitivity(blink_sens_level);
    delay(ONE_SECOND);
  }
  set = false;
}

 // Count button clicks during sensitivity setup
 void onPressed() {
   if (_setup) {
    clicks++;
   }
 }

// Get sensitivity based on number of clicks
// 0 clicks --> 5m
// 1 clicks --> 4.5m
// 2 clicks --> 4m
// 3 clicks --> 3m
// 4 clicks --> 2m
// 5 clicks --> 1m
// other    --> 5m
int get_clicks_sensitivity() {
  if (clicks == 5) return sens0;
  if (clicks == 4) return sens1;
  if (clicks == 3) return sens2;
  if (clicks == 2) return sens3;
  if (clicks == 1) return sens4;
  return sens5;
}

// Set sensitivity and store it in EEPROM pepmanently (reversed for failsafe)
void set_sensitivity() {
  if (_setup) {
    set = true;   // Keep in set mode to block run main until diplay of sense levels complete in main loop
    currentTime = millis();
    sens_eeprom = get_clicks_sensitivity();

    if (currentTime > exitSet) {    // Wait 10 seconds for input, set and exit
      sensitivity = sens_eeprom * 4;
      EEPROM.update(0, sens_eeprom);
      digitalWrite(LED_RED, HIGH); delay(THREE_SECONDS);
      digitalWrite(LED_RED, LOW); delay(ONE_SECOND);
      _setup = false;
    }
  } 
}

void setup_mode() {
  if (millis() < FIVE_SECONDS) {
    _setup = true;
    digitalWrite(LED_RED, HIGH); delay(THREE_SECONDS);
    digitalWrite(LED_RED, LOW);
    exitSet = currentTime + 10000; 
  }
}

// Check sensor for object in front
void check_sensor() {
  if (millis() > FIVE_SECONDS) {
    if (!pressed && !block) {
      if (millis() - timeMeasure > ONE_SECOND) { // Block measurement for 1sec after previous
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
  // button.onSequence(2, 2000, two_click);
  button.onPressedFor(THREE_SECONDS, setup_mode);
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