#include <Arduino.h>
#define CONTENANCE 400 // Le verre peut contenir 400 mL

int prop[4];
int debimeterPIN = 2;
int valve1PIN = 3;
int valve2PIN = 4;
int valve3PIN = 5;
int valve4PIN = 6;
int pompePIN = 7;
volatile double waterFlow;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(debimeterPIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(debimeterPIN), pulse, RISING);
  pinMode(valve1PIN,OUTPUT);
  pinMode(valve2PIN,OUTPUT);
  pinMode(valve3PIN,OUTPUT);
  pinMode(valve4PIN,OUTPUT);
  pinMode(pompePIN,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
}


/**
 * @brief remplie un verre en fonction d'une proportion contenance donnée
 * 
 * @param prop  proportion (sur 100) de chaque contenant (ex c[0] = 20, c[1] = 30, c[3] = 50, c[4] = 0)
 * @return volumes de chaque contenant versés (en mL dans un tableau d'entier)
 */
int* remplir(int prop[]){
  int valves[4] = {valve1PIN,valve2PIN,valve3PIN,valve4PIN};

  // verse les contenants 1 par 1
  for(int contenant = 0;contenant<4;contenant++){
    // Allume la pompe
      digitalWrite(pompePIN,1);
    if(prop[contenant] != 0){
      // Ouvrir valve
      digitalWrite(valves[contenant],1);
      // attend...
      debitmetre(prop[contenant],contenant);
      // Fermer valve
      digitalWrite(valves[contenant],0);
      // Attend 1 sec pour vider les tuyaux
      delay(1000);
    }
    // Ferme la pompe
    digitalWrite(pompePIN,0);
  }

  int r[4] = {prop[0]*CONTENANCE,prop[1]*CONTENANCE,prop[2]*CONTENANCE,prop[3]*CONTENANCE};
  return r;

}

void debitmetre(int prop, int contenant){
  int aVerser = prop * CONTENANCE;
  while(waterFlow < aVerser){
    delay(10);
  }
}

void pulse() //measure the quantity of square wave
{
  waterFlow += 1.0;
}