#include "Definitions.h"
#include "Motor_MGR.h"
#include "SIM.h"
#include "S_EEPROM.h"

S_EEPROM eeprom1;

bool batStatus;
bool batLevelChange=false;

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
  byte b=MCUSR;

  pinMode(PIN_BATLEVEL,INPUT_PULLUP);
  pinMode(PIN_TURNOFF,OUTPUT);

  if(digitalRead(PIN_BATLEVEL)==LOW)
  {
    batStatus=false;
    digitalWrite(PIN_TURNOFF,LOW);
  }
  else
  {
    digitalWrite(PIN_TURNOFF,HIGH);
    batStatus=true;
  }

  PCICR |= (1 << PCIE0);    // set PCIE0 to enable PCMSK0 scan
  PCMSK0 |= (1 << PCINT1);  // set PCINT1 to trigger an interrupt on state change


  noInterrupts();
  watchdogConfig(WATCHDOG_OFF);
  // wdt_init();
  // put your setup code here, to run once:
  interrupts();

  Serial.begin(19200);
#ifndef disable_debug
  USART1->begin(19200);
#endif

    if (b & _BV(BORF))
    {
    #ifndef disable_debug
      USART1->println("Brown Out Reset");
    #endif
    
    }

  
  pinMode(PIN_LED,OUTPUT);
  digitalWrite(PIN_LED,HIGH);
  
  eeprom1.loadAllData();
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

void IVR_RING(){}   //stub, used to wake up the MCU by SIM by generating interrupt on PIN_RING

ISR(PCINT0_vect)
{
  byte b = digitalRead(PIN_BATLEVEL);
  if(b==batStatus)
    motor1.eventOccured = true;
  else
  {
    batStatus=b;
    batLevelChange=true;    
  }

  #ifndef disable_debug
    USART1->println("PCI");
  #endif
  // motor1.eventOccured = true;
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

bool checkSleepElligible()
{
  return (!motor1.ACPowerState() && motor1.checkSleepElligible() && sim1.checkSleepElligible());
}

String str;
bool simDebugMode = false;
bool initialized = false;
bool checkedUpdate=false;

unsigned short temp=0;
bool led=true;
bool initSleepSeqeunce=false;
unsigned long tempSleepWait=0;
unsigned short sleepWaitTime=20000;

void flashLed()
{
    if(millis()-temp>500)
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
}


void operateOnSleepElligible()
{
    if(!initSleepSeqeunce)
    {
      // sim1.setNetLight(L_SLEEP);
      #ifndef disable_debug
        USART1->print(F("Sleep Init Seq :"));
        USART1->println(millis());
      #endif
      digitalWrite(PIN_LED,LOW);
      tempSleepWait=millis();
      initSleepSeqeunce=true;
    }
    else if(initSleepSeqeunce && millis()-tempSleepWait>sleepWaitTime)
    {
      #ifndef disable_debug
        USART1->print(F("Enter sleep :"));
        USART1->println(millis());
      #endif
        byte cnt=6;
        bool led=false;
        do
        {
            led=!led;
            if(led)
              digitalWrite(PIN_LED,HIGH);
            else
              digitalWrite(PIN_LED,LOW);

            tempSleepWait=millis();
            while(millis()-tempSleepWait<300)
            {}
        }while(--cnt);               
        gotoSleep();
        initSleepSeqeunce=false;      //after wakeup 

        // sim1.setNetLight(L_REGULAR);

      #ifndef disable_debug
        USART1->print(F("WakeUp :"));
        USART1->println(millis());
      #endif

    }
}

void gotoSleep()
{

  set_sleep_mode(SLEEP_MODE_IDLE);   // sleep mode is set here
  noInterrupts();
  sleep_enable();          // enables the sleep bit in the mcucr register
                             // so sleep is possible. just a safety pin 
  power_adc_disable();
  // power_aca_disable();
  power_spi_disable();
  power_timer0_disable();
  power_timer1_disable();
  power_timer2_disable();
  power_twi_disable();

  interrupts();
  sleep_mode();            // here the device is actually put to sleep!!
 
                             // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
  sleep_disable();         // first thing after waking from sleep:
                            // disable sleep...
  power_all_enable();
}


unsigned long tempTurnoffTime=0;
bool turnOffTimerOn=false;
bool initTurnoff=false;

void loop() {
  // put your main code here, to run repeatedly:
    // if(digitalRead(PIN_BATLEVEL)==HIGH)
      // digitalWrite(PIN_TURNOFF,HIGH);

  if(turnOffTimerOn)
  {
    if(millis()-tempTurnoffTime>15000)
    {
      if(digitalRead(PIN_BATLEVEL)==LOW && sim1.checkSleepElligible())
      {
        turnOffTimerOn=false;
        USART1->println("LOW BAT");
        digitalWrite(PIN_TURNOFF,LOW);  
      }
      else
        turnOffTimerOn=false;
    }    
  }

  if(batLevelChange)
  {
    batLevelChange=false;
    if(!batStatus && sim1.checkSleepElligible())
    {
       tempTurnoffTime=millis();
       turnOffTimerOn=true;
      USART1->println("LOW BAT DET");

    }
    else
    {
      digitalWrite(PIN_TURNOFF,HIGH);
    }
  }
  // if(batLevelChange && sim1.checkSleepElligible())//&& !turnOffTimerOn)
  // {
    // if(digitalRead(PIN_BATLEVEL)==LOW)
    // {
    // }
    // else 
    // {
      // digitalWrite(PIN_TURNOFF,HIGH);
    // }
  // }

  if (!initialized)
  {  
    if(!checkedUpdate && millis() >= 2000)
    {
      sim1.startSIMAfterUpdate(eeprom1.getUpdateStatus());
      eeprom1.discardUpdateStatus();
      checkedUpdate=true;
    }
    
    // flashLed();

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
      attachInterrupt(digitalPinToInterrupt(PIN_ACPHASE), IVR_PHASE, CHANGE);
      attachInterrupt(digitalPinToInterrupt(PIN_RING), IVR_RING, CHANGE);

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
        sim1.sendCommand(str,true);
        // SUSART1->println(str);
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

  flashLed();
  sim1.update();
  motor1.update();

  if(initSleepSeqeunce)
    flashLed();

  if(checkSleepElligible())
    operateOnSleepElligible();
  else
  {
    initSleepSeqeunce=false;
    digitalWrite(PIN_LED,LOW);
  }
}
