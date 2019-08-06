#include <ECE3.h>

const int left_nslp_pin = 31;
const int left_dir_pin = 29;
const int left_pwm_pin = 40;
const int right_nslp_pin = 11;
const int right_dir_pin = 30;
const int right_pwm_pin = 39;
const int even_LED = 61;
const int odd_LED = 45;
const int front_right_LED = 51;
const int front_left_LED = 41;
const int back_left_LED = 57;
const int back_right_LED = 58;

const float K_p= 2.1;
const float K_d = 2.1;
const int basePow=64;
float d=0;
float prevSum=0;
float weightedSum=0;


int ref[8];
int leftCount;
int rightCount;
int pulseCount;
int donutFlag;

void setup() {
  // put your setup code here, to run once:
  ECE3_Init();
  Serial.begin(9600);
  pinMode(even_LED, OUTPUT);
  pinMode(odd_LED, OUTPUT);
  pinMode(front_right_LED, OUTPUT);
  pinMode(front_left_LED, OUTPUT);
  pinMode(back_left_LED, OUTPUT);
  pinMode(back_right_LED, OUTPUT);
  digitalWrite(even_LED, HIGH);
  digitalWrite(odd_LED, HIGH);
  digitalWrite(front_right_LED, LOW);
  digitalWrite(front_left_LED, LOW);
  digitalWrite(back_left_LED, LOW);
  digitalWrite(back_right_LED, LOW);

  pinMode(left_nslp_pin, OUTPUT);
  pinMode(left_dir_pin, OUTPUT);
  pinMode(left_pwm_pin, OUTPUT);
  pinMode(right_nslp_pin, OUTPUT);
  pinMode(right_dir_pin, OUTPUT);
  pinMode(right_pwm_pin, OUTPUT);

  pinMode(PUSH2, INPUT_PULLUP);
  attachInterrupt(PUSH2, button, FALLING); 

  donutFlag = 0;
}

void loop() {
  // put your main code here, to run repeatedly: 

  pinMode(65, OUTPUT);
  pinMode(48, OUTPUT);
  pinMode(64, OUTPUT);
  pinMode(47, OUTPUT);
  pinMode(52, OUTPUT);
  pinMode(68, OUTPUT);
  pinMode(53, OUTPUT);
  pinMode(69, OUTPUT);

  digitalWrite(65, HIGH);
  digitalWrite(48, HIGH);
  digitalWrite(64, HIGH);
  digitalWrite(47, HIGH);
  digitalWrite(52, HIGH);
  digitalWrite(68, HIGH);
  digitalWrite(53, HIGH);
  digitalWrite(69, HIGH);
  
  delayMicroseconds(10000);

  pinMode(65, INPUT);
  pinMode(48, INPUT);
  pinMode(64, INPUT);
  pinMode(47, INPUT);
  pinMode(52, INPUT);
  pinMode(68, INPUT);
  pinMode(53, INPUT);
  pinMode(69, INPUT);

  delayMicroseconds(600);

  ref[0] = digitalRead(65);
  ref[1] = digitalRead(48);
  ref[2] = digitalRead(64);
  ref[3] = digitalRead(47);
  ref[4] = digitalRead(52);
  ref[5] = digitalRead(68);
  ref[6] = digitalRead(53);
  ref[7] = digitalRead(69);

  leftCount = getEncoderCount_left();
  rightCount = getEncoderCount_right();
  pulseCount = (leftCount+rightCount)/2;
  if(pulseCount > 15000)
  {
    digitalWrite(back_right_LED, HIGH);
  }
  else if (pulseCount > 14500)
  {
    digitalWrite(back_left_LED, HIGH);
  }
  else if(pulseCount > 14000)
  {
    digitalWrite(front_right_LED, HIGH);
  }
  else if(pulseCount > 8000)
  {
    digitalWrite(front_left_LED, HIGH);
  }
  
/*
  for (unsigned char i = 0; i < 8; i++) //0 is right side, 7 is left
  {
    Serial.print(ref[i]);
    Serial.print('\t'); // tab to format the raw data into columns in the Serial monitor
  }
  Serial.print('\n');
  */
weightedSum=ref[0]*1.75+ref[1]*1.25+ref[2]*0.75+ref[3]*0.25-ref[4]*0.25-ref[5]*0.75-ref[6]*1.25-ref[7]*1.75;

  if( ((ref[0] + ref[1] + ref[2] + ref[3] + ref[4] + ref[5] + ref[6] + ref[7]) <= 1 ) && (pulseCount < 14000) && (pulseCount > 6700) && (donutFlag == 0))
  {
    //Serial.print("U turn");
    //Serial.print('\n');
    digitalWrite(left_dir_pin, LOW);
    digitalWrite(right_dir_pin, HIGH);
    digitalWrite(left_nslp_pin, HIGH); 
    digitalWrite(right_nslp_pin, HIGH); 
    analogWrite(left_pwm_pin, 64);
    analogWrite(right_pwm_pin, 64);
    donutFlag = 1;
    delay(1100);
    digitalWrite(left_dir_pin, LOW);
    digitalWrite(right_dir_pin, LOW);
    delay(500);
  }
  
   analogWrite(right_pwm_pin, basePow+weightedSum*K_p-d*K_d);
  analogWrite(left_pwm_pin, basePow-weightedSum*K_p+d*K_d);
  d=prevSum-weightedSum;
  prevSum=weightedSum;
}

void button(){
  resetEncoderCount_left();
  resetEncoderCount_right();
}
