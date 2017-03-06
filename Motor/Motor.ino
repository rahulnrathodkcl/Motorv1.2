#include "Definitions.h"
#include "Motor_MGR.h"
#include "SIM.h"
#include "S_EEPROM.h"
#include "BATTERY_MGR.h"

S_EEPROM eeprom1;

#ifndef disable_debug
  SoftwareSerial s1(5,6);
  #ifdef software_SIM
    SIM sim1(&Serial,&s1);
    BATTERY_MGR battery1(&Serial,&sim1,&eeprom1);
    Motor_MGR motor1(&Serial,&sim1,&eeprom1,&battery1);
    HardwareSerial* USART1=&Serial;
    SoftwareSerial* SUSART1=&s1;

  #else
    SIM sim1(&s1,&Serial);
    BATTERY_MGR battery1(&s1,&sim1,&eeprom1);
    Motor_MGR motor1(&s1,&sim1,&eeprom1,&battery1);
    SoftwareSerial* USART1=&s1;
    HardwareSerial* SUSART1=&Serial;
  #endif
#else
	#ifdef software_SIM
    SoftwareSerial s1(5,6);
    SIM sim1(&s1);
    SoftwareSerial* SUSART1=&s1;
  #else
    SIM sim1(&Serial);
  #endif
	BATTERY_MGR battery1(&sim1,&eeprom1);
  Motor_MGR motor1(&sim1,&eeprom1,&battery1);
#endif  

void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);
  #ifndef disable_debug
    USART1->begin(19200);
  #endif

    eeprom1.loadAllData();
	// attachInterrupt(digitalPinToInterrupt(PIN_PHASE1),IVR_PHASE,CHANGE);
	// attachInterrupt(digitalPinToInterrupt(PIN_PHASE2),IVR_PHASE,CHANGE);
	// attachInterrupt(digitalPinToInterrupt(PIN_ACPHASE),IVR_PHASE,CHANGE);

	sim1.setClassReference(&eeprom1,&motor1);
  #ifndef disable_debug
  USART1->println("Starting");	
  #endif
}

ISR(PCINT0_vect)
{
  motor1.eventOccured=true;   
}

ISR(BADISR_vect)
{
    #ifndef disable_debug
      USART1->println("!!!");
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

void printData()
{
  #ifndef disable_debug
    USART1->print("START V:");
    USART1->println(eeprom1.STARTVOLTAGE);

    USART1->print("STOP V:");
    USART1->println(eeprom1.STOPVOLTAGE);

    USART1->print("DND :");
    USART1->println(eeprom1.DND);

    USART1->print("RESPONSE :");
    USART1->println(eeprom1.RESPONSE);
 
    USART1->print("AUTO :");
    USART1->println(eeprom1.AUTOSTART);

    USART1->print("TIME :");
    USART1->println(eeprom1.AUTOSTARTTIME);

    USART1->print("TEMP :");
    USART1->println(eeprom1.HIGHTEMP);
    
    USART1->print("nos :");
    USART1->println(eeprom1.numbersCount);
    for(byte i = 0;i<eeprom1.numbersCount;i++)
    {
      if(i==0)
        USART1->println(eeprom1.primaryNumber);
      else
        USART1->println(eeprom1.secondary[i-1]);
    }
  #endif
}

String str;
bool simDebugMode=false;
bool initialized=false;
void loop() {
  // put your main code here, to run repeatedly:

  if(!initialized)
  {
    if(millis()>=10000)
    {

      if (!sim1.initialize())
      {
        #ifndef disable_debug
          USART1->println("NOT INIT SIM");
        #endif
      }
      motor1.eventOccured=true;
      // motor1.getMotorState();
      
      PCICR |= (1 << PCIE0);   // set PCIE0 to enable PCMSK0 scan
      PCMSK0 |= (1 << PCINT1); // set PCINT1 to trigger an interrupt on state change
      PCMSK0 |= (1 << PCINT2);// set PCINT2 to trigger an interrupt on state change
      PCMSK0 |= (1 << PCINT3);// set PCINT3 to trigger an interrupt on state change
      
      initialized=true;
    }
    return;
  }


  #ifndef disable_debug
    if(simDebugMode)
    {
        if(SUSART1->available()>0)
          USART1->print(SUSART1->readString());
    }

    if (USART1->available() > 0)
    {
      str = USART1->readStringUntil('\n');
      if(simDebugMode)
      {
        if(str=="SIMDEBUGOFF\r")
        {
          simDebugMode=false;
          USART1->print("SIM DEBUG ");
          USART1->println("OFF");
        }
        else
          SUSART1->println(str);
      }
      else
      {
          if(str=="S\r")
            motor1.startMotor();
          else if(str=="A\r")
            motor1.stopMotor();
          else if(str=="SIMDEBUGON\r")
          {
            simDebugMode=true;
            USART1->print("SIM DEBUG ");
            USART1->println("ON");
          }
          else if(str=="PRINTEEPROM\r")
              printData();
          else
              sim1.operateOnMsg(str,true);
      }
    }
  #endif
  
	sim1.update();
	battery1.update();
	motor1.update();
	//eeprom1.update();
}