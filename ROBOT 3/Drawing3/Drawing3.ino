//Crida a les llibreries necessàries per a realitzar la tasca
#include <Wire.h> //GY-723
#include <MechaQMC5883.h> //QMC5883 (controlador GY-723)
#include<SoftwareSerial.h> //HC-05
#include <Servo.h> //Servo

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
MechaQMC5883 qmc;
//2.HC-05
SoftwareSerial bluetooth(4, 12);
boolean start = false;
//3.Servo
Servo servo;
int pos = 0; //Posició del servo

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
  //5. Servo
  servo.attach(13);
}

void loop() {
  //Es coloca el servo en posició de no-escritura
  int servoAngle = 45;
  servo.write(servoAngle);
  delay(1500);
  //Es detecta l'ordre enviada per l'ordinador
  if (bluetooth.available())
  {
    //Es filtra l'ordre amb que es treballarà
    char ordre = char(bluetooth.read());
    //Cas gir esquerra
    if(ordre == '1')
    {
      move(1, 50, 1); //Moviment motor 1
      move(2, 50, -1); //Moviment motor 2
    }
    //Cas gir dreta
    else if(ordre == '2')
    {
      move(1, 200, 1); //Moviment motor 1
      move(2, 200, -1); //Moviment motor 2
    }
    //Cas baixar servo
    if(ordre == '3')
    {
      stop(); //S'atura el robot
      delay(250); //S'espera 1/4 segon
      //Es coloca el servo en posició de si-escritura
      servoAngle = 90;
      servo.write(servoAngle);
      delay(1500);
    }
    //Cas moure's endavant
    else if(ordre == '4')
    {
      move(1, 200, -1); //Moviment motor 1
      move(2, 200, -1); //Moviment motor 2
    }
    //Cas stop
    else if(ordre == '5')
    {
      stop();
      //Es coloca el servo en posició de no-escritura
      servoAngle = 45;
      servo.write(servoAngle);
      delay(1500);
    }
  }
}
