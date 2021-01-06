#include <Arduino.h>
/* include écran oled */


/******Definition******/

#define CONTENANCE 400 // Le verre peut contenir 400 mL
int prop[4];
int debimeterPIN = 2;
int valve1PIN = 3;
int valve2PIN = 4;
int valve3PIN = 5;
int valve4PIN = 6;
int pompePIN = 7;
volatile float waterFlow;
unsigned long time_now = 0;

/******Prototype******/

bool time_loop(unsigned long start_millis,unsigned int delay);
void pulse();
boolean remplir(float prop[]);
bool debitmetre(float prop);
float* serialEvent();

/******Main code******/

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
  /*code pour les tests de l'arduino*/
  float prop[4] = {0.20,0.3,0.2,0.3};
  remplir(prop);
  
}

void loop() {
  /*code pour tester l'esp*/
  if(Serial.available()){
    float* prop = serialEvent();
    for(int i=0;i<4;i++){
      Serial.println(prop[i]);
    }
  } 

  /* code final */
  // if(Serial.available()){
  //   float* prop = serialEvent();
  //   Serial.println("Remplissage en cours");
  //   remplir(prop);
  // }
  delete[] prop;
}

/******Functions******/

/**
 * @brief remplie un verre en fonction d'une proportion contenance donnée
 * 
 * @param prop  proportion (sur 1) de chaque contenant (ex c[0] = 0.2, c[1] = 0.3, c[2] = 0.5, c[3] = 0)
 * @return true le remplissage a été effectué
 * @return false le remplissage a échoué
 */
boolean remplir(float prop[]){
  int valves[4] = {valve1PIN,valve2PIN,valve3PIN,valve4PIN};

  // Allume la pompe
  digitalWrite(pompePIN,HIGH);
  Serial.println("Ouverture pompe");

  // verse les contenants 1 par 1
  for(int contenant = 0;contenant<4;contenant++){
    waterFlow = 0;
    if(prop[contenant] != 0){
      // Ouvrir valve
      Serial.println("Ouverture vanne");
      digitalWrite(valves[contenant],HIGH);
      // attend...
      if(!debitmetre(prop[contenant])){ // plus de 10 sec passées = le contenant est vide
        Serial.println("Contenant vide :");
        Serial.println(contenant+1);
        return false;
      }
      Serial.println("Debit écoulé");
      // Fermer valve
      digitalWrite(valves[contenant],LOW);
      Serial.println("Fermeture vanne");
    }
    // Attend 1 sec entre chaque versement
    time_loop(millis(),1000);
  }

  // Ferme la pompe
  digitalWrite(pompePIN,LOW);
  Serial.println("Fermeture pompe");

  return true;
}

/**
 * @brief Attend le temps que la quantité de boissons demandé soit écoulé
 * 
 * @param prop float
 * @return true La proportion a bien été versée
 * @return false Le contenant est vide
 */
bool debitmetre(float prop){
  float aVerser = prop * CONTENANCE;
  while(waterFlow < aVerser){
    if(time_loop(millis(),10000)){ // Si au bout de 10 secondes aVerser ne s'est toujours pas écoulé, on considère le contenant vide
      waterFlow = aVerser;
      return false;
    }
  }
  return true;
}

/**
 * @brief Fonction nécessaire pour le débitmètre
 * 
 */
void pulse(){
  waterFlow += 1.0/(5.880); //measure the quantity of square wave
}

/**
 * @brief Fonction qui répcupère les proportions envoyé par l'ESP
 * 
 * @return float* Tableau de proportions
 */
float* serialEvent(){
  float* prop = new float[4];
  int i=0;
  while(Serial.available()){
    prop[i] = Serial.read();
    // Serial.println(prop[i]);
    i++;
  }
  return prop;
}

bool time_loop(unsigned long start_millis,unsigned int delay){
  while(millis() - start_millis < delay){}
  return true;
}