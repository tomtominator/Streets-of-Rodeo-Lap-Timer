// ---------- Lap Time Counter ----------
// Using Ultrasonic Sensor HC-SR04
// Base code taken from: https://create.arduino.cc/projecthub/abdularbi17/ultrasonic-sensor-hc-sr04-with-arduino-tutorial-327ff6
// Written by: Thomas Matthew


//Pins
#define echoPin P6_0
#define trigPin P6_1


#define SAMPLE_RATE 100 //Sample every 100ms = .1s
#define LAP_COOLDOWN 10 //Laps can happen only every 1 sec

#define WOBBLE 5 // Must change more than this range to be considered a real change in measured distance
#define PASS_DIS_CHANGE 30 // Distance decreases by this much for a valid pass - lower to make more sensitive. Raise to decrease noise

// Define Variables
long duration; // duration of sound wave travel
int distance; //  distance measurement
int last_distance;
int laps; // number of times a object passes
int lap_cooldown_cnt;

float lap_time; //Lap time in ms
long lap_start;
long lap_end;

void setup() {
  laps = 1;
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600); 
  Serial.println("Ultrasonic Sensor HC-SR04 Test"); 
  Serial.println("with TI Launchpad MSP-EXP430F5529LP");
}
void loop() {

  //----Measure Distance using HC-SR04----
  digitalWrite(trigPin, LOW); // Clears the trigPin condition
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH); // Reads the echoPin, returns the sound wave travel time in microseconds
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)


  //----Lap Time Measurement----
  //Check that distance has changed outside of some range to account for noise and see if the measured distance has gotten a lot closer
  if (lap_cooldown_cnt > LAP_COOLDOWN && (distance - PASS_DIS_CHANGE) > 0 && !((last_distance - WOBBLE) < distance && distance < (last_distance + WOBBLE)) && (distance - PASS_DIS_CHANGE) < last_distance) { 
    lap_end = millis();
    lap_time = lap_end - lap_start;
    Serial.println("------------------------Object passed----------------------");
    Serial.print("Lap Number: ");
    Serial.println(laps);
    Serial.print("Lap Time: ");
    Serial.println(lap_time / 1000);
    
    laps = laps + 1;
    lap_cooldown_cnt = 0;
    lap_start = millis();
  }

  last_distance = distance;
  lap_cooldown_cnt = lap_cooldown_cnt + 1;

  delay(SAMPLE_RATE);
}
