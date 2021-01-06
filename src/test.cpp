#include <Arduino.h>
#define CONTENANCE 100 // Le verre peut contenir 400 mL

double prop[4];
int debimeterPIN = 2;
int valve1PIN = 35;
///int valve2PIN = 4;
//int valve3PIN = 5;
//int valve4PIN = 6;
int pompePIN = 34;
volatile double waterFlow;

void pulse() //measure the quantity of square wave
{
  waterFlow += 1.0/(5.880);
}



void debitmetre(double prop, int contenant)
{
  
  double aVerser = prop * CONTENANCE;
  while(waterFlow < aVerser){
  }
}


/**
 * @brief remplie un verre en fonction d'une proportion contenance donnée
 * 
 * @param prop  proportion (sur 1) de chaque contenant (ex c[0] = 0.2, c[1] = 0.3, c[2] = 0.5, c[3] = 0)
 * @return volumes de chaque contenant versés (en mL dans un tableau d'entier)
 */

bool remplir(double prop[]){
  //int valves[4] = {valve1PIN,valve2PIN,valve3PIN,valve4PIN};

  // Allume la pompe
  digitalWrite(pompePIN,HIGH);
  Serial.println("Ouverture pompe");

  // verse les contenants 1 par 1
  for(int contenant = 0;contenant<4;contenant++){
    waterFlow = 0;

    if(prop[contenant] != 0){
      // Ouvrir valve
      Serial.println("Ouverture vanne");
      //digitalWrite(valves[contenant],HIGH);
      digitalWrite(valve1PIN,HIGH);
      // attend...
      debitmetre(prop[contenant],contenant);
      Serial.println("Debit écoulé");
      // Fermer valve
      //digitalWrite(valves[contenant],LOW);
      digitalWrite(valve1PIN,LOW);
      Serial.println("Fermeture vanne");
      delay(1000);
    }

    // Attend 1 sec pour vider les tuyaux
    // int lastMillis = millis() - 1000;
    // if (millis() - lastMillis > 1000){
    //   lastMillis = millis();
    // }
  }
  // Ferme la pompe
  digitalWrite(pompePIN,LOW);
  Serial.println("Fermeture pompe");

  
  // for(int j=0;j<4;j++){
  //   Serial.println("Contenant %d: %s",j+1,r[j]);
  // }
  return true;

}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(debimeterPIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(debimeterPIN), pulse, RISING);

  pinMode(valve1PIN,OUTPUT);
  //pinMode(valve2PIN,OUTPUT);
  //pinMode(valve3PIN,OUTPUT);
  //pinMode(valve4PIN,OUTPUT);
  pinMode(pompePIN,OUTPUT);
  double prop[4] = {0.20,0.3,0.2,0.3};
  remplir(prop);
}

void loop() 
{
}





