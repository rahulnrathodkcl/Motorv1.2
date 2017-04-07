#include "Definitions.h"
#include "Motor_MGR.h"
#include "SIM.h"
#include "S_EEPROM.h"

S_EEPROM eeprom1;

// Function Pototype
// void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3")));
// void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3")));

void __attribute__((noinline)) watchdogConfig(uint8_t x);

#ifndef disable_debug
SoftwareSerial s1(5, 6);
#ifdef software_SIM
SIM sim1(&Serial, &s1);
// Motor_MGR motor1(&Serial,&sim1,&eeprom1,&battery1);
Motor_MGR motor1(&Serial, &sim1, &eeprom1);
HardwareSerial* USART1 = &Serial;
SoftwareSerial* SUSART1 = &s1;

#else
SIM sim1(&s1, &Serial);
// Motor_MGR motor1(&s1,&sim1,&eeprom1,&battery1);
Motor_MGR motor1(&s1, &sim1, &eeprom1);
SoftwareSerial* USART1 = &s1;
HardwareSerial* SUSART1 = &Serial;
#endif
#else
#ifdef software_SIM
SoftwareSerial s1(5, 6);
SIM sim1(&s1);
SoftwareSerial* SUSART1 = &s1;
#else
SIM sim1(&Serial);
#endif
Motor_MGR motor1(&sim1, &eeprom1);
#endif



void setup() {
  noInterrupts();
  watchdogConfig(WATCHDOG_OFF);
  // wdt_init();
  // put your setup code here, to run once:
  interrupts();

  Serial.begin(19200);
#ifndef disable_debug
  USART1->begin(19200);
#endif
  
  pinMode(PIN_LED,OUTPUT);
  digitalWrite(PIN_LED,HIGH);
  
  eeprom1.loadAllData();
  attachInterrupt(digitalPinToInterrupt(PIN_ACPHASE), IVR_PHASE, CHANGE);
  // attachInterrupt(digitalPinToInterrupt(PIN_PHASE1),IVR_PHASE,CHANGE);
  // attachInterrupt(digitalPinToInterrupt(PIN_PHASE2),IVR_PHASE,CHANGE);
  // attachInterrupt(digitalPinToInterrupt(PIN_ACPHASE),IVR_PHASE,CHANGE);

  sim1.setClassReference(&eeprom1, &motor1);
#ifndef disable_debug
  USART1->println("ST");
#endif
}


void watchdogConfig(uint8_t x) {
  WDTCSR = _BV(WDCE) | _BV(WDE);
  WDTCSR = x;
}

void IVR_PHASE()
{
  motor1.eventOccured = true;
}

ISR(PCINT0_vect)
{
  motor1.eventOccured = true;
}

ISR(BADISR_vect)
{
#ifndef disable_debug
  USART1->println("!");
  USART1->println(MCUSR);
#endif
}

// ISR(PCINT2_vect)
// {
//   #ifdef software_SIM
//     SUSART1->gotInterrupt();
//   #else
//     #ifndef disable_debug
//       USART1->gotInterrupt();
//     #endif
//   #endif
// }

//...

// Function Implementation
// void wdt_init(void)
// {
    // MCUSR = 0;
    // wdt_disable();
    // return;
// }

void printData()
{
#ifndef disable_debug
  // USART1->print("START:");
  // USART1->println(eeprom1.STARTVOLTAGE);

  // USART1->print("STOP:");
  // USART1->println(eeprom1.STOPVOLTAGE);

  // USART1->print("DND:");
  USART1->println(eeprom1.DND);

  USART1->print("RES:");
  USART1->println(eeprom1.RESPONSE);

  USART1->print("AUT:");
  USART1->println(eeprom1.AUTOSTART);

  USART1->print("TIM:");
  USART1->println(eeprom1.AUTOSTARTTIME);

  // USART1->print("TEMP:");
  // USART1->println(eeprom1.HIGHTEMP);

  USART1->print("nos:");
  USART1->println(eeprom1.numbersCount);
  for (byte i = 0; i < eeprom1.numbersCount; i++)
  {
    if (i == 0)
      USART1->println(eeprom1.primaryNumber);
    else
      USART1->println(eeprom1.secondary[i - 1]);
  }
#endif
}

String str;
bool simDebugMode = false;
bool initialized = false;
bool checkedUpdate=false;
unsigned short temp=0;
bool led=true;
void loop() {
  // put your main code here, to run repeatedly:


  if (!initialized)
  {
    if(!checkedUpdate && millis() >= 2000)
    {
      if(eeprom1.getUpdateStatus())
      {
          eeprom1.discardUpdateStatus();
          checkedUpdate=true;
          sim1.startSIMAfterUpdate();
      }
    }
    
    if(millis()-temp>2000)
    {
        if(led)
        {
          digitalWrite(PIN_LED,LOW);
          led=false;
        }
        else
        {
          digitalWrite(PIN_LED,HIGH);
          led=true;
        }
        temp=millis();
    }


    if (millis() >= 25000)
    {
      digitalWrite(PIN_LED,LOW);

      if (!sim1.initialize())
      {
#ifndef disable_debug
        USART1->println("NOSIM");
#endif
      }
      motor1.eventOccured = true;
      motor1.resetAutoStart();
      // motor1.getMotorState();

      PCICR |= (1 << PCIE0);   // set PCIE0 to enable PCMSK0 scan
      PCMSK0 |= (1 << PCINT1); // set PCINT1 to trigger an interrupt on state change
      PCMSK0 |= (1 << PCINT2);// set PCINT2 to trigger an interrupt on state change
      PCMSK0 |= (1 << PCINT3);// set PCINT3 to trigger an interrupt on state change

      initialized = true;
    }
    return;
  }


#ifndef disable_debug
  if (simDebugMode)
  {
    if (SUSART1->available() > 0)
      USART1->print(SUSART1->readString());
  }

  if (USART1->available() > 0)
  {
    str = USART1->readStringUntil('\n');
    if (simDebugMode)
    {
      if (str == "SIMOFF\r")
      {
        simDebugMode = false;
        USART1->println("SIMOFF\r");
      }
      else
        SUSART1->println(str);
    }
    else
    {
      if (str == "S\r")
        motor1.startMotor();
      else if (str == "A\r")
        motor1.stopMotor();
      else if (str == "SIMON\r")
      {
        simDebugMode = true;
        USART1->println("SIMON\r");
      }
      else if (str == "PPROM\r")
        printData();
      else
        sim1.operateOnMsg(str, true);
    }
  }
#endif

  sim1.update();
  motor1.update();
}
