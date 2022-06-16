// MSQC4911C Display data sheet: https://www.allelectronics.com/mas_assets/media/allelectronics2018/spec/FDA-5.pdf
// 4 Digit 7 Segment Display Control Code
// Written by Thomas Matthew 5/27/2022

#define digit_1 P2_5 // 1
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

#define SWITCHING_SPEED 10 //how quickly rotates between digits in ms

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
float input_number;
int temp;

// These set what digits are being displayed
int display_1;
int display_2;
int display_3;
int display_4;
int decimal_place; //encodes where to place decimal point
// (1, ones number, 2 tens number, 3 hundreds number, 4 thousands number)

void setup() {
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

  input_number = 12.24;
}

void loop() {
  //Get the first 4 digits of input float
  if (input_number > 1000) {      temp = input_number;          decimal_place = 4;
  } else if(input_number > 100) { temp = input_number * 10;     decimal_place = 3;
  } else if(input_number > 10) {  temp = input_number * 100;    decimal_place = 2;
  } else {                        temp = input_number * 1000;   decimal_place = 1; }
  display_1 = temp / 1000;
  display_2 = (temp / 100) % 10;
  display_3 = (temp / 10) % 10;
  display_4 = temp % 10;
 
  
  //1st Digit:
  digitalWrite(digit_1, HIGH);
  digitalWrite(digit_2, LOW);
  digitalWrite(digit_3, LOW);
  digitalWrite(digit_4, LOW);
  if (dsegs[display_1][0] == 1) { digitalWrite(A, LOW); } else { digitalWrite(A, HIGH); }
  if (dsegs[display_1][1] == 1) { digitalWrite(B, LOW); } else { digitalWrite(B, HIGH); }
  if (dsegs[display_1][2] == 1) { digitalWrite(C, LOW); } else { digitalWrite(C, HIGH); }
  if (dsegs[display_1][3] == 1) { digitalWrite(D, LOW); } else { digitalWrite(D, HIGH); }
  if (dsegs[display_1][4] == 1) { digitalWrite(E, LOW); } else { digitalWrite(E, HIGH); }
  if (dsegs[display_1][5] == 1) { digitalWrite(F, LOW); } else { digitalWrite(F, HIGH); }
  if (dsegs[display_1][6] == 1) { digitalWrite(G, LOW); } else { digitalWrite(G, HIGH); }
  if (decimal_place == 1 ) { digitalWrite(DP, LOW); } else { digitalWrite(DP, HIGH); }

  delay(SWITCHING_SPEED);

  digitalWrite(digit_1, LOW);
  digitalWrite(digit_2, HIGH);
  digitalWrite(digit_3, LOW);
  digitalWrite(digit_4, LOW);
  if (dsegs[display_2][0] == 1) { digitalWrite(A, LOW); } else { digitalWrite(A, HIGH); }
  if (dsegs[display_2][1] == 1) { digitalWrite(B, LOW); } else { digitalWrite(B, HIGH); }
  if (dsegs[display_2][2] == 1) { digitalWrite(C, LOW); } else { digitalWrite(C, HIGH); }
  if (dsegs[display_2][3] == 1) { digitalWrite(D, LOW); } else { digitalWrite(D, HIGH); }
  if (dsegs[display_2][4] == 1) { digitalWrite(E, LOW); } else { digitalWrite(E, HIGH); }
  if (dsegs[display_2][5] == 1) { digitalWrite(F, LOW); } else { digitalWrite(F, HIGH); }
  if (dsegs[display_2][6] == 1) { digitalWrite(G, LOW); } else { digitalWrite(G, HIGH); }
  if (decimal_place == 2 ) { digitalWrite(DP, LOW); } else { digitalWrite(DP, HIGH); }

  delay(SWITCHING_SPEED);

  digitalWrite(digit_1, LOW);
  digitalWrite(digit_2, LOW);
  digitalWrite(digit_3, HIGH);
  digitalWrite(digit_4, LOW);
  if (dsegs[display_3][0] == 1) { digitalWrite(A, LOW); } else { digitalWrite(A, HIGH); }
  if (dsegs[display_3][1] == 1) { digitalWrite(B, LOW); } else { digitalWrite(B, HIGH); }
  if (dsegs[display_3][2] == 1) { digitalWrite(C, LOW); } else { digitalWrite(C, HIGH); }
  if (dsegs[display_3][3] == 1) { digitalWrite(D, LOW); } else { digitalWrite(D, HIGH); }
  if (dsegs[display_3][4] == 1) { digitalWrite(E, LOW); } else { digitalWrite(E, HIGH); }
  if (dsegs[display_3][5] == 1) { digitalWrite(F, LOW); } else { digitalWrite(F, HIGH); }
  if (dsegs[display_3][6] == 1) { digitalWrite(G, LOW); } else { digitalWrite(G, HIGH); }
  if (decimal_place == 3 ) { digitalWrite(DP, LOW); } else { digitalWrite(DP, HIGH); }
  
  delay(SWITCHING_SPEED);

  digitalWrite(digit_1, LOW);
  digitalWrite(digit_2, LOW);
  digitalWrite(digit_3, LOW);
  digitalWrite(digit_4, HIGH);
  if (dsegs[display_4][0] == 1) { digitalWrite(A, LOW); } else { digitalWrite(A, HIGH); }
  if (dsegs[display_4][1] == 1) { digitalWrite(B, LOW); } else { digitalWrite(B, HIGH); }
  if (dsegs[display_4][2] == 1) { digitalWrite(C, LOW); } else { digitalWrite(C, HIGH); }
  if (dsegs[display_4][3] == 1) { digitalWrite(D, LOW); } else { digitalWrite(D, HIGH); }
  if (dsegs[display_4][4] == 1) { digitalWrite(E, LOW); } else { digitalWrite(E, HIGH); }
  if (dsegs[display_4][5] == 1) { digitalWrite(F, LOW); } else { digitalWrite(F, HIGH); }
  if (dsegs[display_4][6] == 1) { digitalWrite(G, LOW); } else { digitalWrite(G, HIGH); }
  if (decimal_place == 4 ) { digitalWrite(DP, LOW); } else { digitalWrite(DP, HIGH); }

  delay(SWITCHING_SPEED);
  
  
}
