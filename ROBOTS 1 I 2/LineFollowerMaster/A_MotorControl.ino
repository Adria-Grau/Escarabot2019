////////////////////////////////
////   MOVIMENT DEL ROBOT   ////
////////////////////////////////
//Classe encarregada de moure el robot i controlar-ne els seus moviments
//1. Funció per moure el robot indicant quin motor s'engega, la velocitat i el sentit (-1 endavant)
void move(int motor, int velocitat, int direccio)
{
  //Variables control del direcció
  boolean pinMotor1 = LOW;
  boolean pinMotor2 = HIGH;

  //Control de direcció
  if(direccio == 1)
  {
    pinMotor1 = HIGH;
    pinMotor2 = LOW;
  }
  
  //S'activa el motor
  digitalWrite(STBY, HIGH);

  //Control motor
  if(motor == 1) //Cas motor esquerra
  {
    digitalWrite(AIN1, pinMotor1);
    digitalWrite(AIN2, pinMotor2);
    analogWrite(PWMA, velocitat);
  }
  else //Control motor esquerra
  {
    digitalWrite(BIN1, pinMotor1);
    digitalWrite(BIN2, pinMotor2);
    analogWrite(PWMB, velocitat);
  }
}

//2. Funció per parar el robot
void stop()
{
  digitalWrite(STBY, LOW); //Es para el motor
}

//3. Funció per girar el robot en X graus a la dreta
void turnRight(float additionAngle)
{
  float actualAngle = getAngle(); //Obtenim l'angle actual
  float finalAngle = angleAddition(actualAngle, additionAngle, 0); //Es calcula l'angle final
  //Mentre que no s'arribi a l'angle, que continui girant
  while((int(actualAngle) > int(finalAngle) + 3) || (int(actualAngle) < int(finalAngle) - 3)) //Rotació fins arribar al final
  {
    move(1, 70, -1); //Moviment motor 1
    move(2, 70, 1); //Moviment motor 2
    actualAngle = getAngle();
  }
}

//4. Funció per girar el robot X graus a l'esquerra
void turnLeft(float additionAngle)
{
  float actualAngle = getAngle(); //Obtenim l'angle actual;
  float finalAngle = angleAddition(actualAngle, additionAngle, 1); //Es calcula l'angle final
  //Mentre que no s'arribi a l'angle, que continui girant
  while((int(actualAngle) > int(finalAngle) + 3) || (int(actualAngle) < int(finalAngle) - 3)) //Rotació fins arribar al final
  {
    move(1, 70, 1); //Moviment motor 1
    move(2, 70, -1); //Moviment motor 2
    actualAngle = getAngle();
  }
}

//5. Funció encarregada d'obtenir l'angle actual del front del robot (respecte el N magnètic)
int getAngle()
{
  int x,y,z; //Variables amb la que es treballarà
  //Inicialització, lectura i tancament de l'obtenció de les dades
  Wire.beginTransmission(addr);
  Wire.write(0x03);
  Wire.endTransmission();
  //Llegir els valors dels sensors amb els 6 eixos
  Wire.requestFrom(addr, 6);
  if(6 <= Wire.available()){
    x = Wire.read()<<8; //Valor magnètic X 
    x |= Wire.read(); //Valor rotacional  X
    z = Wire.read()<<8; //Valor magnètic Y
    z |= Wire.read(); //Valor rotacional  Y
    y = Wire.read()<<8; //Valor magnètic Z
    y |= Wire.read(); //Valor rotacional  Z
  }
  //Adquisició de l'angle en radians amb la fòrmula del triangle
  float angulo = atan2(y, x);
  //Conversió de l'angle a graus
  angulo = angulo * RAD_TO_DEG;
  //Adequació a la declinació terrestre de Barcelona
  angulo = angulo - declinationBCN;
  //Conversió de l'angle en cas de donar negatiu
  if(angulo < 0)
  {
    angulo = angulo + 360;
  }
  //Retorn de l'angle calculat
  return(int(angulo));
}

//6. Funció que calcula l'angle que ha de girar el robot a partir d'un input
float angleAddition(float initialAngle, float degrees, float direction)
{
  //Variable que es retornarà
  float finalAngle = 0;
  //Si s'indica que vol girar a l'esquerra, afegir graus
  if(direction == 1)
  {
    finalAngle = initialAngle + degrees;
    //Conversió per evitar angles superiors a 360º
    if(finalAngle >= 360)
    {
      finalAngle = finalAngle - 360;
    }
  }
  //En cas contrari, girar a la dreta restant graus
  else
  {
    finalAngle = initialAngle - degrees;
    //Conversió per evitar angles superiors  360º
    if(finalAngle >= 360)
    {
      finalAngle = 360 - finalAngle;
    }
    //Conversió per evitar angles negatius
    else if(finalAngle < 0)
    {
      finalAngle = finalAngle + 360;
    }
  }
  //Retorn de la variable final
  return finalAngle;
}
