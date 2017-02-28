#include "Motor_MGR.h"

/*
simEventTemp[0] 	: _motor not started							N
simEventTemp[1] 	: _cannot turn off motor due to some problem	P
simEventTemp[2]		: _motor has turned off due to unknown reason.	U
simEventTemp[3]		: _single phasing has occured					F
simEventTemp[4]		: _AC power On									G
simEventTemp[5]		: _AC power off									L
simEventTemp[6]		: _motor has turned off due to power cut off.	C
simEventTemp[7]		: _motor has started  							S
simEventTemp[8]		: _motor has turned off							O
*/

#ifndef disable_debug
  #ifdef software_SIM
    Motor_MGR::Motor_MGR(HardwareSerial *serial,SIM* sim1,S_EEPROM* eeprom1,BATTERY_MGR* battery1)
    {
      	_NSerial = serial;
      	_NSerial->begin(19200);
		anotherConstructor(sim1,eeprom1,battery1);
    }
  #else
    Motor_MGR::Motor_MGR(SoftwareSerial *serial,SIM* sim1,S_EEPROM* eeprom1,BATTERY_MGR* battery1)
    {
		_NSerial = serial;
		_NSerial->begin(19200);
		anotherConstructor(sim1,eeprom1,battery1);
    }
  #endif
#else
  Motor_MGR::Motor_MGR(SIM* sim1,S_EEPROM* eeprom1, BATTERY_MGR* battery1)
  {
    anotherConstructor(sim1,eeprom1,battery1);
  }
#endif

void Motor_MGR::anotherConstructor(SIM* sim1,S_EEPROM* eeprom1, BATTERY_MGR* battery1)
{
	this->sim1=sim1;
	this->eeprom1=eeprom1;
	this->battery1=battery1;

	pinMode(PIN_START,OUTPUT);
	digitalWrite(PIN_START,HIGH);
	pinMode(PIN_STOP,OUTPUT);
	digitalWrite(PIN_STOP,HIGH);


	pinMode(PIN_PHASE1,INPUT_PULLUP);
	pinMode(PIN_PHASE2,INPUT_PULLUP);
	pinMode(PIN_ACPHASE,INPUT_PULLUP);

	gotOffCommand=false;
	gotOnCommand=false;
	
	eventOccured=false;
	startTimerOn=false;

	stopTimerTime=600;
	stopTimerOn=false;

	waitCheckACTime=50;
	waitCheckACTimerOn=false;

	singlePhasingTime=100;
	singlePhasingTimerOn=false;

	startSequenceTimerTime=20;
	startSequenceOn=false;

	stopSequenceTimerTime=20;
	stopSequenceOn=false;

	for(byte i=0;i<9;i++)
		simEventTemp[i]=true;

	simEvent[0]='N';
	simEvent[1]='P';
	simEvent[2]='U';
	simEvent[3]='F';
	simEvent[4]='G';
	simEvent[5]='L';
	simEvent[6]='C';
	simEvent[7]='S';
	simEvent[8]='O';	
}


bool Motor_MGR::getMotorState()
{
	bool p1,p2,p3;
	readSensorState(p1,p2,p3);
	
	eeprom1->ACPowerState(p3);
	if(p3 && !phaseAC)
		battery1->gotACPower();
	else if(!p3 && phaseAC)
		battery1->lostACPower();

	eeprom1->motorState((p1 || p2) && p3);

	updateSensorState(p1,p2,p3);
	return eeprom1->motorState();
}

void Motor_MGR::readSensorState(bool &p1,bool &p2,bool &p3)
{
	eventOccured=false;
	noInterrupts();
	p1=!digitalRead(PIN_PHASE1);
	p2=!digitalRead(PIN_PHASE2);
	p3=!digitalRead(PIN_ACPHASE);
	interrupts();

	#ifndef disable_debug
		_NSerial->print("P1:");
		_NSerial->println(p1);
		_NSerial->print("P2:");
		_NSerial->println(p2);
		_NSerial->print("PAC:");
		_NSerial->println(p3);
	#endif
}

void Motor_MGR::updateSensorState(bool &p1,bool &p2,bool &p3)
{
	phase1=p1;
	phase2=p2;
	phaseAC=p3;
}

void Motor_MGR::operateOnEvent()
{
	bool tp1,tp2,tac;
	readSensorState(tp1,tp2,tac);

	if(eeprom1->motorState())		//motorOn
	{
		if(!tac && !tp1 && !tp2)	//acPower Cut Off
		{
			waitCheckACTimerOn=false;		//stop any unknown reason of motor off event
			stopMotor();
			simEventTemp[6]=sim1->registerEvent('C');
			//report To SIM Motor Off due to POWER CUT OFF
			setACPowerState(false);
		}
		else if((tac && phaseAC) &&
			(!tp1 && phase1)  && 
			(!tp2 && phase2))		//motor switched off due to any reason
		{
			tempWaitCheckACTimer=millis();
			waitCheckACTimerOn=true;			//report to SIM Motor Off due to Unknown Reason
			// simEventTemp[2]=sim1->registerEvent('U');
			// stopMotor();
		}
		else if(!tac && phaseAC || 			
			(!tp1 && phase1) || 
			(!tp2 && phase2))	////single phasing occured
		{
			tempSinglePhasingTimer=millis();
			singlePhasingTimerOn=true;
			if(!tac && phaseAC)			// if AC phase got off then
			{
				setACPowerState(false);
			}
		}
		else if(tac && tp1 && tp2 && 
			((tac && !phaseAC) || 
			(tp1 && !phase1)	|| 
			(tp2 && !phase2))) 	//single Phasing had occured Previously but all phase are present now.
		{
			singlePhasingTimerOn=false;  //discardSinglePhasing();
			if(tac && !phaseAC)			// if AC power got back then
			{
				setACPowerState(true);
			}
		}
	}
	else				//motorOff
	{	
		if((tac && (!phaseAC || phaseAC)) && tp1 && tp2)		// motor turn on manually
		{
			eeprom1->motorState(true);	
			simEventTemp[7] = sim1->registerEvent('S');// ;//register To SIM Motor has started
		}
		else if(tac && !phaseAC)					//Got AC Power
		{
			setACPowerState(true);
				#ifndef disable_debug
					_NSerial->print("Auto Start:");
					_NSerial->println(eeprom1->AUTOSTART);
					_NSerial->print("TIME:");
					_NSerial->println(eeprom1->AUTOSTARTTIME);
				#endif
			if(eeprom1->AUTOSTART)			//startMotor if autoStart=ON
			{
				if(eeprom1->AUTOSTARTTIME==0)
					startMotor();
				else
				{
					startTimerOn=true;
					tempStartTimer=millis();
				}
			}
			else
			{
				if(!eeprom1->DND)	//if DND off then
					simEventTemp[4]= sim1->registerEvent('G');
				// ;//register TO SIM AC power ON				
			}
				
		}
		else if(!tac && phaseAC) 			//lostACPower();
		{
			setACPowerState(false);
			if(!eeprom1->DND)		//if DND off
				simEventTemp[5]= sim1->registerEvent('L');
			//register To SIM AC Power OFF
		}
	}
	updateSensorState(tp1,tp2,tac);
}


bool Motor_MGR::startMotorTimerOver()
{
	return (millis()-tempStartTimer>=(((unsigned long int)eeprom1->AUTOSTARTTIME*1000)));
}

bool Motor_MGR::stopMotorTimerOver()
{
	return (millis()-tempStopTimer>=(stopTimerTime*100));
}

void Motor_MGR::setACPowerState(bool ACPowerState)
{
	if(ACPowerState)
	{
		eeprom1->ACPowerState(true);
		battery1->gotACPower();
	}
	else
	{
		eeprom1->ACPowerState(false);
		battery1->lostACPower();
	}
}

bool Motor_MGR::waitCheckACTimerOver()
{
	return (waitCheckACTimerOn && (millis()-tempWaitCheckACTimer>(waitCheckACTime*100)));
}

void Motor_MGR::unknownMotorOff()
{
	waitCheckACTimerOn=false;
	//report to SIM Motor Off due to Unknown Reason
	stopMotor();
	simEventTemp[2]=sim1->registerEvent('U');
}

bool Motor_MGR::singlePhasingTimerOver()
{
	return (singlePhasingTimerOn && millis()-tempSinglePhasingTimer>(singlePhasingTime*100));
}

void Motor_MGR::operateOnSinglePhasing()
{
	// stopMotor();

	stopMotor();
	simEventTemp[3] = sim1->registerEvent('F');
	//reportSinglePhasing TO SIM..
}

void Motor_MGR::startMotor(bool commanded)
{
	#ifndef disable_debug
		_NSerial->print("Start");
		_NSerial->println("Motor ");
	#endif
	if(!eeprom1->motorState())
	{
		startTimerOn=false;
		digitalWrite(PIN_START,LOW);
		tempStartSequenceTimer=millis();
		startSequenceOn=true;
		eeprom1->motorState(true);
		gotOnCommand=commanded;
	}
	else
	{
		if(commanded)
			sim1->setMotorMGRResponse('O');
		#ifndef disable_debug
			_NSerial->print("Motor ");
			_NSerial->println("ON");
		#endif
	}
}
	
void Motor_MGR::stopMotor(bool commanded)
{
	#ifndef disable_debug
		_NSerial->print("Stop");
		_NSerial->println("Motor ");
	#endif
	if(eeprom1->motorState())
	{
		singlePhasingTimerOn=false;
		digitalWrite(PIN_STOP,LOW);
		tempStopSequenceTimer=millis();
		startTimerOn=false;
		stopSequenceOn=true;
		eeprom1->motorState(false);
		gotOffCommand=commanded;		
	}
	else
	{
		if(commanded)
			sim1->setMotorMGRResponse('O');
		#ifndef disable_debug
			_NSerial->print("Motor ");
			_NSerial->println("OFF");
		#endif
	}
}

void Motor_MGR::terminateStopRelay()
{
	if(stopSequenceOn && millis() - tempStopSequenceTimer > (stopSequenceTimerTime*100))	
	{
		digitalWrite(PIN_STOP,HIGH);
		stopSequenceOn=false;

		#ifndef disable_debug
			_NSerial->println("Stop Over");
		#endif
		bool t= getMotorState();
		if(!t)		//motor has turned off
		{
			if(gotOffCommand)
			{
				gotOffCommand=false;
				sim1->setMotorMGRResponse('D');		//motor has stopped
			}
				// else
				// simEventTemp[8] = sim1->registerEvent('O'); //register TO SIM motor has turned off
		}	
		else	//motor is still on, with either phase 1 or phase 2 feedback on with ACPhase on
		{
			if(gotOffCommand)
			{
				gotOffCommand=false;
				sim1->setMotorMGRResponse('L');		//cannot turn off motor
			}	
			else
				simEventTemp[1] = sim1->registerEvent('P');
			//register to SIM cannot turn off motor due to some problem
		}
	}
}

void Motor_MGR::terminateStartRelay()
{
	if(startSequenceOn &&  millis() - tempStartSequenceTimer > (startSequenceTimerTime*100))
	{
		digitalWrite(PIN_START,HIGH);
		startSequenceOn=false;

		#ifndef disable_debug
			_NSerial->println("Start Over");
		#endif

		if(gotOnCommand)
		{
			gotOnCommand=false;
			if(getMotorState())
			{
				sim1->setMotorMGRResponse('D'); // motor has started
			}
			else
			{
				stopMotor();
				sim1->setMotorMGRResponse('L');	//cannot start motor deu to some problem
			}
		}
		else
		{
			if(getMotorState())
				simEventTemp[7] = sim1->registerEvent('S');// ;//register To SIM Motor has started
			else
			{
				stopMotor();
				simEventTemp[0] = sim1->registerEvent('N');//register To SIM motor not started due to phase failure
			}			
		}
	}
}

void Motor_MGR::statusOnCall()
{
	bool temp = getMotorState();
	if(temp)
		sim1->setMotorMGRResponse('D');	//motor is on
	else
	{
		if(!phaseAC)
			sim1->setMotorMGRResponse('L');	//motor off, no light
		else
			sim1->setMotorMGRResponse('O');	//motor off, light on
	}
}

void Motor_MGR::SIMEventManager()
{
	for(byte i=0;i<9;i++)
	{
		if(!simEventTemp[i])
			simEventTemp[i]= sim1->registerEvent(simEvent[i]);
	}
}
	// if(!simEventTemp[0])
	// 	simEventTemp[0] = sim1->registerEvent('N');

	// if(!simEventTemp[1])
	// 	simEventTemp[1] = sim1->registerEvent('P');

	// if(!simEventTemp[2])
	// 	simEventTemp[2]=sim1->registerEvent('U');
	
	// if(!simEventTemp[3])
	// 	simEventTemp[3] = sim1->registerEvent('F');

	// if(!simEventTemp[4] && !eeprom1->DND)
	// 	simEventTemp[4]= sim1->registerEvent('G');

	// if(!simEventTemp[5] && !eeprom1->DND)
	// 	simEventTemp[5]= sim1->registerEvent('L');

	// if(!simEventTemp[6])
	// 	simEventTemp[6]=sim1->registerEvent('C');

	// if(!simEventTemp[7])
	// 	simEventTemp[7] = sim1->registerEvent('S');

	// if(!simEventTemp[8])
	// 	simEventTemp[8] = sim1->registerEvent('O');

void Motor_MGR::update()
{
	if(!startSequenceOn && !stopSequenceOn && eventOccured)
		operateOnEvent();

	if(waitCheckACTimerOn)
		if(waitCheckACTimerOver())
			unknownMotorOff();	

	if(singlePhasingTimerOn)
		if(singlePhasingTimerOver())
			operateOnSinglePhasing();

	if(startTimerOn)
		if(startMotorTimerOver())
			startMotor();
	
	if(stopTimerOn)
		if(stopMotorTimerOver())
			stopMotor();

	if(startSequenceOn)
		terminateStartRelay();

	if(stopSequenceOn)
		terminateStopRelay();

	SIMEventManager();
}
