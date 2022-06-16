// ---------- Lap Time Counter ----------
// Using Ultrasonic Sensor HC-SR04
// Base code taken from: https://create.arduino.cc/projecthub/abdularbi17/ultrasonic-sensor-hc-sr04-with-arduino-tutorial-327ff6
// 
// The display we made can set any of the 28 segments to on/off at the same time. To mimic the 
// MSQC4911C Display (data sheet: https://www.allelectronics.com/mas_assets/media/allelectronics2018/spec/FDA-5.pdf)
// and to reduce the total current draw we only power 1 digit at a time and flip digits rapidly so the human eye cannot tell
//
// 
// Written by: Thomas Matthew 6/4/2022


// ---------------- LAP TIME MEASUREMENT + HC-SR04 SECTION ----------------
#define echoPin P6_0
#define trigPin P6_1


#define SAMPLE_RATE 1000 //Sample every 50ms = .05s
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
//1st Digit Pins
#define A_1 P2_5 // 14
#define B_1 P2_4 // 16
#define C_1 P1_5 // 13
#define D_1 P1_4 // 3
#define E_1 P1_3 // 5
#define F_1 P1_2 // 11
#define G_1 P4_3 // 15
#define DP_1 P3_7 // 7
//2nd Digit Pins
#define A_2 P6_5 // 
#define B_2 P6_5 // 
#define C_2 P6_5 // 
#define D_2 P6_5 // 
#define E_2 P6_5 // 
#define F_2 P6_5 // 
#define G_2 P6_5 // 
#define DP_2 P6_5 // 
//3rd Digit Pins
#define A_3 P6_5 // 
#define B_3 P6_5 // 
#define C_3 P6_5 // 
#define D_3 P6_5 // 
#define E_3 P6_5 // 
#define F_3 P6_5 // 
#define G_3 P6_5 // 
#define DP_3 P6_5 // 
//4th Digit Pins
#define A_4 P6_5 // 
#define B_4 P6_5 // 
#define C_4 P6_5 // 
#define D_4 P6_5 // 
#define E_4 P6_5 // 
#define F_4 P6_5 // 
#define G_4 P6_5 // 
#define DP_4 P6_5 // 

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

  // ----------------------------------------------------
  pinMode(A_1, OUTPUT);
  pinMode(B_1, OUTPUT);
  pinMode(C_1, OUTPUT);
  pinMode(D_1, OUTPUT);
  pinMode(E_1, OUTPUT);
  pinMode(F_1, OUTPUT);
  pinMode(G_1, OUTPUT);
  pinMode(DP_1, OUTPUT);

  pinMode(A_2, OUTPUT);
  pinMode(B_2, OUTPUT);
  pinMode(C_2, OUTPUT);
  pinMode(D_2, OUTPUT);
  pinMode(E_2, OUTPUT);
  pinMode(F_2, OUTPUT);
  pinMode(G_2, OUTPUT);
  pinMode(DP_2, OUTPUT);

  pinMode(A_3, OUTPUT);
  pinMode(B_3, OUTPUT);
  pinMode(C_3, OUTPUT);
  pinMode(D_3, OUTPUT);
  pinMode(E_3, OUTPUT);
  pinMode(F_3, OUTPUT);
  pinMode(G_3, OUTPUT);
  pinMode(DP_3, OUTPUT);

  pinMode(A_4, OUTPUT);
  pinMode(B_4, OUTPUT);
  pinMode(C_4, OUTPUT);
  pinMode(D_4, OUTPUT);
  pinMode(E_4, OUTPUT);
  pinMode(F_4, OUTPUT);
  pinMode(G_4, OUTPUT);
  pinMode(DP_4, OUTPUT);
}

void loop() {
  /*
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
    
    laps = laps + 1;
    lap_cooldown_cnt = 0;
    lap_start = millis();
  }

  last_distance = distance;
  lap_cooldown_cnt = lap_cooldown_cnt + 1;

  */
  // ------------------- DISPLAY RECORDED TIME -------------------

  // Display lap time for [LAP_COOLDOWN] seconds after object passes
  // else display the time spent in current lap
  /*if (lap_cooldown_cnt < LAP_COOLDOWN) {
    display_number = lap_time;
  } else {
    display_number = ongoing_lap_time;
  }*/

  

  
  //Get the first 4 digits of input float
  /*if (display_number > 1000) {      temp = display_number;          decimal_place = 4;
  } else if(display_number > 100) { temp = display_number * 10;     decimal_place = 3;
  } else if(display_number > 10) {  temp = display_number * 100;    decimal_place = 2;
  } else {                        temp = display_number * 1000;   decimal_place = 1; }
  display_digit_1 = temp / 1000;
  display_digit_2 = (temp / 100) % 10;
  display_digit_3 = (temp / 10) % 10;
  display_digit_4 = temp % 10;*/

  display_digit_1 = 1;
  decimal_place = 2;
 
  
  //1st Digit:
  digitalWrite(A_1, LOW);
  digitalWrite(B_1, LOW);
  digitalWrite(C_1, LOW);
  digitalWrite(D_1, LOW);
  digitalWrite(E_1, LOW);
  digitalWrite(F_1, LOW);
  digitalWrite(G_1, LOW);  
  delay(SAMPLE_RATE);

  digitalWrite(A_1, HIGH);
  digitalWrite(B_1, HIGH);
  digitalWrite(C_1, HIGH);
  digitalWrite(D_1, HIGH);
  digitalWrite(E_1, HIGH);
  digitalWrite(F_1, HIGH);
  digitalWrite(G_1, HIGH);

  delay(SAMPLE_RATE);


/*
  //2nd Digit:
  if (dsegs[display_digit_2][0] == 1) { digitalWrite(A_2, LOW); } else { digitalWrite(A_2, HIGH); }
  if (dsegs[display_digit_2][1] == 1) { digitalWrite(B_2, LOW); } else { digitalWrite(B_2, HIGH); }
  if (dsegs[display_digit_2][2] == 1) { digitalWrite(C_2, LOW); } else { digitalWrite(C_2, HIGH); }
  if (dsegs[display_digit_2][3] == 1) { digitalWrite(D_2, LOW); } else { digitalWrite(D_2, HIGH); }
  if (dsegs[display_digit_2][4] == 1) { digitalWrite(E_2, LOW); } else { digitalWrite(E_2, HIGH); }
  if (dsegs[display_digit_2][5] == 1) { digitalWrite(F_2, LOW); } else { digitalWrite(F_2, HIGH); }
  if (dsegs[display_digit_2][6] == 1) { digitalWrite(G_2, LOW); } else { digitalWrite(G_2, HIGH); }
  if (decimal_place == 2 ) { digitalWrite(DP_2, LOW); } else { digitalWrite(DP_2, HIGH); }

  delay(SAMPLE_RATE/4);

  digitalWrite(A_2, HIGH);
  digitalWrite(B_2, HIGH);
  digitalWrite(C_2, HIGH);
  digitalWrite(D_2, HIGH);
  digitalWrite(E_2, HIGH);
  digitalWrite(F_2, HIGH);
  digitalWrite(G_2, HIGH);
  digitalWrite(DP_2, HIGH);

  //3rd Digit:
  if (dsegs[display_digit_3][0] == 1) { digitalWrite(A_3, LOW); } else { digitalWrite(A_3, HIGH); }
  if (dsegs[display_digit_3][1] == 1) { digitalWrite(B_3, LOW); } else { digitalWrite(B_3, HIGH); }
  if (dsegs[display_digit_3][2] == 1) { digitalWrite(C_3, LOW); } else { digitalWrite(C_3, HIGH); }
  if (dsegs[display_digit_3][3] == 1) { digitalWrite(D_3, LOW); } else { digitalWrite(D_3, HIGH); }
  if (dsegs[display_digit_3][4] == 1) { digitalWrite(E_3, LOW); } else { digitalWrite(E_3, HIGH); }
  if (dsegs[display_digit_3][5] == 1) { digitalWrite(F_3, LOW); } else { digitalWrite(F_3, HIGH); }
  if (dsegs[display_digit_3][6] == 1) { digitalWrite(G_3, LOW); } else { digitalWrite(G_3, HIGH); }
  if (decimal_place == 3 ) { digitalWrite(DP_3, LOW); } else { digitalWrite(DP_3, HIGH); }
  
  delay(SAMPLE_RATE/4);

  digitalWrite(A_3, HIGH);
  digitalWrite(B_3, HIGH);
  digitalWrite(C_3, HIGH);
  digitalWrite(D_3, HIGH);
  digitalWrite(E_3, HIGH);
  digitalWrite(F_3, HIGH);
  digitalWrite(G_3, HIGH);
  digitalWrite(DP_3, HIGH);

  //4th Digit:
  if (dsegs[display_digit_4][0] == 1) { digitalWrite(A_4, LOW); } else { digitalWrite(A_4, HIGH); }
  if (dsegs[display_digit_4][1] == 1) { digitalWrite(B_4, LOW); } else { digitalWrite(B_4, HIGH); }
  if (dsegs[display_digit_4][2] == 1) { digitalWrite(C_4, LOW); } else { digitalWrite(C_4, HIGH); }
  if (dsegs[display_digit_4][3] == 1) { digitalWrite(D_4, LOW); } else { digitalWrite(D_4, HIGH); }
  if (dsegs[display_digit_4][4] == 1) { digitalWrite(E_4, LOW); } else { digitalWrite(E_4, HIGH); }
  if (dsegs[display_digit_4][5] == 1) { digitalWrite(F_4, LOW); } else { digitalWrite(F_4, HIGH); }
  if (dsegs[display_digit_4][6] == 1) { digitalWrite(G_4, LOW); } else { digitalWrite(G_4, HIGH); }
  if (decimal_place == 4 ) { digitalWrite(DP_4, LOW); } else { digitalWrite(DP_4, HIGH); }

  delay(SAMPLE_RATE/4);
  
  digitalWrite(A_4, HIGH);
  digitalWrite(B_4, HIGH);
  digitalWrite(C_4, HIGH);
  digitalWrite(D_4, HIGH);
  digitalWrite(E_4, HIGH);
  digitalWrite(F_4, HIGH);
  digitalWrite(G_4, HIGH);
  digitalWrite(DP_4, HIGH);


*/

  // ------------------------------------------------------------
  //delay(SAMPLE_RATE);
}
