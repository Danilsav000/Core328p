//#include <core.h>
#include <Arduino.h>
#include <ServoSmooth.h>
#include <wire.h>
#include <GyverOLED.h>

#define DEBUG  //Раскоментировать чтобы включить отладку по уарт

#ifdef DEBUG
#define PRINTS(x) {Serial.print(F(x)); }
#define PRINT(s,v)  { Serial.print(F(s)); Serial.print(v); }
#else 
#define PRINTS(x)
#endif

#define AMOUNT 5 // указываем колличество приводов используемых в проекте
#define BPIN A7
#define impulsMin  200   //600
#define impulsMax 2800  //2600


#define servosSpeed 50
#define servosAccel 0.4


#define homePosition 0
#define maxDeg 180

ServoSmooth servos[AMOUNT];
GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> oled;




uint8_t AB;


boolean isHomeClick = false;
boolean isSetPositionClick;

uint8_t GetAB();

void setup() {
  Serial.begin(9600);
  PRINTS("version 1.0, arduino nano");
  PRINT("\nServos connected: ", AMOUNT);
  PRINT("\nSet speed: ", servosSpeed);
  PRINT("\nSet accel: ", servosAccel);
  PRINTS("\nНачнем! Нажмите кнопку 'Парковка'");
  

  servos[0].attach(11);
  servos[1].attach(10);
  servos[2].attach(9);
  servos[3].attach(6);
  servos[4].attach(5);


  
  
  //servos[0].attach(11, impulsMin, impulsMax);
  //servos[1].attach(10, impulsMin, impulsMax);
  //servos[2].attach(9, impulsMin, impulsMax);
  //servos[3].attach(6, impulsMin, impulsMax);
  //servos[4].attach(5, impulsMin, impulsMax);

  for (int i = 0; i < AMOUNT; i++)
  {
    servos[i].setAccel(servosAccel);
    servos[i].setSpeed(servosSpeed);
    servos[i].smoothStart();
  }
  

}

void loop() {
  

  servos[0].tick();
  servos[1].tick();
  servos[2].tick();
  servos[3].tick();
  servos[4].tick();


AB = GetAB();



  if (isHomeClick == true) {
  int pos0 = map(analogRead(A0),0,1023,0,180);
  int pos1 = map(analogRead(A1),0,1023,0,180);
  int pos2 = map(analogRead(A2),0,1023,0,180);
  int pos3 = map(analogRead(A3),0,1023,0,180);
  int pos4 = map(analogRead(A6),0,1023,0,180);
    servos[0].setTargetDeg(pos0);
    servos[1].setTargetDeg(pos1);
    servos[2].setTargetDeg(pos2);
    servos[3].setTargetDeg(pos3);
    servos[4].setTargetDeg(pos4);
  }

  switch (AB)
  {
  case 1:
    
    for (int i = 0; i < AMOUNT; i++)
    {
      servos[i].setTargetDeg(homePosition);
    }
    isHomeClick = true;
    PRINT("\nAB", AB);
    
    break;
  case 2:
    isSetPositionClick = true;
    for (int i = 0; i < AMOUNT; i++)
    {
      servos[i].setTargetDeg(maxDeg);
    }
    PRINT("\nAB", AB);
      
    break;
  case 3:
    isHomeClick = false;
    PRINT("\nAB", AB);
  break;
  
    
  default:
    break;
  }

   




  //int pos1 = analogRead(A0);
  //pos1 = map(pos1,0,1023,0,180);
  //int pos2 = map(analogRead(A1),0,1023,0,180);
  //int pos3 = map(analogRead(A2),0,1023,0,180);
  //int pos4 = map(analogRead(A3),0,1023,0,180);
  //int pos5 = map(analogRead(A6),0,1023,0,180);
  //servos[0].setTargetDeg(pos1);
  //servos[1].setTargetDeg(pos2);
  //servos[2].setTargetDeg(pos3);
  //servos[3].setTargetDeg(pos4);
  //servos[4].setTargetDeg(pos5);

  
}

uint8_t GetAB() {                                           // Функция устраняющая дребезг
  static int   count;
  static int   oldKeyValue;                                 // Переменная для хранения предыдущего значения состояния кнопок
  static int   innerKeyValue;
  uint8_t actualKeyValue = (analogRead(BPIN) / 171);        // Получаем актуальное состояние
  if (innerKeyValue != actualKeyValue) {                    // Пришло значение отличное от предыдущего
    count = 0;                                              // Все обнуляем и начинаем считать заново
    innerKeyValue = actualKeyValue;                         // Запоминаем новое значение
  }
  else {
    count += 1;                                             // Увеличиваем счетчик
  }
  if ((count >= 10) && (actualKeyValue != oldKeyValue)) {   // Счетчик преодолел барьер, можно иницировать смену состояний
    oldKeyValue = actualKeyValue;                           // Присваиваем новое значение
  }
  
  return    oldKeyValue;
}