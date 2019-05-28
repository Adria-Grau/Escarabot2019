//Crida a les llibreries necessàries per a realitzar la tasca
#include <NewPing.h> //HC-SR04
#include <Wire.h> //GY-723
#include <MechaQMC5883.h> //QMC5883 (controlador GY-723)
#include<SoftwareSerial.h>

//Definició dels pins a utilitzar
//1. Pins dels motors
#define AIN1 10
#define AIN2 11
#define BIN1 8
#define BIN2 7
#define PWMA 6
#define PWMB 5
#define STBY 9
//2. Pins del sensor d'ultrasó HC-RS04
#define ECHO 3
#define TRIGGER 2

//Inicialització dels objectes, variables i sensors
//1.HC-SR04
#define MAX_DISTANCE 200
NewPing sonar(TRIGGER, ECHO, MAX_DISTANCE);
//2.GY-723
const float declinationBCN = 0.87; //Declinació terrestre a l'altura de Barcelona
MechaQMC5883 qmc;
//3.HC-05
SoftwareSerial bluetooth(4, 12);
boolean start = false;
//4. Variables 
int sentitGir = 0; //Últim gir marcat a l'esquerra

//Funció setup que inicialitzarà les peces necessàries
void setup() {
  //1. Motors
  pinMode(STBY, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  //2. GY-723
  Wire.begin();
  qmc.init();
  //3. Bluetooth
  bluetooth.begin(9600);
  //4. Output
  Serial.begin(9600);
}

void loop() {
  //Esperem a rebre la senyal d'inici de l'ordinador central
  while(start == false)
  {
    if (bluetooth.available())
    { 
      char controlA = char(bluetooth.read());
      if(controlA == '1')
      {
        start = true;
      }
    }
  }
  //Actualització de la variable d'engegar el software i esperar a acabar anàlisi
  start = false;
  while(start == false)
  {
    if (bluetooth.available())
    {
      char control = char(bluetooth.read());
      if(control == '2')
      {
        start == true;
      }
      //Obtenim distància moviment
      int distance = sonar.ping_cm();
      //Si la distància és 5, indicar al'ordinador el punt
      if(distance == 5)
      {
        bluetooth.println("1");
      }
      //Cas gir esquerra lleuger
      else if(control == 'q')
      {
        turnLeft(5);
        delay(10); 
        stop();
      }
      //Cas moure's endavant
      else if(control == 'w' && distance > 5)
      {
        move(1, 250, -1); //Moviment motor 1
        move(2, 250, -1); //Moviment motor 2
        delay(10); 
        stop();
      }
      //Cas gir dreta lleuger
      else if(control == 'e')
      {
        turnRight(5);
        delay(250); 
        stop();
      }
      //Cas gir esq 90º
      else if(control == 'a')
      {
        turnLeft(90);
        delay(250); 
        stop();
      }
      //Cas moure's endarrere
      else if(control == 's')
      {
        move(1, 250, 1); //Moviment motor 1
        move(2, 250, 1); //Moviment motor 2
        delay(10); 
        stop();
      }
      //Cas gir dre 90º
      else if(control == 'd')
      {
        turnRight(90);
        delay(250); 
        stop();
      }
    }
  }
}
