// libraries
#include <Servo.h>
#include "Adafruit_LiquidCrystal.h"

// pin definitions
#define FORCE_PIN 1
#define POT_PIN 0
#define LED1 5
#define LED2 6
#define LED3 7
#define EMG 2
#define ROACH 10

// constructors
Servo myservo; // creates servo object
Adafruit_LiquidCrystal lcd(0);

// variable declarations
int force_digi;
float force_volt;
int motor_duty;
int emg_digi;
float weakThresh = 20;
float strongThresh = 40;

// game state variables
int batterPoints = 0;  
int strikeCount = 0;  
int reactionTimePenalty = 500; // penalty time for weak flex (ms)
unsigned long reactionStartTime = 0; // start time for measuring reaction
bool isHit = false; // tracks if the force sensor was hit
bool isFlexDetected = false; // tracks if a flex was detected
bool flag_gameActive = false; // flag for game state
int flex_magnitude = 0;

// function declarations
void handleStrongFlex();
void handleStrike();

// setup code
void setup() {
  Serial.begin(9600);
  myservo.attach(9); // pin 9 for PWM on servo

  // lcd setup
  lcd.begin(16, 2); // specify LCD dimensions
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.setBacklight(HIGH);
  lcd.print("Batter Up!");

  // led setup
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);

  // roach setup
  pinMode(ROACH, OUTPUT);
}

// loop code
void loop() {
  // Read force sensor
  force_digi = analogRead(FORCE_PIN);
  force_volt = force_digi * (5.0 / 1023.0);
  
  // Servo & potentiometer control
  motor_duty = analogRead(POT_PIN);
  motor_duty = map(motor_duty, 0, 1023, 0, 180);
  myservo.write(motor_duty);
  delay(20);

  // EMG read-in
  emg_digi = analogRead(EMG);

  // STARTING GAME LOGIC
  if (force_volt >= 0.1) {
    isHit = true; 
    flag_gameActive = true; 
    reactionStartTime = millis();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Hit!");
    flex_magnitude = 0;
  }

  // Game active: monitor for flex or timeout
  while (flag_gameActive) {
    unsigned long currentTime = millis();
    isFlexDetected = false;

    // Continuously read EMG data
    emg_digi = analogRead(EMG);

    if (emg_digi >= strongThresh) {
      isFlexDetected = true;
      flex_magnitude = 2;
      // handleStrongFlex();
      Serial.println("flex detected (strong)"); 
    } 
    else if ((emg_digi >= weakThresh) && (emg_digi < strongThresh)) {
      isFlexDetected = true;
      if ((currentTime-reactionStartTime)<2000){
        if (flex_magnitude == 0){
          flex_magnitude = 1;
        }
      }
      // handleStrike();
      Serial.println("flex detected (weak)"); 
      // flag_gameActive = false;  // Exit game state
    }

    // If max reaction time elapsed and no flex detected
    if ((currentTime - reactionStartTime) > 3000) {
      handleStrike();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Batter Up!");
      flag_gameActive = false;
      if (flex_magnitude == 2){
        handleStrongFlex();
      }
      else if (flex_magnitude == 1){
        handleStrongFlex();
      }
      else{
        handleStrike();
      }
    }
  }

  delay(10);  // Short delay for loop timing
}

// Handles a strong flex
void handleStrongFlex() {
  if (digitalRead(LED1) == HIGH) {
    if (digitalRead(LED2) == HIGH) {
      if (digitalRead(LED3) == HIGH) {
        batterPoints++;  // Batter scores a point
        Serial.println("strong flex: batter scores one point");
      } else {
        digitalWrite(LED3, HIGH);  // Move to base 3
        Serial.println("strong flex: batter moved up to base 3");
        digitalWrite(ROACH, HIGH); 
        delay(5000); 
        digitalWrite(ROACH, LOW); 
      }
    } else {
      digitalWrite(LED2, HIGH);  // Move to base 2
      Serial.println("strong flex: batter moved up to base 2");
    }
  } else {
    digitalWrite(LED1, HIGH);  // Move to base 1
    Serial.println("strong flex: batter moved up to base 1");
  }
}

// Handles a strike
void handleStrike() {
  strikeCount++;
  Serial.println("strike"); 
}
