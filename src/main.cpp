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
 
int motorAPWM = 255;
int motorBPWM = 255;
int wheel = 0;

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
        digitalWrite(AIN1_PIN, LOW);
        digitalWrite(AIN2_PIN, HIGH);
        digitalWrite(BIN1_PIN, HIGH);
        digitalWrite(BIN2_PIN, LOW);
      }
      if(Ps3.data.button.right){
        digitalWrite(AIN1_PIN, HIGH);
        digitalWrite(AIN2_PIN, LOW);
        digitalWrite(BIN1_PIN, LOW);
        digitalWrite(BIN2_PIN, HIGH);
      }
      if(Ps3.data.button.down){
        digitalWrite(AIN1_PIN, HIGH);
        digitalWrite(AIN2_PIN, LOW);
        digitalWrite(BIN1_PIN, HIGH);
        digitalWrite(BIN2_PIN, LOW);
      }
      if(Ps3.data.button.up){
        digitalWrite(AIN1_PIN, LOW);
        digitalWrite(AIN2_PIN, HIGH);
        digitalWrite(BIN1_PIN, LOW);
        digitalWrite(BIN2_PIN, HIGH);
      }
      if ((!Ps3.data.button.left) && (!Ps3.data.button.right) && (!Ps3.data.button.down) && (!Ps3.data.button.up)){
        digitalWrite(AIN1_PIN, LOW);
        digitalWrite(AIN2_PIN, LOW);
        digitalWrite(BIN1_PIN, LOW);
        digitalWrite(BIN2_PIN, LOW);
      }
    }else {
      if((Ps3.data.button.cross) && (Ps3.data.button.square)){
        digitalWrite(AIN1_PIN, LOW);
        digitalWrite(AIN2_PIN, LOW);
        digitalWrite(BIN1_PIN, LOW);
        digitalWrite(BIN2_PIN, LOW);
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
          digitalWrite(AIN1_PIN, LOW);
          digitalWrite(AIN2_PIN, HIGH);
          digitalWrite(BIN1_PIN, LOW);
          digitalWrite(BIN2_PIN, HIGH);                        
        }
        if(Ps3.data.button.square){
          digitalWrite(AIN1_PIN, HIGH);
          digitalWrite(AIN2_PIN, LOW);
          digitalWrite(BIN1_PIN, HIGH);
          digitalWrite(BIN2_PIN, LOW);
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
  }
  delay(10);
}