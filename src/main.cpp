#include <Arduino.h>
#include <string.h>
/* include écran oled */
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_I2CDevice.h>



/******Definition******/

int xx=40;
int yy=-7;
#define CONTENANCE 30; // Le verre peut contenir 100 mL

float prop[4];
int debimeterPIN = 2;
int valve1PIN = 30;
int valve2PIN = 28;
int valve3PIN = 26;
int valve4PIN = 24;
int pompePIN = 22;
volatile float waterFlow = 0;
unsigned long time_now = 0;
String inString;
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

// Definition for the oled Displayer

/******Prototype******/

bool time_loop(unsigned long start_millis,unsigned int delay);
void pulse();
boolean remplir(float prop[]);
bool debitmetre(float prop);
void serialEvent();
void decodage(String str);
void Commande_Preparation(int);
void Commande_Servie(void);
void Transition(void);

/******Main code******/

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial3.begin(115200);
  pinMode(debimeterPIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(debimeterPIN), pulse, RISING);

  display.begin(SSD1306_SWITCHCAPVCC,0x3C);
  delay(1000);
  Serial.println("OLED Starts");
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  pinMode(valve1PIN,OUTPUT);
  pinMode(valve2PIN,OUTPUT);
  pinMode(valve3PIN,OUTPUT);
  pinMode(valve4PIN,OUTPUT);
  pinMode(pompePIN,OUTPUT);
  /*code pour les tests de l'arduino*/
  // float prop[4] = {0.25,0.25,0.25,0.25};
  // remplir(prop);

  }

void loop() {

  
  if(Serial3.available() > 0){
    String inString;
    char c;
    while(Serial3.available() > 0){      
       c = Serial3.read();
      inString += c;
      Serial.println(c);
    }
    Serial.println(inString);
    if(inString[0] == 'A' || inString[0] == 'B' || inString[0] == 'C' || inString[0] == 'D'){
      decodage(inString);
      Serial.println(prop[0]);
      Serial.println(prop[1]);
      Serial.println(prop[2]);
      Serial.println(prop[3]);
      float test[4] = {prop[0],prop[1],prop[2],prop[3]};
      remplir(test);
    }
  }
  inString = "";
  delay(1000);
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
  // Commande_Preparation(1);
  int valves[4] = {valve1PIN,valve2PIN,valve3PIN,valve4PIN};

  // Allume la pompe
  digitalWrite(pompePIN,HIGH);
  Serial.println("Ouverture pompe");

  // verse les contenants 1 par 1
  for(int contenant = 0;contenant<4;contenant++){
    
    waterFlow = 0;
    if(prop[contenant] != 0){
      Commande_Preparation(contenant+2);
      // Ouvrir valve
      Serial.println("Ouverture vanne");
      digitalWrite(valves[contenant],HIGH);
      // attend...
      debitmetre(prop[contenant]); // plus de 10 sec passées = le contenant est vide
      
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
  Transition();
  Commande_Servie();
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
  float aVerser = prop/2.2 * CONTENANCE;
  waterFlow = 0;
  while(waterFlow < aVerser){Serial.println();}
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
 * @brief Fonction qui répcupère les informations envoyées par l'ESP
 * 
 * @return String Message envoyé par l'esp
 */
void serialEvent(){
  if(Serial3.available() > 0){
  while (Serial3.available() > 0) {    
    char inChar = Serial3.read();
    inString += inChar;
    }
    Serial.println(inString);
    if(inString[0]=='A' || inString[0]=='B' || inString[0]=='C' || inString[0]=='D'){
      decodage(inString);
    }
  }
 
}

bool time_loop(unsigned long start_millis,unsigned int delay){
  while(millis() - start_millis < delay){}
  return true;
}


/**
 * @brief Decode la chaîne de caractère en un tableau de 4 proportions
 * 
 * @param str Chaîne à décoder
 * @return float* Tableau de 4 float
 */
void decodage(String str){
  for(int i = 0; i < 10; i=i+3){
    switch (str[i]){
    case 'A':
      prop[0] = str[i+1] - '0';
      prop[0] = (prop[0]*10 + (str[i+2]-'0'))/100;
      break;
    case 'B':
      prop[1] = str[i+1] - '0';
      prop[1] = (prop[1]*10 + (str[i+2]-'0'))/100;
      break;
    case 'C':
      prop[2] = str[i+1] - '0';
      prop[2] = (prop[2]*10 + (str[i+2]-'0'))/100;
      break;
    case 'D':
      prop[3] = str[i+1] - '0';
      prop[3] = (prop[3]*10 + (str[i+2]-'0'))/100;
      break;
    default:
      break;
    }
  }
  // prop[0] = str[1] - '0';
  // prop[0] = (prop[0]*10 + (str[2]-'0'))/100;
  // prop[1] = str[4]-'0';
  // prop[1] = (prop[1]*10 + (str[5]-'0'))/100;
  // prop[2] = str[7]-'0';
  // prop[2] = (prop[2]*10 + (str[8]-'0'))/100;
  // prop[3] = str[10]-'0';
  // prop[3] = (prop[3]*10 + (str[11]-'0'))/100;
}

//---------------FONCTION SCREEN----------------------

void Commande_Servie(void) {
  display.clearDisplay();

  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("Commande servie !"));
  display.display();      // Show initial text
  delay(100);

  // Scroll in various directions, pausing in-between:
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
  delay(1000);
}

void Transition(void) {
  display.clearDisplay();

  for(int i=0; i<max(display.width(),display.height())/2; i+=2) {
    display.drawCircle(display.width()/2, display.height()/2, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }

  delay(2000);

    //display.clearDisplay();

  for(int i=max(display.width(),display.height())/2; i>0; i-=2) {
    display.drawCircle(display.width()/2, display.height()/2, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }

  delay(2000);
}
void testdrawline() {
  int16_t i;

  display.clearDisplay(); // Clear display buffer

  for(i=0; i<display.width(); i+=4) {
    display.drawLine(0, 0, i, display.height()-1, SSD1306_WHITE);
    display.display(); // Update screen with each newly-drawn line
    delay(1);
  }
  for(i=0; i<display.height(); i+=4) {
    display.drawLine(0, 0, display.width()-1, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  delay(250);
  display.clearDisplay();
}

void Commande_Preparation(int step)
{
display.clearDisplay();

display.setTextSize(1); // Draw 2X-scale text
display.setTextColor(SSD1306_WHITE);
display.setCursor(10, 0);
display.println(F("En preparation..."));
display.display();
  
switch(step)
{
  case 1 :
    display.drawBitmap(xx, yy,frame0,48,48, 1);
    display.display();
    break;
  case 2 :
    display.drawBitmap(xx, yy,frame2,48,48, 1);
    display.display();
    break;
  case 3 :
    display.drawBitmap(xx, yy,frame3,48,48, 1);
    display.display();
    break;
  case 4 :
    display.drawBitmap(xx, yy,frame4,48,48, 1);
    display.display();
    break;
  case 5 :
    display.drawBitmap(xx, yy,frame5,48,48, 1);
    display.display();
    break;
}

}