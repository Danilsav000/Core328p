//#define DEBUG  //Раскоментировать чтобы включить отладку по уарт
#define GS_NO_ACCEL // отключить модуль движения с ускорением (уменьшить вес кода)
#define DRIVER_STEP_TIME 20
//#define USE_MICRO_WIRE
//#include <core.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <ServoSmooth.h>
#include <wire.h>
#include <GyverOLED.h>
#include <GyverStepper2.h>
#include <GyverWDT.h>



//работа с памтью EEPROM
#define INIT_ADDR 1023
#define INIT_KEY 254

//функции для отладки
#ifdef DEBUG
#define PRINTS(x) {Serial.print(F(x)); }
#define PRINT(s,v)  { Serial.print(F(s)); Serial.print(v); }
#else 
#define PRINTS(x)
#define PRINT(s,v)
#endif

//сервы
#define AMOUNT 5 // указываем колличество приводов используемых в проекте
#define BPIN A7
#define impulsMin  500  //600
#define impulsMax 2400 //2600
#define servosSpeed 100
#define servosAccel 0.5
#define homePosition 180
#define maxDeg 180

//таймеры
#define MY_PERIOD 1000
#define STEPP_PERIOD 5000
#define BTN_PERIOD 10000
uint32_t tmr1; 
uint32_t tmr2; 
uint32_t tmr3;
uint32_t tmr_stp;
uint32_t tmr_oled;

//Шаговик
#define steps 3200
#define pinStep 8
#define pinDir 7
#define pinEnable 12


//объекты сервы, экран, шаговик
ServoSmooth servos[AMOUNT];
GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> oled;
GStepper2<STEPPER2WIRE> stepper(steps, pinStep, pinDir, pinEnable);




uint8_t AB;


boolean isRepeatTrue = false;
boolean isManualClick = false;

uint8_t GetAB();
void OledPrint();
void StepperCCW();
void StepperCW();
void checkEEPROM();
void WriteStatusBTN();
void ManualBTN();

uint32_t eepromTimer = 0;
boolean eepromFlag = false;

int stepperInitialpoint = 0;
int16_t servo0InitialPoint = 2400;
int16_t servo1InitialPoint = 2400;
int16_t servo2InitialPoint = 2400;
int16_t servo3InitialPoint = 2400;
int16_t servo4InitialPoint = 2400;

#define numOfValsRec 5
#define digitsPerValRec 1
int valsRec[numOfValsRec];
int stringLength = numOfValsRec * digitsPerValRec + 1; //$00000
int counter = 0;
bool countStart = false;
String receivedString;

char ch;
char res;
 

void setup() {

  Serial.begin(9600);
  Serial.setTimeout(10);
  //EEPROM.put(254, 10); // раскоментировать чтобы записать начальные значения в EEPROM

    if (EEPROM.read(INIT_ADDR) != INIT_KEY) { // первый запуск
    EEPROM.write(INIT_ADDR, INIT_KEY);    // записали ключ
    // записали стандартное значение 
    // в данном случае это значение переменной
    EEPROM.put(0, stepperInitialpoint);
    EEPROM.put(10, servo0InitialPoint);
    EEPROM.put(20, servo1InitialPoint);
    EEPROM.put(30, servo2InitialPoint);
    EEPROM.put(40, servo3InitialPoint);
    EEPROM.put(50, servo4InitialPoint);
  }

  EEPROM.get(0, stepperInitialpoint);
  EEPROM.get(10, servo0InitialPoint);
  EEPROM.get(20, servo1InitialPoint);
  EEPROM.get(30, servo2InitialPoint);
  EEPROM.get(40, servo3InitialPoint);
  EEPROM.get(50, servo4InitialPoint);
  
  
  PRINTS("\nversion 1.0, arduino nano");
  PRINT("\nServos connected: ", AMOUNT);
  PRINT("\nSet speed: ", servosSpeed);
  PRINT("\nSet accel: ", servosAccel);
  PRINTS("\nНачнем!");
  PRINT("\nПозиция шаговика: ", stepperInitialpoint);
  PRINT("\nПозиция Серво 0: ", servo0InitialPoint);
  PRINT("\nПозиция Серво 1: ", servo1InitialPoint);
  PRINT("\nПозиция Серво 2: ", servo2InitialPoint);
  PRINT("\nПозиция Серво 3: ", servo3InitialPoint);
  PRINT("\nПозиция Серво 4: ", servo4InitialPoint);


  oled.init();  // инициализация дисплея
  oled.clear();
  oled.autoPrintln(true);
  oled.home(); //устанавливаем курсор в 0,0
  oled.setScale(2);
  oled.print(F("Привет!"));
  delay(500);


  servos[0].attach(11, impulsMin, impulsMax, servo0InitialPoint);
  servos[1].attach(10, impulsMin, impulsMax, servo1InitialPoint);
  servos[2].attach(9, impulsMin, impulsMax, servo2InitialPoint);
  servos[3].attach(6, impulsMin, impulsMax, servo3InitialPoint);
  servos[4].attach(5, impulsMin, impulsMax, servo4InitialPoint);
  oled.clear();
  oled.setCursor(20,2);
  oled.print(F("Приводы подключены"));
  delay(500);



  for (int i = 0; i < AMOUNT; i++)
  {
    servos[i].setAccel(servosAccel);
    servos[i].setSpeed(servosSpeed);
    //servos[i].setMaxAngle(maxDeg);
    //servos[i].smoothStart();
  }
  
  OledPrint();
  oled.print(F("ГОТОВ!!!"));
  delay(500);
  oled.clear();

  stepper.setMaxSpeed(400); 
  stepper.setAcceleration(500);
  stepper.setCurrent(stepperInitialpoint);
  stepper.setTarget(servo0InitialPoint, ABSOLUTE); 
  
  
  stepper.autoPower(true);
}

/* void receivedData(){
  while (Serial.available())
  {
    char ch = Serial.read();
    if (ch=='$')
    {
      countStart = true;
    }

    if (countStart) {
      if (counter < stringLength)
      {
        receivedString = String(receivedString + ch);
        counter++;
      }
      if (counter >= stringLength)
      {
        for (int i = 0; i < numOfValsRec; i++)
        {
          int num = (i * digitsPerValRec) + 1;
          valsRec[0] = receivedString.substring(num,num + digitsPerValRec).toInt(); 
        }
        receivedString = "";
        counter = 0;
        countStart = false;
        
        
      }
      
      
    }
    
    
  }
  
}
 */
 char receivedData(){
  if (millis() - tmr_stp >= 40) {
    tmr3 = millis();
    if (Serial.available()) {
      ch = Serial.read();
    }

    oled.setCursor(60,2);
    oled.print(ch);
    oled.update();
    
    return ch;
  }
 }
  
uint32_t movementStepper(uint32_t step){
  if (!stepper.ready()){
    stepper.enable();
    stepper.setTarget(-step, RELATIVE);
  }
  
}

void loop() {


  servos[0].tick();
  servos[1].tick();
  servos[2].tick();
  servos[3].tick();
  servos[4].tick();
  stepper.tick();

  
  /* if (Serial.available())
  {
    ch = Serial.read();
    oled.print(ch);
  } */

  if (isRepeatTrue==true)
  {
    res = receivedData();
  }
  
  

  switch (res)
  {

  case 'L':{
      movementStepper(-100);
    }
    break;
  case 'R':{
      movementStepper(100);
    }
    break;
  case 'M':{
    stepper.disable();
    break;

  case 'C':{
    servos[0].setTarget(impulsMax);
  }
  break;

  case 'O':{
    servos[0].setTarget(impulsMin);
  }
  break;
  default:
    break;
  }
  }


  

  
  //if (valsRec[0] == 1){servos[0].setTarget(impulsMax);} else {servos[0].setTarget(impulsMin);}


  
  



  if (millis() - tmr2 >= STEPP_PERIOD)
  {
      tmr2 = millis();
      stepper.disable();
  }
  if (isManualClick == true) {
    if (millis() - tmr3 >= 40) {
      tmr3 = millis();
      int pos0 = map(analogRead(A0),0,1023,impulsMin,impulsMax);
      int pos1 = map(analogRead(A1),0,1023,impulsMin,impulsMax);
      int pos2 = map(analogRead(A2),0,1023,impulsMin,impulsMax);
      int pos3 = map(analogRead(A3),0,1023,impulsMin,impulsMax);
      int pos4 = map(analogRead(A6),0,1023,impulsMin,impulsMax);
      servos[0].setTarget(pos0);
      servos[1].setTarget(pos1);
      servos[2].setTarget(pos2);
      servos[3].setTarget(pos3);
      servos[4].setTarget(pos4);
  }
  }

AB = GetAB();
switch (AB)
  {
  case 1:
  {
    for (int i = 0; i < AMOUNT; i++)
    {
      servos[i].setTarget(2600);
    }

    isManualClick = false;
    isRepeatTrue = true;
    PRINT("\nAB", AB);
    OledPrint();
    oled.print(F("Auto"));
  }
    break;
  case 2:
  {
  
    
    ManualBTN();
  }
      
    break;
  case 3:
  {
    WriteStatusBTN();
  }
  break;

  case 4: {
      if (isManualClick==true)
    {
      StepperCCW();
    }
    
  }
  break;

    case 5: {
      if (isManualClick==true)
    {
    StepperCW();
    }
  }
  
  break;
  
    
  default:
    break;
  }







  
}

void checkEEPROM() {

    //stepperInitialpoint = stepper.getCurrent();
    EEPROM.put(0, stepper.getCurrent());     // записали в EEPROM текущее положение двигателя
    EEPROM.put(10, servos[0].getCurrent());
    EEPROM.put(20, servos[1].getCurrent());
    EEPROM.put(30, servos[2].getCurrent());
    EEPROM.put(40, servos[3].getCurrent());
    EEPROM.put(50, servos[4].getCurrent());
    
  }


void ManualBTN(){
    if (Serial.available())
    {
      Serial.end();
    }
    isRepeatTrue = false;
    isManualClick = true;
    PRINT("\nAB", AB);
    OledPrint();
    oled.print(F("Manual mode"));
}

void WriteStatusBTN(){
  int degr[AMOUNT];

    if (millis() - tmr1 >= MY_PERIOD) {
      tmr1 = millis(); 
      for (int i = 0; i < AMOUNT; i++)
    {
      degr[i] = servos[i].getCurrent();
    }
    
    OledPrint();
    oled.setScale(1);
    oled.textMode(BUF_ADD);
    for (int i = 0; i < AMOUNT; i++)
    {
      oled.print(F("Позиция:")) + oled.println(degr[i]);
    }
    oled.print(F("Motor: ")) + oled.print(stepper.getCurrent());
    oled.setScale(2);
    PRINT("\nAB", AB);

    checkEEPROM();
    PRINT("\nПозиция шаговика: ", stepperInitialpoint);
    }  
}

void StepperCCW(){
  if (!stepper.ready())
  {
    stepper.enable();
    stepper.setTarget(100, RELATIVE);
  }


      PRINT("\nПозиция шаговика: ", stepper.pos);
    

  
  
}

void StepperCW(){
    if (!stepper.ready())
  {
    stepper.enable();
    stepper.setTarget(-100, RELATIVE);
  }


    PRINT("\nПозиция шаговика: ", stepper.pos);
    

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
  if ((count >= 3) && (actualKeyValue != oldKeyValue)) {   // Счетчик преодолел барьер, можно иницировать смену состояний
    oldKeyValue = actualKeyValue;                           // Присваиваем новое значение
  }
  
  return    oldKeyValue;
}

void OledPrint() {
  oled.clear();
  oled.home();

}

