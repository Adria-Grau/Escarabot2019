//Crida a les llibreries necessàries per a realitzar la tasca
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

//Inicialització dels objectes, variables i sensors
//1.GY-723
const float declinationBCN = 0.87; //Declinació terrestre a l'altura de Barcelona
//2.HC-05
SoftwareSerial bluetooth(4, 12);
boolean start = false;

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
  //Es detecta l'ordre enviada per el master
  if (bluetooth.available())
  { Serial.println("OK");
    //Es filtra l'ordre amb que es treballarà
    char ordre = char(bluetooth.read());
    Serial.println(ordre);
    //Cas recte
    if(ordre == '1')
    {
      move(1, 85, -1); //Moviment motor 1
      move(2, 85, -1); //Moviment motor 2
    }
    //Cas gir esquerra
    else if(ordre == '2')
    {
    move(1, 45, -1); //Moviment motor 1
    move(2, 45, 1); //Moviment motor 
    }
    //Cas gir dreta
    else if(ordre == '3')
    {
    move(1, 45, 1); //Moviment motor 1
    move(2, 45, -1); //Moviment motor 
    }
    //Cas stop
    else if(ordre == '4')
    {
      stop();
    }
  }
}
