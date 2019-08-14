#include <ECE3.h>

#define LEDR RED_LED
#define LEDG GREEN_LED
#define LEDB BLUE_LED

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

const float K_p= 21.5;
const float K_d = 11.5;
const int basePow=100;
float d=0;
float prevSum=0;
float weightedSum=0;

int counter;
int ref[8];
int lastSeen[2];
int leftCount;
int rightCount;
int pulseCount;
int donutFlag;

void setup() {
  // put your setup code here, to run once:
  ECE3_Init();
  //Serial.begin(9600);
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  
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

    digitalWrite(left_nslp_pin, HIGH); 
    digitalWrite(right_nslp_pin, HIGH); 

  donutFlag = 0;
  counter=0;
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
  
  delayMicroseconds(50);

  pinMode(65, INPUT);
  pinMode(48, INPUT);
  pinMode(64, INPUT);
  pinMode(47, INPUT);
  pinMode(52, INPUT);
  pinMode(68, INPUT);
  pinMode(53, INPUT);
  pinMode(69, INPUT);

  delayMicroseconds(850);

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
  if(pulseCount > 20000)
  {
    digitalWrite(back_right_LED, HIGH);
  }
  else if (pulseCount > 19000)
  {
    digitalWrite(back_left_LED, HIGH);
  }
  else if(pulseCount > 18000)
  {
    digitalWrite(front_right_LED, HIGH);
  }
  else if(pulseCount > 9000)
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
  //Serial.println(weightedSum);

  if( ((ref[0] + ref[1] + ref[2] + ref[3] + ref[4] + ref[5] + ref[6] + ref[7]) >= 7 ) && (pulseCount > 3000))
  {
    if(donutFlag == 1)
    {
      digitalWrite(left_nslp_pin, LOW); 
      digitalWrite(right_nslp_pin, LOW); 
    }
    else
    {
      //Serial.print("U turn");
      //Serial.print('\n');
      digitalWrite(left_dir_pin, LOW);
      digitalWrite(right_dir_pin, HIGH);
      digitalWrite(left_nslp_pin, HIGH); 
      digitalWrite(right_nslp_pin, HIGH); 
      analogWrite(left_pwm_pin, 128);
      analogWrite(right_pwm_pin, 128);
      donutFlag = 1;
      delay(450);
      digitalWrite(left_dir_pin, LOW);
      digitalWrite(right_dir_pin, LOW);
      analogWrite(left_pwm_pin, basePow);
      analogWrite(right_pwm_pin, 1.065*basePow);
      delay(300);
    }
  }

  else if( (ref[0] + ref[1] + ref[2] + ref[3] + ref[4] + ref[5] + ref[6] + ref[7]) == 0 )
  {
    if(lastSeen[0] == 1 && lastSeen[1] == 0)
    {
      weightedSum=4.5;
    }
    else if(lastSeen[1] == 1 && lastSeen[0] == 0)
    {
      weightedSum=-4.5;
    }
  }
  
  else{
    digitalWrite(left_dir_pin, LOW);
    digitalWrite(right_dir_pin, LOW);
    analogWrite(right_pwm_pin, 1.065*(basePow - weightedSum*K_p + d*K_d));
    analogWrite(left_pwm_pin, basePow + weightedSum*K_p - d*K_d);
    if(counter%100 == 0)
    {
      d=prevSum-weightedSum;
      prevSum=weightedSum;
    }
    if((ref[0] + ref[1] + ref[2] + ref[3] + ref[4] + ref[5] + ref[6] + ref[7]) == 1)
    {
      lastSeen[0] = ref[0];
      lastSeen[1] = ref[7];
    }
    /*
    if((basePow + weightedSum*K_p - d*K_d) > (basePow - weightedSum*K_p + d*K_d))
    {
      digitalWrite(LEDB, LOW);
      digitalWrite(LEDG, LOW);
      digitalWrite(LEDR, HIGH);
    }
    else if(basePow - weightedSum*K_p + d*K_d > basePow + weightedSum*K_p - d*K_d)
    {
      digitalWrite(LEDB, HIGH);
      digitalWrite(LEDG, LOW);
      digitalWrite(LEDR, LOW);
    }
    else
    {
      digitalWrite(LEDB, LOW);
      digitalWrite(LEDG, HIGH);
      digitalWrite(LEDR, LOW);
    }
    */
  }
  counter++;
}

void button(){
  resetEncoderCount_left();
  resetEncoderCount_right();
}
