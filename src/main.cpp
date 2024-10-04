#include <Arduino.h>
#include <Ps3Controller.h>

#define PWMA_PIN  GPIO_NUM_16
#define AIN1_PIN  GPIO_NUM_14
#define AIN2_PIN  GPIO_NUM_12
#define PWMB_PIN  GPIO_NUM_27
#define BIN1_PIN  GPIO_NUM_17
#define BIN2_PIN  GPIO_NUM_13
#define ENA_PIN   A0
#define ENB_PIN   A1

#define motorFreq       15000
#define motorResolution 8
 
#define motorAChannel   3
#define motorBChannel   4

#define FORWARDS   1
#define BACKWARDS  2
#define TURN_RIGHT 3
#define TURN_LEFT  4
#define STOP  0

int motorAPWM = 255;
int motorBPWM = 255;
int wheel = 0;

void setMotorsDirection(int direction) {
  switch (direction)
  {
  case 0:
    digitalWrite(AIN1_PIN, LOW);
    digitalWrite(AIN2_PIN, LOW);
    digitalWrite(BIN1_PIN, LOW);
    digitalWrite(BIN2_PIN, LOW);
    break;
  case 1:
    digitalWrite(AIN1_PIN, LOW);
    digitalWrite(AIN2_PIN, HIGH);
    digitalWrite(BIN1_PIN, LOW);
    digitalWrite(BIN2_PIN, HIGH);
    break;
  case 2:
    digitalWrite(AIN1_PIN, HIGH);
    digitalWrite(AIN2_PIN, LOW);
    digitalWrite(BIN1_PIN, HIGH);
    digitalWrite(BIN2_PIN, LOW);
    break;
  case 3:
    digitalWrite(AIN1_PIN, HIGH);
    digitalWrite(AIN2_PIN, LOW);
    digitalWrite(BIN1_PIN, LOW);
    digitalWrite(BIN2_PIN, HIGH);
    break;
  case 4:
    digitalWrite(AIN1_PIN, LOW);
    digitalWrite(AIN2_PIN, HIGH);
    digitalWrite(BIN1_PIN, HIGH);
    digitalWrite(BIN2_PIN, LOW);
    break;
  default:
    break;
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println("ESP32 PS3 Car");

  pinMode(PWMA_PIN, OUTPUT);
  pinMode(PWMB_PIN, OUTPUT);
  pinMode(AIN1_PIN, OUTPUT);
  pinMode(AIN2_PIN, OUTPUT);
  pinMode(BIN1_PIN, OUTPUT);
  pinMode(BIN2_PIN, OUTPUT);
  pinMode(ENA_PIN, OUTPUT);
  pinMode(ENB_PIN, OUTPUT);
  
  digitalWrite(ENA_PIN, HIGH);
  digitalWrite(ENB_PIN, HIGH);

  ledcSetup(motorAChannel, motorFreq, motorResolution);
  ledcSetup(motorBChannel, motorFreq, motorResolution);

  ledcAttachPin(PWMA_PIN, motorAChannel);
  ledcAttachPin(PWMB_PIN, motorBChannel);
  
  Ps3.begin("00:00:00:00:00:00");
  Serial.println("Started");
}

void loop() {
  if(Ps3.isConnected()){
    motorAPWM = 255;
    motorBPWM = 255;
    wheel = 0;
    if((!Ps3.data.button.cross) && (!Ps3.data.button.square)){
      if(Ps3.data.button.left){
        setMotorsDirection(TURN_LEFT);
      }
      if(Ps3.data.button.right){
        setMotorsDirection(TURN_RIGHT);
      }
      if(Ps3.data.button.down){
        setMotorsDirection(BACKWARDS);
      }
      if(Ps3.data.button.up){
        setMotorsDirection(FORWARDS);
      }
      if ((!Ps3.data.button.left) && (!Ps3.data.button.right) && (!Ps3.data.button.down) && (!Ps3.data.button.up)){
        setMotorsDirection(STOP);
      }
    }else {
      if((Ps3.data.button.cross) && (Ps3.data.button.square)){
        setMotorsDirection(STOP);
      }else {
        wheel = map(Ps3.data.sensor.accelerometer.x, -112, 112, -255, 255);
        if(wheel > 20){
          motorAPWM = 255;
          motorBPWM = 255 - abs(wheel);
        }else if(wheel < (-20)){
          motorAPWM = 255 - abs(wheel);
          motorBPWM = 255;
        }else {
          motorAPWM = 255;
          motorBPWM = 255;
        }
        if(Ps3.data.button.cross){
          setMotorsDirection(FORWARDS);                  
        }
        if(Ps3.data.button.square){
          setMotorsDirection(BACKWARDS);
        }
      }
    }

    ledcWrite(motorAChannel, motorAPWM);
    ledcWrite(motorBChannel, motorBPWM);

    Serial.print(wheel);
    Serial.print(" ");
    Serial.print(motorAPWM);
    Serial.print(" ");
    Serial.println(motorBPWM);
  } else {
    ledcWrite(motorAChannel, 0);
    ledcWrite(motorBChannel, 0);
  }
  delay(10);
}