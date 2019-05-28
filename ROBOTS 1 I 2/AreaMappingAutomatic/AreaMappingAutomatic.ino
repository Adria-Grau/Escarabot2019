//Crida a les llibreries necessàries per a realitzar la tasca
#include <NewPing.h> //HC-SR04
#include <Wire.h> //GY-723
#define addr 0x1E //Adreça IC2 pel HMC5883
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
  Wire.begin(); //Inicialització del vector de lectura de configuració
  Wire.beginTransmission(addr); //Inici de la lectura del vector
  Wire.write(0x02); //Creació del registre
  Wire.write(0x00); //Mesurament continuat del registre
  Wire.endTransmission(); //Tancament del valor de lectura de configuració
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
      char controlB = char(bluetooth.read());
      if(controlB == '2')
      {
        start = true;
      }
    }
    //Obtenim distància moviment
    int distance = sonar.ping_cm();
    int angleInici = getAngle();
    //Mentres que el robot no pugui xocar, continuar avançant 
    while(distance > 10)
    {
      move(1, 163, -1); //Moviment motor 1
      move(2, 200, -1); //Moviment motor 2
      distance = sonar.ping_cm();
    }
    //Si detecta un obstacle s'atura i comença l'evitament
    stop(); //S'atura el robot
    delay(250); //S'espera mig segon
    if(sentitGir == 0) //Gir cap a la dreta
    {
      sentitGir = avoidanceRight(bluetooth, sonar);
    }
    else //Gira a l'esquerra
    {
      sentitGir = avoidanceLeft(bluetooth, sonar);
    }
  }
}
