#include "BATTERY_MGR.h"

#ifndef disable_debug
  #ifdef software_SIM
    BATTERY_MGR::BATTERY_MGR(HardwareSerial *serial,SIM* s1,S_EEPROM* e1)
    {
      _NSerial = serial;
      _NSerial->begin(19200);
      anotherConstructor(s1,e1);
    }
  #else
    BATTERY_MGR::BATTERY_MGR(SoftwareSerial *serial,SIM* s1,S_EEPROM* e1)
    {
      _NSerial = serial;
      _NSerial->begin(19200);
      anotherConstructor(s1,e1);
    }
  #endif
#else
  BATTERY_MGR::BATTERY_MGR(SIM* s1,S_EEPROM* e1)
  {
    anotherConstructor(s1,e1);
  }
#endif

void BATTERY_MGR::anotherConstructor(SIM* s1,S_EEPROM* e1)
{
	pinMode(PIN_BAT_SENSOR_PWR,OUTPUT);
	pinMode(PIN_BAT_CHARGE,OUTPUT);
	stopCharging();
	readingWaitTime=250;
	signalState=false;
	reportedLowBattery=false;
	lowBattery=false;	

	sim1=s1;
	eeprom1=e1;
}


void BATTERY_MGR::turnBatSignalOn()
{
	digitalWrite(PIN_BAT_SENSOR_PWR,HIGH);
	signalState=true;
}

void BATTERY_MGR::turnBatSignalOff()
{
	digitalWrite(PIN_BAT_SENSOR_PWR,LOW);
	signalState=false;
	readingWaitTemp=millis();
}

bool BATTERY_MGR::readingElligible()
{
	return(!signalState && millis()-readingWaitTemp>=(readingWaitTime*100));
}

float BATTERY_MGR::detectBatLevel()
{
	turnBatSignalOn();
	analogRead(PIN_BAT_SENSOR);
	float data=analogRead(PIN_BAT_SENSOR);
	turnBatSignalOff();
	data=data*VOLTAGERATIO;
	data=data*4.0/1024;
	#ifndef disable_debug
		_NSerial->print("BAT VOLT:");
		_NSerial->println(data);
	#endif
	return data;
}

bool BATTERY_MGR::startCharging()
{
	initiateCharging=true;
	if(eeprom1->ACPowerState())		//AC power On
	{
		digitalWrite(PIN_BAT_CHARGE,HIGH);
		chargeState=true;

		#ifndef disable_debug
			_NSerial->print("Started ");
			_NSerial->println("Charging");
		#endif
	}
	else							//AC power off
		chargeState=false;

	return chargeState;
}

void BATTERY_MGR::stopCharging()
{
	digitalWrite(PIN_BAT_CHARGE,LOW);
	chargeState=false;
	initiateCharging=false;
	#ifndef disable_debug
		_NSerial->print("Stopped ");
		_NSerial->println("Charging");
	#endif
}

void BATTERY_MGR::reportLowBattery()
{
	reportedLowBattery=sim1->registerEvent('B');
}

void BATTERY_MGR::actOnBatLevel(float batLevel)
{
	if(!chargeState)	//not charging
	{
		if(batLevel<=6.40)
		{
			if(startCharging())
				return;
			if(!lowBattery && batLevel<=6.20)
			{
				#ifndef disable_debug
					_NSerial->println("LOW BAT");
				#endif
				reportedLowBattery=false;
				lowBattery=true;
			}
			else if(lowBattery && batLevel>=6.35)
			{
				lowBattery=false;
				reportedLowBattery=false;
				#ifndef disable_debug
					_NSerial->println("BAT>=6.35");
				#endif
			}
		}
	}
	else	//charging
	{
		if(lowBattery)
		{
			lowBattery=false;
			reportedLowBattery=false;
		}

		if(batLevel>=7.10)
		{
			stopCharging();
		}
	}
}

void BATTERY_MGR::gotACPower()
{
	#ifndef disable_debug
		_NSerial->print("got ");
		_NSerial->println("AC power");
	#endif
	if(initiateCharging && !chargeState)
	{
		startCharging();
	}
}

void BATTERY_MGR::lostACPower()
{
	#ifndef disable_debug
	_NSerial->print("lost ");
	_NSerial->println("AC power");
	#endif
	
	if(chargeState)	
	{
		stopCharging();
		initiateCharging=true;
	}
}

void BATTERY_MGR::update()
{
	if(lowBattery && !reportedLowBattery)
		reportLowBattery();

	if(readingElligible())
		actOnBatLevel(detectBatLevel());
}