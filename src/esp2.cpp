#include <Arduino.h>
#include <SPI.h>

#define LED1 19
#define LED2 21
#define LED3 22
#define LED4 23
#define SENSO1 25 // Viene de ESP1
#define SENSO2 26 // Viene de ESP1
#define SENSO3 27 // Viene de ESP1
#define SENSEXIT 33 //CAMBIAR CON PIN REAL //Sensor propio para salida
#define ENTRADA 99 //CAMBIAR ESTO CON EL PIN REAL // Esto Viene de ESP1, acá se attach el interrupt
#define SERVOIN 99 //CAMBIAR ESTO CON EL PIN REAL
#define SERVOOUT 99 //CAMBIAR ESTO CON EL PIN REAL

volatile bool OpenToExit = false;
volatile bool OpenToEnter = false;

void actualizarLEDS();
void openEntrance();
void openExit();

volatile unsigned long lastInterrupt = 0;

void IRAM_ATTR handleExit(){
    unsigned long now = millis();
    if (now - lastInterrupt > 200) {
        OpenToExit = true;
        lastInterrupt = now;
    }
}

void IRAM_ATTR handleEntrance(){
    OpenToEnter=true;
}

void setup() {
  Serial.begin(115200);


  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(SERVOIN, OUTPUT);
  pinMode(SERVOOUT, OUTPUT);

  
  pinMode(SENSO1, INPUT);
  pinMode(SENSO2, INPUT);
  pinMode(SENSO3, INPUT);
  pinMode(SENSEXIT, INPUT);
  pinMode(ENTRADA, INPUT);


  attachInterrupt(digitalPinToInterrupt(SENSEXIT),handleExit,FALLING);
  attachInterrupt(digitalPinToInterrupt(ENTRADA),handleEntrance,RISING);

}   

void loop() {

    actualizarLEDS();

    if (OpenToExit){
        OpenToExit=false;
        openExit();}
    if (OpenToEnter){
        OpenToEnter=false;
        openEntrance();}
}

void actualizarLEDS(){
    digitalWrite(LED1, digitalRead(SENSO1));
    digitalWrite(LED2, digitalRead(SENSO2));
    digitalWrite(LED3, digitalRead(SENSO3));
}

void openEntrance(){
    // Servo 180
}

void openExit(){
    // servo 360
}