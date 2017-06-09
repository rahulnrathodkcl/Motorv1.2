#include "Definitions.h"
#include "Motor_MGR.h"
#include "SIM.h"
#include "S_EEPROM.h"

S_EEPROM eeprom1;

bool batStatus;
bool batLevelChange=false;


String str;
bool simDebugMode = false;
bool initialized = false;
// bool checkedUpdate=false;
bool initCFUN=false;

unsigned short temp=0;
bool led=true;
bool initSleepSeqeunce=false;
unsigned long tempSleepWait=0;
unsigned short sleepWaitTime=20000;


unsigned long tempTurnoffTime=0;
bool turnOffTimerOn=false;
bool gotLowBatEvent=false;
bool initTurnOn=false;

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
  pinMode(PIN_TURNOFF,OUTPUT);
  digitalWrite(PIN_TURNOFF,LOW);
  pinMode(PIN_BATLEVEL,INPUT_PULLUP);

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
      USART1->println(F("BO Reset"));
    #endif
    
    }

  pinMode(PIN_LED,OUTPUT);
  
  eeprom1.loadAllData();
  pinMode(8,INPUT_PULLUP);
  pinMode(12,INPUT_PULLUP);
  pinMode(A1,INPUT_PULLUP);
  #ifdef disable_debug
  pinMode(5,INPUT_PULLUP);
  pinMode(6,INPUT_PULLUP);
  #endif

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

void IVR_RING()
{
  sim1.tempInterruptTime=millis();
  sim1.inInterrupt=true;
}   //stub, used to wake up the MCU by SIM by generating interrupt on PIN_RING

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

#ifndef disable_debug
void printData()
{
  // USART1->print("START:");
  // USART1->println(eeprom1.STARTVOLTAGE);

  // USART1->print("STOP:");
  // USART1->println(eeprom1.STOPVOLTAGE);

  USART1->print("DND:");
  USART1->println(eeprom1.DND);

  USART1->print("RES:");
  USART1->println(eeprom1.RESPONSE);

  USART1->print("AUT:");
  USART1->println(eeprom1.AUTOSTART);

  USART1->print("TIM:");
  USART1->println(eeprom1.AUTOSTARTTIME);

  // USART1->print("TEMP:");
  // USART1->println(eeprom1.HIGHTEMP);

  USART1->print("nos:" + eeprom1.numbersCount);
  USART1->println(eeprom1.getNumbers());
  // USART1->println(eeprom1.numbersCount);
  // for (byte i = 0; i < eeprom1.numbersCount; i++)
  // {
  //   if (i == 0)
  //     USART1->println(eeprom1.primaryNumber);
  //   else
  //     USART1->println(eeprom1.secondary[i - 1]);
  // }
}
#endif

bool checkSleepElligible()
{
  return (!turnOffTimerOn && !motor1.ACPowerState() && motor1.checkSleepElligible() && sim1.checkSleepElligible());
}

void flashLed()
{
    if(millis()-temp>500)
    {
        led=!led;
        if(led) digitalWrite(PIN_LED,HIGH);
        if(!led) digitalWrite(PIN_LED,LOW);
        temp=millis();
    }
}

void operateOnSleepElligible()
{
    if(!initSleepSeqeunce)
    {
      // sim1.setNetLight(L_SLEEP);
      #ifndef disable_debug
        USART1->print(F("S I Seq :"));
        USART1->println(millis());
      #endif
      // digitalWrite(PIN_LED,LOW);
      tempSleepWait=millis();
      initSleepSeqeunce=true;
    }
    else if(initSleepSeqeunce && millis()-tempSleepWait>sleepWaitTime)
    {
      #ifndef disable_debug
        USART1->print(F("Ent sleep:"));
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
            while(millis()-tempSleepWait<200)
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
// SLEEP_MODE_PWR_DOWN
  // SLEEP_MODE_IDLE
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
  noInterrupts();
  sleep_enable();          // enables the sleep bit in the mcucr register
                             // so sleep is possible. just a safety pin 
  power_all_disable();
  interrupts();

  // power_adc_disable();
  // // power_aca_disable();
  // power_spi_disable();
  // power_timer0_disable();
  // power_timer1_disable();
  // power_timer2_disable();
  // power_twi_disable();

  MCUCR |= (3<<5);                        //disable BOD before sleeing
  MCUCR = (MCUCR & ~(1<<5)) | (1<<6);     // it is timed sequence, so need to execute before sleep_cpu() only.

  sleep_cpu();            // here the device is actually put to sleep!!
                             // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
  sleep_disable();         // first thing after waking from sleep:
                            // disable sleep...
  power_all_enable();
}


void loop() {
  // put your main code here, to run repeatedly:
    // if(digitalRead(PIN_BATLEVEL)==HIGH)
      // digitalWrite(PIN_TURNOFF,HIGH);

  if(turnOffTimerOn)
  {
    if(millis()-tempTurnoffTime>10000)
    {
      if(!batStatus && sim1.checkNotInCall())
      {
        turnOffTimerOn=false;
        #ifndef disable_debug
        USART1->println(F("OFF"));
        #endif
        digitalWrite(PIN_TURNOFF,LOW);  
      }
      else
        turnOffTimerOn=false;
    }    
  }

  if(batLevelChange)
  {
    batLevelChange=false;
    if(!batStatus)// && sim1.checkNotInCall() && !sim1.busy())
      {
        #ifndef disable_debug
          USART1->println(F("LOW B 1"));
        #endif
        gotLowBatEvent=true;
      }
    else
    {
      gotLowBatEvent=false;
      digitalWrite(PIN_TURNOFF,HIGH);
      if(!initTurnOn)
        initTurnOn=true;
    }
  }

  if(gotLowBatEvent && sim1.checkNotInCall() && !sim1.busy())
  {
        gotLowBatEvent=false;
        tempTurnoffTime=millis();
        turnOffTimerOn=true;
        #ifndef disable_debug
        USART1->println(F("LOW"));
        #endif
  }

  if (!initialized)
  {  
    if(!initCFUN && millis() >= 2000)
    {
      sim1.startSIMAfterUpdate();
      initCFUN=true;
      // eeprom1.getUpdateStatus();
      // eeprom1.discardUpdateStatus();
      // checkedUpdate=true;
    }
    
    if(initTurnOn)
      flashLed();

    if (millis() >= 25000)
    {
      if (!sim1.initialize())
      {
#ifndef disable_debug
        USART1->println(F("NOSIM"));
#endif
      }
      sim1.sendUpdateStatus(eeprom1.getUpdateStatus());
      eeprom1.discardUpdateStatus();

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
      led=false;
      digitalWrite(PIN_LED,LOW);
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
    str.toUpperCase();
    if (simDebugMode)
    {
      if (str == F("SIMOFF\r"))
      {
        simDebugMode = false;
        USART1->println(str);
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
      else if (str == F("SIMON\r"))
      {
        simDebugMode = true;
        USART1->println(str);
      }
      else if (str == F("PPROM\r"))
        printData();
      else if(str == "CSL\r")
      {
        USART1->println(gotLowBatEvent);
        USART1->println(batStatus);
        USART1->println(sim1.checkNotInCall());
        USART1->println(sim1.busy());
        USART1->println(turnOffTimerOn);
      }
      else if(str.startsWith("@"))
      {
        // not to remove '\r' from string, as it would be removed by operateOnMsg() in SIM
        // so, the stop index of substring should be str.length().
        // otherwise , if '\r' is not removed in operateOnMsg than str.length() -1 should be stop index.
        str = str.substring(1,str.length());      
        sim1.operateOnMsg(str, true,true);
      }
    }
  }
#endif

  sim1.update();
  motor1.update();

  if(checkSleepElligible())
    operateOnSleepElligible();
  else
  {
    initSleepSeqeunce=false;
  }
}