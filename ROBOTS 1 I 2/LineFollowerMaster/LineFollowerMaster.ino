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
//3. Sensor TCRT5000
#define sensorL 16
#define sensorR 17

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
  //5. Pins sensors
  pinMode(sensorL, INPUT);
  pinMode(sensorR, INPUT);
}

void loop() {
  //Cas de que el no es detecti cap línia, el robot avança
  if((digitalRead(sensorL) == true) && ((digitalRead(sensorR) == true)))
  {
    move(1, 200, 1); //Moviment motor 1
    move(2, 200, 1); //Moviment motor 2
    bluetooth.println("1");
  }
  //Cas de que el robot detecta línia a la dreta, gira esquerra
  else if((digitalRead(sensorL) == true) && ((digitalRead(sensorR) == false)))
  {
    move(1, 200, 1); //Moviment motor 1
    move(2, 0, 1); //Moviment motor 2
    bluetooth.println("2");
  }
  //Cas de que el robot detecta línia a l'esquerra, gira dreta
  else if((digitalRead(sensorL) == false) && ((digitalRead(sensorR) == true)))
  {
    move(1, 0, 1); //Moviment motor 1
    move(2, 200, 1); //Moviment motor 2
    bluetooth.println("3");
  }
  //Cas de que el robot detecta línia als dos sensors i es para
  else if((digitalRead(sensorL) == false) && ((digitalRead(sensorR) == false)))
  {
    stop();
    bluetooth.println("4");
  }
}
