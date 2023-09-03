// Text last updated 2023-09-03 Sun 22:23
// Citations in this program will be presented as ^[n], and the citations can be found in the documentation.

#include <Stepper.h>  // Including required libraries
#include <LowPower.h>
#include <Time.h>

// Declaring pin numbers to keep them explicit
const int soilPin = 0; // Pin from SEN0192, ANALOG
const int relayPin =8;  // Pin to HW482
const int motorPin1 = 9; // Pin 1 to ULN2003
const int motorPin2 = 10; // Pin 2 to ULN2003
const int motorPin3 = 11; // Pin 3 to ULN2003
const int motorPin4 = 12; // Pin 4 to ULN2003

// Declaring how many steps to have in a revolution for the stepper motor. Number taken from ^[1]
const int stepRef = 2038;

// Soil moisture sensor threshold, this should be determined by calibration.
// Higher # = drier. 350 is just the value I happened to get for dry soil.
const int drynessThreshold = 450;

// Set up a place to record whether the relay is live or not
bool danger;

// Set up nap lengths...
const int littleNap = 5;  // Nap length with inactive relay, minutes
const int bigNap = 15;    // Nap length with active relay, minutes

// Declaring stepper motor.
Stepper motor = Stepper(stepRef,motorPin1,motorPin2,motorPin3,motorPin4);

// Function to water the soil, i.e. open and then close the air opening.
bool water() {
  motor.setSpeed(10);
	motor.step((stepRef*0.5));  // Turn cw
  delay(3000);
  motor.step((stepRef*-0.5)); // Turn ccw
  return true;
}

void setup() {
    pinMode(relayPin,OUTPUT);   // Initializing output as output.
    // Motor pins do not need to be initialized as stepper.h does this for me.
    Serial.begin(9600); // Open serial port to PC, 9600 seems to be a standard bit rate ^[1]
    Serial.println("Hello world!"); // Sign of life
}

void loop() {

    // Checks:
    // Serial.println(analogRead(soilPin));
    // Serial.println(isAM());

    if (analogRead(soilPin) > drynessThreshold) { // If soil is dry
      water();
    }
    
    // Have the lamp on during daytime.
    // More complex schedules are easily possible in the if condition.
    if (isAM()) {
      digitalWrite(relayPin, HIGH); // Powers the relay  
      danger = true;
    } else {
      digitalWrite(relayPin, LOW);  // Shuts the relay
      danger = false;
    }

    if (danger) {
      delay(littleNap *60 *1000);    // If the relay is live, the device takes a 5 minute light snooze.
    } else {
      for (int i = 0 ; i < ((bigNap *60) /8)  ; i++) {   // If the relay is not live, the device should take 15 minute naps.
        LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); // Sadly this is the best method I could find with LowPower.
      }
    }

}
