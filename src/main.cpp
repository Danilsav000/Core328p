#include <core.h>
#include <Arduino.h>
#include <ServoSmooth.h>
#include <wire.h>


#define AMOUNT 5 // указываем колличество приводов используемых в проекте
#define BPIN A7

ServoSmooth servos[AMOUNT];

int Servo1Position = 0;
int Servo2Position = 20;
int Servo3Position = 40;
int Servo4Position = 60;
int Servo5Position = 80;

int impulsMin = 600;
int impulsMax = 2400;

long interval = 15;
long previousMillis = 0;

uint8_t AB;


boolean flag;

void setup() {
  Serial.begin(9600);

  servos[0].attach(11);
  servos[1].attach(10);
  servos[2].attach(9);
  servos[3].attach(6);
  servos[4].attach(5);

  servos[0].setAccel(0.1);
  servos[1].setAccel(0.2);
  servos[2].setAccel(0.3);
  servos[3].setAccel(0.4);
  servos[4].setAccel(0.5);

  servos[0].setSpeed(20);
  servos[1].setSpeed(40);
  servos[2].setSpeed(60);
  servos[3].setSpeed(80);
  servos[4].setSpeed(100);



}

void loop() {
    servos[0].tick();
    servos[1].tick();
    servos[2].tick();
    servos[3].tick();
    servos[4].tick();

    //unsigned long currentMillis = millis();
    //interval = millis();
    //if(currentMillis - previousMillis > interval) {

    int pos1 = analogRead(A0);
    pos1 = map(pos1,0,1023,0,180);
    int pos2 = map(analogRead(A1),0,1023,0,180);
    int pos3 = map(analogRead(A2),0,1023,0,180);
    int pos4 = map(analogRead(A3),0,1023,0,180);
    int pos5 = map(analogRead(A6),0,1023,0,180);
    servos[0].setTargetDeg(pos1);
    servos[1].setTargetDeg(pos2);
    servos[2].setTargetDeg(pos3);
    servos[3].setTargetDeg(pos4);
    servos[4].setTargetDeg(pos5);
    //}
  
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