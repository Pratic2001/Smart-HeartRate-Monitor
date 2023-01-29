/*

 * VARIABLES

 * count: variable to hold count of rr peaks detected in 10 seconds

 * flag: variable that prevents multiple rr peak detections in a single heatbeat

 * hr: HeartRate (initialised to 72)

 * hrv: Heart Rate variability (takes 10-15 seconds to stabilise)

 * instance1: instance when heart beat first time

 * interval: interval between second beat and first beat

 * timer: variable to hold the time after which hr is calculated

 * value: raw sensor value of output pin

 */

#include <SoftwareSerial.h>
#include "LowPower.h"

long instance1 = 0, timer;
double hrv = 1, hr = 72, interval = 0;
int value = 0, count = 0;  

bool flag = 0;

#define shutdown_pin 12
#define cmd_enable_pin 6 
#define threshold 160 // to identify R peak
#define timer_value 10000 // 10 seconds timer to calculate hr

SoftwareSerial Serial1(4 , 5); //(Rx , Tx)


void on_Event(){
  String rdy = "";
  String done = "";
  
  digitalWrite(cmd_enable_pin , HIGH);
  delay(10);
  digitalWrite(cmd_enable_pin , LOW);

  delay(10000);

  Serial1.println("_");

  delay(20000);
  
  for(int i = 0; i < 10; i++)
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
}

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  pinMode(cmd_enable_pin , OUTPUT);
  pinMode(8, INPUT); // Setup for leads off detection LO +
  pinMode(9, INPUT); // Setup for leads off detection LO -
  pinMode(10 , OUTPUT);

  digitalWrite(cmd_enable_pin , LOW);

}

void loop() {

  digitalWrite(cmd_enable_pin , LOW);
  digitalWrite(shutdown_pin , HIGH);
  
  if((digitalRead(8) == 1)||(digitalRead(9) == 1)){
    Serial.println("leads off!");
    digitalWrite(10 , LOW);
    digitalWrite(shutdown_pin, LOW); //standby mode
    instance1 = micros();
    timer = millis();
    Serial.println("Power Down");
    for(int i = 0; i < 3; i++)
      LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }

  else {
   
    digitalWrite(10 , HIGH);
    digitalWrite(shutdown_pin, HIGH); //normal mode
 
    value = analogRead(A0);
    value = map(value, 250, 400, 0, 100); //to flatten the ecg values a bit
    if((value > threshold) && (!flag)) {
      count++;  
      flag = 1;
      interval = micros() - instance1; //RR interval
      instance1 = micros(); 
    }

    else if((value < threshold)) {
      flag = 0;
    }

    if ((millis() - timer) > 10000) {
      hr = count*6;
      timer = millis();
      count = 0; 
    }

    hrv = hr/60 - interval/1000000;
    //if(hr >= 70 && hrv <= 0.9)
      on_Event();
 
    Serial.print(hr);
    Serial.print(",");
    Serial.print(hrv);
    Serial.print(",");
    Serial.println(value);

  }

}
