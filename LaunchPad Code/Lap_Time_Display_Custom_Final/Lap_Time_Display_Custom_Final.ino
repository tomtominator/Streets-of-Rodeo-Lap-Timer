// ---------- Lap Time Counter ----------
// Using Ultrasonic Sensor HC-SR04
// Base sensor code taken from: https://create.arduino.cc/projecthub/abdularbi17/ultrasonic-sensor-hc-sr04-with-arduino-tutorial-327ff6
// MSQC4911C Display data sheet: https://www.allelectronics.com/mas_assets/media/allelectronics2018/spec/FDA-5.pdf
//   Note: our custom display has a wire for each of the 30 segments (7 segment * 4 digit + 2 decimal places) where LOW = on, HIGH = off
//    but we only power 1 digit at a time to limit how much current the microcontroller has to sink. Powering 1 digit it sinks about 45mA
//   Note 2: pins 6_1 and 6_2 on the microcontroller don't work; can't write a high value to them

// Written by: Thomas Matthew 6/6/2022
// Last Updated: 7/1/22

// --- Feature Flags ---
// 1/0 values for enable/disable of certain sections of the code
#define DEBUG 0 // Currently only enables/disables serial communication with computer


// --- End ---

//---Parameters---
#define REFRESH_RATE 20 // Decrease REFRESH_RATE to refresh digits quicker and sample more often
                        // Digit refreshes every REFERSH_RATE/4 and distance sensor samples evert REFERSH_RATE
                        // Note 1: LAP_COOLDOWN dependency on REFRESH_RATE
                        // Note 2: that sensor operation takes some time between displaying the digits but it micro sec vs milli sec (if sensor is not working properly it will be significant amount of time)

#define SENSE_COOLDOWN 10 // Only operate the sensor every SENSE_COOLDOWN loops of the main loop (sense rate: SENSE_COOLDOWN * REFRESH_RATE = 150ms)
#define LAP_COOLDOWN 125 //Laps can happen only every LAP_COOLDOWN(125) * REFRESH_RATE(20) = 2.5 sec
#define PASS_DIS_CHANGE 30 // Distance decreases by this much for a valid pass - lower to make more sensitive. Raise to decrease noise
//---End Parameters---

// ---------------- LAP TIME MEASUREMENT + HC-SR04 SECTION ----------------

#define echoPin P8_2
#define trigPin P8_1

// Variables for distance sensor
long duration; // duration of sound wave travel
int distance; //  distance measurement
int last_distance;
int laps; // number of times a object passes
int lap_cooldown_cnt;
int sense_cooldown_cnt;

float lap_time; //Lap time in ms
float ongoing_lap_time;
long lap_start;
long lap_end;
// ---------------- END ----------------


// ---------------- CUSTOM 7 SEGMENT DISPLAY SECTION ----------------

// Unfortunate Note: ABC.. wiring is reflected across xy plane. 
//                   Ording of letters represents ordering of wires into the microcontroller. ie P6_5 is adjacent to P3_4. and 6_5 was originally meant to be segment A_1
//                   ABCDEFG -> ECBAFEG (this is not the case for all digits) (we were rushed when wiring this unfortunately)
//                   (for digit 1 FEG -> GEF)
//1st Digit Pins  
#define D_1 P6_5 // blue
#define C_1 P3_4 // 
#define B_1 P3_3 // 
#define A_1 P1_6 // 
#define G_1 P6_6 // 
#define E_1 P3_2 // 
#define F_1 P2_7 // red 
#define DP_1 P3_5 // black
//2nd Digit Pins
#define D_2 P6_0 // purple 
#define C_2 P6_1 // 
#define B_2 P4_0 // 
#define A_2 P4_2 // 
#define F_2 P6_4 // 
#define E_2 P7_0 // 
#define G_2 P3_6 // black
#define DP_2 P4_1 // red
//3rd Digit Pins //ABCDEF->GEFABCD
#define G_3 P2_5 // black
#define E_3 P2_4 // 
#define F_3 P1_5 // 
#define A_3 P1_4 // 
#define B_3 P1_3 // 
#define C_3 P1_2 // 
#define D_3 P4_3 // gray
//4th Digit Pins
#define D_4 P2_0 // red
#define C_4 P2_2 // 
#define B_4 P7_4 // 
#define A_4 P3_0 // 
#define F_4 P3_1 // 
#define E_4 P2_6 // 
#define G_4 P2_3 // orange
 

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
  if (DEBUG) {
    Serial.begin(9600); 
    Serial.println("Ultrasonic Sensor HC-SR04 Test"); 
    Serial.println("with TI Launchpad MSP-EXP430F5529LP");
  }

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
  //pinMode(DP_3, OUTPUT); no DP 3

  pinMode(A_4, OUTPUT);
  pinMode(B_4, OUTPUT);
  pinMode(C_4, OUTPUT);
  pinMode(D_4, OUTPUT);
  pinMode(E_4, OUTPUT);
  pinMode(F_4, OUTPUT);
  pinMode(G_4, OUTPUT);
  //pinMode(DP_4, OUTPUT); no DP 4
}

void loop() {
  
  if (lap_cooldown_cnt > LAP_COOLDOWN && sense_cooldown_cnt > SENSE_COOLDOWN) {
    sense_cooldown_cnt = 0;

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
    
    // Object Passed if distance decreased by at least PASS_DIS_CHANGE 
    //    Note: object must be further than PASS_DIS_CHANGE
    if ((distance - PASS_DIS_CHANGE) > 0 && (distance - PASS_DIS_CHANGE) < last_distance) { 
      lap_time = ongoing_lap_time;
      laps = laps + 1;
      lap_cooldown_cnt = 0;
      lap_start = millis();

      if (DEBUG) {
        Serial.println("------------------------Object passed----------------------");
        Serial.print("Lap Number: ");
        Serial.println(laps);
        Serial.print("Lap Time: ");
        Serial.println(lap_time);
      }
    }

    last_distance = distance;
  }

  lap_cooldown_cnt = lap_cooldown_cnt + 1;
  sense_cooldown_cnt = sense_cooldown_cnt + 1;

  // ------------------- DISPLAY RECORDED TIME -------------------

  // Note: ongoing_lap_time only updates on a SENSE

  if (sense_cooldown_cnt == 1 || lap_cooldown_cnt == 1) { // Display number doesnt change during LAP_COOLDOWN so dont recalculate
    //Display lap time for [LAP_COOLDOWN] seconds after object passes
    // else display the time spent in current lap
    if (lap_cooldown_cnt < LAP_COOLDOWN) {
      display_number = lap_time;
    } else {
      display_number = ongoing_lap_time;
    }

  //Get the first 4 digits of input unsigned long
    if (display_number > 1000) {      temp = display_number;          decimal_place = 4;
    } else if(display_number > 100) { temp = display_number * 10;     decimal_place = 3;
    } else if(display_number > 10) {  temp = display_number * 100;    decimal_place = 2;
    } else {                        temp = display_number * 1000;   decimal_place = 1; }
    display_digit_1 = temp / 1000;
    display_digit_2 = (temp / 100) % 10;
    display_digit_3 = (temp / 10) % 10;
    display_digit_4 = temp % 10;
  }
  
  // --- DISPLAY DIGITS ---
  // by scanning through them
  // --- 1st Digit ---
  if (dsegs[display_digit_1][0] == 1) { digitalWrite(A_1, LOW); } else { digitalWrite(A_1, HIGH); }
  if (dsegs[display_digit_1][1] == 1) { digitalWrite(B_1, LOW); } else { digitalWrite(B_1, HIGH); }
  if (dsegs[display_digit_1][2] == 1) { digitalWrite(C_1, LOW); } else { digitalWrite(C_1, HIGH); }
  if (dsegs[display_digit_1][3] == 1) { digitalWrite(D_1, LOW); } else { digitalWrite(D_1, HIGH); }
  if (dsegs[display_digit_1][4] == 1) { digitalWrite(E_1, LOW); } else { digitalWrite(E_1, HIGH); }
  if (dsegs[display_digit_1][5] == 1) { digitalWrite(F_1, LOW); } else { digitalWrite(F_1, HIGH); }
  if (dsegs[display_digit_1][6] == 1) { digitalWrite(G_1, LOW); } else { digitalWrite(G_1, HIGH); }
  if (decimal_place == 1 ) { digitalWrite(DP_1, LOW); } else { digitalWrite(DP_1, HIGH); }

  delay(REFRESH_RATE/4);
  //Turn off digit:
  digitalWrite(A_1, HIGH);
  digitalWrite(B_1, HIGH);
  digitalWrite(C_1, HIGH);
  digitalWrite(D_1, HIGH);
  digitalWrite(E_1, HIGH);
  digitalWrite(F_1, HIGH);
  digitalWrite(G_1, HIGH);
  digitalWrite(DP_1, HIGH);

  // --- 2nd Digit ---
  if (dsegs[display_digit_2][0] == 1) { digitalWrite(A_2, LOW); } else { digitalWrite(A_2, HIGH); }
  if (dsegs[display_digit_2][1] == 1) { digitalWrite(B_2, LOW); } else { digitalWrite(B_2, HIGH); }
  if (dsegs[display_digit_2][2] == 1) { digitalWrite(C_2, LOW); } else { digitalWrite(C_2, HIGH); }
  if (dsegs[display_digit_2][3] == 1) { digitalWrite(D_2, LOW); } else { digitalWrite(D_2, HIGH); }
  if (dsegs[display_digit_2][4] == 1) { digitalWrite(E_2, LOW); } else { digitalWrite(E_2, HIGH); }
  if (dsegs[display_digit_2][5] == 1) { digitalWrite(F_2, LOW); } else { digitalWrite(F_2, HIGH); }
  if (dsegs[display_digit_2][6] == 1) { digitalWrite(G_2, LOW); } else { digitalWrite(G_2, HIGH); }
  if (decimal_place == 2 ) { digitalWrite(DP_2, LOW); } else { digitalWrite(DP_2, HIGH); }

  delay(REFRESH_RATE/4);
  //Turn off digit:
  digitalWrite(A_2, HIGH);
  digitalWrite(B_2, HIGH);
  digitalWrite(C_2, HIGH);
  digitalWrite(D_2, HIGH);
  digitalWrite(E_2, HIGH);
  digitalWrite(F_2, HIGH);
  digitalWrite(G_2, HIGH);
  digitalWrite(DP_2, HIGH);

  // --- 3rd Digit ---
  if (dsegs[display_digit_3][0] == 1) { digitalWrite(A_3, LOW); } else { digitalWrite(A_3, HIGH); }
  if (dsegs[display_digit_3][1] == 1) { digitalWrite(B_3, LOW); } else { digitalWrite(B_3, HIGH); }
  if (dsegs[display_digit_3][2] == 1) { digitalWrite(C_3, LOW); } else { digitalWrite(C_3, HIGH); }
  if (dsegs[display_digit_3][3] == 1) { digitalWrite(D_3, LOW); } else { digitalWrite(D_3, HIGH); }
  if (dsegs[display_digit_3][4] == 1) { digitalWrite(E_3, LOW); } else { digitalWrite(E_3, HIGH); }
  if (dsegs[display_digit_3][5] == 1) { digitalWrite(F_3, LOW); } else { digitalWrite(F_3, HIGH); }
  if (dsegs[display_digit_3][6] == 1) { digitalWrite(G_3, LOW); } else { digitalWrite(G_3, HIGH); }
  
  delay(REFRESH_RATE/4);
  digitalWrite(A_3, HIGH);
  digitalWrite(B_3, HIGH);
  digitalWrite(C_3, HIGH);
  digitalWrite(D_3, HIGH);
  digitalWrite(E_3, HIGH);
  digitalWrite(F_3, HIGH);
  digitalWrite(G_3, HIGH);

// -- 4th Digit ---
  if (dsegs[display_digit_4][0] == 1) { digitalWrite(A_4, LOW); } else { digitalWrite(A_4, HIGH); }
  if (dsegs[display_digit_4][1] == 1) { digitalWrite(B_4, LOW); } else { digitalWrite(B_4, HIGH); }
  if (dsegs[display_digit_4][2] == 1) { digitalWrite(C_4, LOW); } else { digitalWrite(C_4, HIGH); }
  if (dsegs[display_digit_4][3] == 1) { digitalWrite(D_4, LOW); } else { digitalWrite(D_4, HIGH); }
  if (dsegs[display_digit_4][4] == 1) { digitalWrite(E_4, LOW); } else { digitalWrite(E_4, HIGH); }
  if (dsegs[display_digit_4][5] == 1) { digitalWrite(F_4, LOW); } else { digitalWrite(F_4, HIGH); }
  if (dsegs[display_digit_4][6] == 1) { digitalWrite(G_4, LOW); } else { digitalWrite(G_4, HIGH); }

  delay(REFRESH_RATE/4);
  digitalWrite(A_4, HIGH);
  digitalWrite(B_4, HIGH);
  digitalWrite(C_4, HIGH);
  digitalWrite(D_4, HIGH);
  digitalWrite(E_4, HIGH);
  digitalWrite(F_4, HIGH);
  digitalWrite(G_4, HIGH);



  // ------------------------------------------------------------
  //delay(REFRESH_RATE);
}
