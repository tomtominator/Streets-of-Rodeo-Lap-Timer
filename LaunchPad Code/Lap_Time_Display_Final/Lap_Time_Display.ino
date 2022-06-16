// ---------- Lap Time Counter ----------
// Using Ultrasonic Sensor HC-SR04
// Base code taken from: https://create.arduino.cc/projecthub/abdularbi17/ultrasonic-sensor-hc-sr04-with-arduino-tutorial-327ff6
// MSQC4911C Display data sheet: https://www.allelectronics.com/mas_assets/media/allelectronics2018/spec/FDA-5.pdf
// Written by: Thomas Matthew 5/27/2022


// ---------------- LAP TIME MEASUREMENT + HC-SR04 SECTION ----------------
#define echoPin P6_0
#define trigPin P6_1


#define SAMPLE_RATE 50 //Sample every 50ms = .05s
#define LAP_COOLDOWN 40 //Laps can happen only every LAP_COOLDOWN * SAMPLE_RATE (= 2) sec

#define WOBBLE 5 // Must change more than this range to be considered a real change in measured distance
#define PASS_DIS_CHANGE 30 // Distance decreases by this much for a valid pass - lower to make more sensitive. Raise to decrease noise

// Define Variables
long duration; // duration of sound wave travel
int distance; //  distance measurement
int last_distance;
int laps; // number of times a object passes
int lap_cooldown_cnt;

float lap_time; //Lap time in ms
float ongoing_lap_time;
long lap_start;
long lap_end;
// ---------------- END ----------------



// ---------------- 7 SEGMENT DISPLAY SECTION ----------------
#define digit_1 P2_5 // pin 1
#define digit_2 P8_2 // 2
#define digit_3 P2_3 // 6
#define digit_4 P8_1 // 8
#define A P1_4 // 14
#define B P2_4 // 16
#define C P1_3 // 13
#define D P4_3 // 3
#define E P4_0 // 5
#define F P1_2 // 11
#define G P1_5 // 15
#define DP P3_7 // 7

//#define SWITCHING_SPEED 10 //how quickly rotates between digits in ms
// = SAMPLE_RATE/4

// Truth table for segements for digit
// ith Row holds the segments for the ith digit in A,B,C, ... order
// 1 value indicates light up that segment
int dsegs[10][7] = {
  {1, 1, 1, 1, 1, 1, 0}, /*0*/
  {0, 1, 1, 0, 0, 0, 0}, /*1*/
  {1, 1, 0, 1, 1, 0, 1}, /*2*/
  {1, 1, 1, 1, 0, 0, 1}, /*3*/
  {0, 1, 1, 0, 0, 1, 1}, /*4*/
  {1, 0, 1, 1, 0, 1, 1}, /*5*/
  {1, 0, 1, 1, 1, 1, 1}, /*6*/
  {1, 1, 1, 0, 0, 0, 0}, /*7*/
  {1, 1, 1, 1, 1, 1, 1}, /*8*/
  {1, 1, 1, 1, 0, 1, 1}  /*9*/
  };

//Decimal number to be displayed (converted to display_*)
float display_number;
int temp;

// These set what digits are being displayed
int display_digit_1;
int display_digit_2;
int display_digit_3;
int display_digit_4;
int decimal_place; //encodes where to place decimal point
// (1, ones number, 2 tens number, 3 hundreds number, 4 thousands number)
// ---------------- END ----------------


void setup() {
  laps = 1;
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600); 
  Serial.println("Ultrasonic Sensor HC-SR04 Test"); 
  Serial.println("with TI Launchpad MSP-EXP430F5529LP");

  // ----------------------------------------------------

  pinMode(digit_1, OUTPUT);
  pinMode(digit_2, OUTPUT);
  pinMode(digit_3, OUTPUT);
  pinMode(digit_4, OUTPUT);
  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);
  pinMode(D, OUTPUT);
  pinMode(E, OUTPUT);
  pinMode(F, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(DP, OUTPUT);
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
  lap_end = millis();
  ongoing_lap_time = lap_end - lap_start;
  ongoing_lap_time = ongoing_lap_time / 1000;
   
  //Check that distance has changed outside of some range to account for noise and see if the measured distance has gotten a lot closer
  if (lap_cooldown_cnt > LAP_COOLDOWN && (distance - PASS_DIS_CHANGE) > 0 && !((last_distance - WOBBLE) < distance && distance < (last_distance + WOBBLE)) && (distance - PASS_DIS_CHANGE) < last_distance) { 
    lap_time = ongoing_lap_time;
    Serial.println("------------------------Object passed----------------------");
    Serial.print("Lap Number: ");
    Serial.println(laps);
    Serial.print("Lap Time: ");
    Serial.println(lap_time);
    
    laps = laps + 1;
    lap_cooldown_cnt = 0;
    lap_start = millis();
  }

  last_distance = distance;
  lap_cooldown_cnt = lap_cooldown_cnt + 1;

  // ------------------- DISPLAY RECORDED TIME -------------------

  //Display lap time for [LAP_COOLDOWN] seconds after object passes
  // else display the time spent in current lap
  if (lap_cooldown_cnt < LAP_COOLDOWN) {
    display_number = lap_time;
  } else {
    display_number = ongoing_lap_time;
  }
  

  
  //Get the first 4 digits of input float
  if (display_number > 1000) {      temp = display_number;          decimal_place = 4;
  } else if(display_number > 100) { temp = display_number * 10;     decimal_place = 3;
  } else if(display_number > 10) {  temp = display_number * 100;    decimal_place = 2;
  } else {                        temp = display_number * 1000;   decimal_place = 1; }
  display_digit_1 = temp / 1000;
  display_digit_2 = (temp / 100) % 10;
  display_digit_3 = (temp / 10) % 10;
  display_digit_4 = temp % 10;
 
  
  //1st Digit:
  digitalWrite(digit_1, HIGH);
  digitalWrite(digit_2, LOW);
  digitalWrite(digit_3, LOW);
  digitalWrite(digit_4, LOW);
  if (dsegs[display_digit_1][0] == 1) { digitalWrite(A, LOW); } else { digitalWrite(A, HIGH); }
  if (dsegs[display_digit_1][1] == 1) { digitalWrite(B, LOW); } else { digitalWrite(B, HIGH); }
  if (dsegs[display_digit_1][2] == 1) { digitalWrite(C, LOW); } else { digitalWrite(C, HIGH); }
  if (dsegs[display_digit_1][3] == 1) { digitalWrite(D, LOW); } else { digitalWrite(D, HIGH); }
  if (dsegs[display_digit_1][4] == 1) { digitalWrite(E, LOW); } else { digitalWrite(E, HIGH); }
  if (dsegs[display_digit_1][5] == 1) { digitalWrite(F, LOW); } else { digitalWrite(F, HIGH); }
  if (dsegs[display_digit_1][6] == 1) { digitalWrite(G, LOW); } else { digitalWrite(G, HIGH); }
  if (decimal_place == 1 ) { digitalWrite(DP, LOW); } else { digitalWrite(DP, HIGH); }

  delay(SAMPLE_RATE/4);

  digitalWrite(digit_1, LOW);
  digitalWrite(digit_2, HIGH);
  digitalWrite(digit_3, LOW);
  digitalWrite(digit_4, LOW);
  if (dsegs[display_digit_2][0] == 1) { digitalWrite(A, LOW); } else { digitalWrite(A, HIGH); }
  if (dsegs[display_digit_2][1] == 1) { digitalWrite(B, LOW); } else { digitalWrite(B, HIGH); }
  if (dsegs[display_digit_2][2] == 1) { digitalWrite(C, LOW); } else { digitalWrite(C, HIGH); }
  if (dsegs[display_digit_2][3] == 1) { digitalWrite(D, LOW); } else { digitalWrite(D, HIGH); }
  if (dsegs[display_digit_2][4] == 1) { digitalWrite(E, LOW); } else { digitalWrite(E, HIGH); }
  if (dsegs[display_digit_2][5] == 1) { digitalWrite(F, LOW); } else { digitalWrite(F, HIGH); }
  if (dsegs[display_digit_2][6] == 1) { digitalWrite(G, LOW); } else { digitalWrite(G, HIGH); }
  if (decimal_place == 2 ) { digitalWrite(DP, LOW); } else { digitalWrite(DP, HIGH); }

  delay(SAMPLE_RATE/4);

  digitalWrite(digit_1, LOW);
  digitalWrite(digit_2, LOW);
  digitalWrite(digit_3, HIGH);
  digitalWrite(digit_4, LOW);
  if (dsegs[display_digit_3][0] == 1) { digitalWrite(A, LOW); } else { digitalWrite(A, HIGH); }
  if (dsegs[display_digit_3][1] == 1) { digitalWrite(B, LOW); } else { digitalWrite(B, HIGH); }
  if (dsegs[display_digit_3][2] == 1) { digitalWrite(C, LOW); } else { digitalWrite(C, HIGH); }
  if (dsegs[display_digit_3][3] == 1) { digitalWrite(D, LOW); } else { digitalWrite(D, HIGH); }
  if (dsegs[display_digit_3][4] == 1) { digitalWrite(E, LOW); } else { digitalWrite(E, HIGH); }
  if (dsegs[display_digit_3][5] == 1) { digitalWrite(F, LOW); } else { digitalWrite(F, HIGH); }
  if (dsegs[display_digit_3][6] == 1) { digitalWrite(G, LOW); } else { digitalWrite(G, HIGH); }
  if (decimal_place == 3 ) { digitalWrite(DP, LOW); } else { digitalWrite(DP, HIGH); }
  
  delay(SAMPLE_RATE/4);

  digitalWrite(digit_1, LOW);
  digitalWrite(digit_2, LOW);
  digitalWrite(digit_3, LOW);
  digitalWrite(digit_4, HIGH);
  if (dsegs[display_digit_4][0] == 1) { digitalWrite(A, LOW); } else { digitalWrite(A, HIGH); }
  if (dsegs[display_digit_4][1] == 1) { digitalWrite(B, LOW); } else { digitalWrite(B, HIGH); }
  if (dsegs[display_digit_4][2] == 1) { digitalWrite(C, LOW); } else { digitalWrite(C, HIGH); }
  if (dsegs[display_digit_4][3] == 1) { digitalWrite(D, LOW); } else { digitalWrite(D, HIGH); }
  if (dsegs[display_digit_4][4] == 1) { digitalWrite(E, LOW); } else { digitalWrite(E, HIGH); }
  if (dsegs[display_digit_4][5] == 1) { digitalWrite(F, LOW); } else { digitalWrite(F, HIGH); }
  if (dsegs[display_digit_4][6] == 1) { digitalWrite(G, LOW); } else { digitalWrite(G, HIGH); }
  if (decimal_place == 4 ) { digitalWrite(DP, LOW); } else { digitalWrite(DP, HIGH); }

  delay(SAMPLE_RATE/4);
  




  // ------------------------------------------------------------
  //delay(SAMPLE_RATE);
}
