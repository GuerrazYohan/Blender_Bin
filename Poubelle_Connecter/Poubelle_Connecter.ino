/*======== Declaration Variable ======*/
#include "SR04.h"
//Declaration pin data Motor 
#define ENABLE 10
#define DIRA 8
#define DIRB 9

//Declaration pin data UltraSon 1Ultrasonic sensor
#define TRIG_PIN 12
#define ECHO_PIN 11
SR04 sr04 = SR04(ECHO_PIN,TRIG_PIN);
long a;

//Declaration pin data bouton 
#define BUTTOM 2

/* =================== Fonction =================== */

void setup() {
//Set pins as outputs

//---set pin direction
  pinMode(ENABLE,OUTPUT);
  pinMode(DIRA,OUTPUT);
  pinMode(DIRB,OUTPUT);
//---set pin ultrason

//---set pin bouton
  pinMode(BUTTOM,INPUT); 
  
  Serial.begin(9600);
  delay(1000);
}


void loop() {
  
  bool action_Buttom = digitalRead(BUTTOM);
  a=sr04.Distance();
  Serial.print(a);
  Serial.println("cm");
  
  if(action_Buttom == false){
     Serial.println("poubelle ouverte"); 
     digitalWrite(ENABLE,LOW); // enable on
    }
  if(action_Buttom == true){
    Serial.println("poubelle fermer");
      if(a >= 10){
        Serial.println("poubelle vide");
        digitalWrite(ENABLE,HIGH); // enable on
        digitalWrite(DIRA,HIGH); //one way
        digitalWrite(DIRB,LOW); 
        }
      if(a < 10){
        Serial.println("poubelle pleinne");
        digitalWrite(ENABLE,LOW); // enable off
        }
    }
}
