#include "Motor_MGR.h"

/*
  simEventTemp[0] 		: _motor not started							N
  simEventTemp[1] 		: _cannot turn off motor due to some problem	P
  simEventTemp[2]		: _motor has turned off due to unknown reason.	U
  simEventTemp[3]		: _single phasing has occured so motor off		F
  simEventTemp[4]		: _AC power On									G
  simEventTemp[5]		: _AC power off									L
  simEventTemp[6]		: _motor has turned off due to power cut off.	C
  simEventTemp[7]		: _motor has started  							S
  simEventTemp[8]		: _motor has turned off							O
  simEventTemp[9]		: lost AC power in 1 phase						A
*/

#ifndef disable_debug
	#ifdef software_SIM
		Motor_MGR::Motor_MGR(HardwareSerial *serial, SIM* sim1, S_EEPROM* eeprom1)
		{
			_NSerial = serial;
			_NSerial->begin(19200);
			anotherConstructor(sim1, eeprom1);
		}
	#else
		Motor_MGR::Motor_MGR(SoftwareSerial *serial, SIM* sim1, S_EEPROM* eeprom1)
		{
			_NSerial = serial;
			_NSerial->begin(19200);
			anotherConstructor(sim1, eeprom1);
		}
	#endif
#else
	Motor_MGR::Motor_MGR(SIM* sim1, S_EEPROM* eeprom1)
	{
		anotherConstructor(sim1, eeprom1);
	}
#endif

void Motor_MGR::anotherConstructor(SIM* sim1, S_EEPROM* eeprom1)
{
  this->sim1 = sim1;
  this->eeprom1 = eeprom1;

  pinMode(PIN_MSTART, OUTPUT);
  digitalWrite(PIN_MSTART, HIGH);
  pinMode(PIN_MSTOP, OUTPUT);
  digitalWrite(PIN_MSTOP, HIGH);					// to connect stop Relay output to the NC , relay is OFF

  pinMode(PIN_3PHASE, INPUT_PULLUP);
  pinMode(PIN_MFEEDBACK, INPUT_PULLUP);
  // pinMode(PIN_ACFEEDBACK, INPUT_PULLUP);
  pinMode(PIN_ACPHASE, INPUT);

  pinMode(PIN_STARTBUTTON,INPUT_PULLUP);
  pinMode(PIN_STOPBUTTON,INPUT_PULLUP);

  #if ! defined(ENABLE_GP) && ! defined(ENABLE_CURRENT)  
  	pinMode(PIN_AUTOBUTTON,INPUT_PULLUP);
  #endif

  #ifdef ENABLE_CURRENT
  		pinMode(PIN_CURRENT,INPUT);
  #endif

  pinMode(PIN_MOTORLED,OUTPUT);

  gotOffCommand = false;
  gotOnCommand = false;

  eventOccured = false;
  startTimerOn = false;

  // stopTimerTime = 600;
  // stopTimerOn = false;

  // waitCheckACTime = 20;
  // waitCheckACTimerOn = false;

  singlePhasingTime = 15;
  singlePhasingTimerOn = false;

  startSequenceTimerTime = 20;
  starDeltaTimerOn=false;
  startSequenceOn = false;

  stopSequenceTimerTime = 20;
  stopSequenceOn = false;

  waitStableLineTime = 20;
  waitStableLineOn = false;


  AllPhaseState(false); // allPhase = false;
  motorState(false);// mFeedback = false;
  ACPowerState(false);//  phaseAC = false;

  #ifdef ENABLE_WATER
  	setWaterDefaults();
  #endif
  	

lastPressTime=0;
lastButtonEvent=0;



	#ifdef ENABLE_CURRENT
		byte i=14;
	#else
		byte i=12;
	#endif
	while(i--)
	{
		simEventTemp[i] = true;
	}

  simEvent[0] = 'N';
  simEvent[1] = 'P';
  simEvent[2] = 'U';
  simEvent[3] = 'F';
  simEvent[4] = 'G';
  simEvent[5] = 'L';
  simEvent[6] = 'C';
  simEvent[7] = 'S';
  simEvent[8] = 'O';
  simEvent[9] = 'A';

  simEvent[10] = '8';		//AUTO ON EVENT 
  simEvent[11] = '9';		//AUTO OFF EVENT


  #ifdef ENABLE_CURRENT
  simEvent[12] = 'B';		//Overload Event 
  simEvent[13] = 'J';		//Underload EVENT
  #endif
  
  // resetAutoStart();
}

#ifdef ENABLE_WATER

#ifdef ENABLE_GP
void Motor_MGR::readOverHeadWaterSensorState(bool &olow,bool &ohigh)
{
  
  	noInterrupts();
  	olow = digitalRead(PIN_OLOWSENSOR);
	ohigh = digitalRead(PIN_OHIGHSENSOR);
	interrupts();
}

void Motor_MGR::updateOverheadLevel(byte level)
{
	overheadLevel=level;
}

byte Motor_MGR::getOverHeadWaterSensorState()
{
	bool olow,ohigh;
	readOverHeadWaterSensorState(olow,ohigh);
	byte ans=0;
	if(!olow)
	{
		ans++;
		if(!ohigh)
		{
			ans++;
		}
	}
	return ans;
}

void Motor_MGR::overHeadWaterStatusOnCall(bool current)
{
	byte temp = getOverHeadWaterSensorState();
	if(current)
		temp = overheadLevel;
	if(temp == OVERHEADHIGHLEVEL)
	{
		sim1->setMotorMGRResponse('V');
	}
	else if(temp == OVERHEADMIDLEVEL)
	{
		sim1->setMotorMGRResponse('X');
	}
	else if (temp == OVERHEADCRITICALLEVEL)
	{
		sim1->setMotorMGRResponse('W');
	}
}
#endif

void Motor_MGR::setWaterDefaults()
{
	pinMode(PIN_LOWSENSOR,INPUT_PULLUP);
	pinMode(PIN_MIDSENSOR,INPUT_PULLUP);
	pinMode(PIN_HIGHSENSOR,INPUT_PULLUP);

	#ifdef ENABLE_GP
		pinMode(PIN_OLOWSENSOR,INPUT_PULLUP);
		pinMode(PIN_OHIGHSENSOR,INPUT_PULLUP);
		overheadLevel = OVERHEADMIDLEVEL;
		tempOverheadLevel = OVERHEADMIDLEVEL;
		// overHeadLowSensorState(false);
		// overHeadHighSensorState(true);
	#endif

	// waterEventBufferTime=150; 		// 200x100= 20,000ms = 20s
	// noInterrupts();
	// waterEventOccured=false;
	// interrupts();

	undergroundLevel = MIDLEVEL;
	tempUndergroundLevel = MIDLEVEL;
	// lowSensorState(false);
	// midSensorState(false);
	// highSensorState(true);

	#ifdef ENABLE_GP
		byte j=19;
	#else
		byte j=17;
	#endif
  for (byte i = 12; i < j; i++)
	simEventTemp[i] = true;

  simEvent[12] = 'I';
  simEvent[13] = 'D';
  simEvent[14] = 'H';
  simEvent[15] = 'E';
  simEvent[16] = 'Z';

  #ifdef ENABLE_GP
	simEvent[17] = 'V';
	simEvent[18] = 'W';
  #endif

	#ifdef ENABLE_M2M
	m2mEvent[0] = ME_CLEARED;
	m2mEvent[1] = ME_CLEARED;
	
	mapTable[0] = 13;
	mapTable[1] = 15;
	#endif
}

void Motor_MGR::readWaterSensorState(bool &low,bool &mid,bool &high)
{
	// waterEventOccured = false;
  	noInterrupts();
  	low = digitalRead(PIN_LOWSENSOR);
	mid = digitalRead(PIN_MIDSENSOR);
	high = digitalRead(PIN_HIGHSENSOR);
	interrupts();

#ifndef disable_debug
  _NSerial->print("LS:");
  _NSerial->println(low);
  _NSerial->print("MS:");
  _NSerial->println(mid);
  _NSerial->print("HS:");
  _NSerial->println(high);
#endif
}

void Motor_MGR::updateUndergroundLevel(byte level)
{
	undergroundLevel=level;
}

// void Motor_MGR::updateWaterSensorState(bool &low,bool &mid,bool &high)
// {
// 	lowSensorState(low);
// 	midSensorState(mid);
// 	highSensorState(high);
// }

byte Motor_MGR::getWaterSensorState()
{
	bool l,m,h;
	readWaterSensorState(l,m,h);
	byte ans=0;

	if(!l) 
	{
		ans++;
		if(!m) 
		{
			ans++;
			if(!h)
			{
				ans++;				
			}
		}
	}
	return ans;
}

void Motor_MGR::waterStatusOnCall(bool current)
{
	byte temp = getWaterSensorState();
	if(current)
		temp = undergroundLevel;

	if(temp==CRITICALLEVEL)
	{
	  sim1->setMotorMGRResponse('T');	//water level insufficient
	}
	else if(temp==LOWLEVEL)
	{
	  sim1->setMotorMGRResponse('Q');	//water below 2nd sensor
	}
	else if(temp==MIDLEVEL)
	{
	  sim1->setMotorMGRResponse('R');	//water below 1st sensor
	}
	else if(temp==HIGHLEVEL)
	{
	  sim1->setMotorMGRResponse('E');	//well is full
	}
}

bool Motor_MGR::checkWater()
{
	bool result=false;

	byte uLevel = getWaterSensorState();
	#ifdef ENABLE_GP
		byte oLevel =getOverHeadWaterSensorState();
		if((uLevel!=undergroundLevel && uLevel==tempUndergroundLevel) || (oLevel!=overheadLevel && oLevel==tempOverheadLevel))
	#else
		if(uLevel!=undergroundLevel && uLevel==tempUndergroundLevel)
	#endif
	{
			operateOnWaterEvent();
			result=true;
			// undergroundLevel=uLevel;			// it will be done by operateOnWaterEvent()
	}
	tempUndergroundLevel=uLevel;
	#ifdef ENABLE_GP
	tempOverheadLevel=oLevel;
	#endif
	return result;
}

void Motor_MGR::operateOnWaterEvent()
{
	// if(millis()-tempWaterEventTime<(15000L))
	// {
	// 	return;
	// }	
	// noInterrupts();
		// waterEventOccured=false;
	// interrupts();
	
	// byte uLevel = getWaterSensorState();
	byte uLevel = tempUndergroundLevel;
	// bool low,mid,high;
	// readWaterSensorState(low,mid,high);

	#ifdef ENABLE_GP
		byte oLevel = tempOverheadLevel;
		// byte oLevel = getOverHeadWaterSensorState();
		// bool olow,ohigh;
		// readOverHeadWaterSensorState(olow,ohigh);
	#endif

	if(uLevel == undergroundLevel)
	{
		#ifndef ENABLE_GP
			return;
		#else
			if(oLevel==overheadLevel)
				return;
		#endif 
	}


	if(motorState())
	{
		if(uLevel==CRITICALLEVEL && undergroundLevel>CRITICALLEVEL)	//no water in well
		{
  			stopMotor(false,true);
			simEventTemp[12] = sim1->registerEvent('I'); //report To SIM Motor Off due to insufficient water level
	
			#ifdef ENABLE_M2M	
			if(eeprom1->M2M && undergroundLevel>LOWLEVEL)
			{
	  			m2mEvent[0] = ME_WAITREGISTER;
			}
			#endif
		}
		else if (uLevel==HIGHLEVEL && undergroundLevel<HIGHLEVEL)		// well is full
		{
  			#ifdef ENABLE_M2M
			if(eeprom1->M2M)
			{
	  			m2mEvent[1] = ME_WAITREGISTER;
			}
			else
			{
				simEventTemp[15] = sim1->registerEvent('E'); //report To SIM well is full.
			}
  			#else
	  		if(eeprom1->PREVENTOVERFLOW)
	  		{
	  			stopMotor(false,true);
				simEventTemp[14] = sim1->registerEvent('H'); //report To SIM well is full, so stopped motor
	  		}
	  		else
	  		{
				simEventTemp[15] = sim1->registerEvent('E'); //report To SIM well is full.
	  		}
  			#endif
		}
		else if (uLevel==LOWLEVEL && undergroundLevel>LOWLEVEL)		// decrease in water level
		{
			#ifdef ENABLE_M2M
			if(eeprom1->M2M)
			{
  				m2mEvent[0] = ME_WAITREGISTER;
			}
			else
			{
				simEventTemp[13] = sim1->registerEvent('D'); //report To SIM water level is decrease..
			}
  			#else
			simEventTemp[13] = sim1->registerEvent('D'); //report To SIM water level is decrease..
			#endif
		}
		else if (uLevel==MIDLEVEL && undergroundLevel<MIDLEVEL)		// increase in water level
		{
			simEventTemp[16] = sim1->registerEvent('Z'); //report To SIM water level is increasing..
		}

  	#ifdef ENABLE_GP
	  	if(oLevel!=overheadLevel)
	  	{
	  		if(oLevel==OVERHEADHIGHLEVEL && overheadLevel<OVERHEADHIGHLEVEL)		////overhead tank is full
	  		{
	  			stopMotor(false,true);
				simEventTemp[17] = sim1->registerEvent('V'); //report To SIM Motor Off due to overhead tank full
	  		}
	  		else if (oLevel==OVERHEADCRITICALLEVEL && overheadLevel>OVERHEADCRITICALLEVEL)	// overhead tank empty.
	  		{
				simEventTemp[18] = sim1->registerEvent('W'); //report To SIM , overhead tank empty.
	  		}
	  	}
  	#endif
	}
	else 		// motoroff
	{
		#ifdef ENABLE_M2M
		if(eeprom1->M2M && uLevel<MIDLEVEL && undergroundLevel>=MIDLEVEL)	//level is decreased 
		{
	  		m2mEvent[0] = ME_WAITREGISTER;
		}
		else if (uLevel==HIGHLEVEL && undergroundLevel<HIGHLEVEL)			//well is full
		{
				if(eeprom1->M2M)
				{
		  			m2mEvent[1] = ME_WAITREGISTER;
				}
				else
				{
					simEventTemp[15] = sim1->registerEvent('E'); //report To SIM well is full.
				}
		}
		#else
		if(uLevel==HIGHLEVEL && undergroundLevel<HIGHLEVEL)		//well is full
		{
			simEventTemp[15] = sim1->registerEvent('E'); //report To SIM well is full.
		}
		#endif
		else if (uLevel==MIDLEVEL && undergroundLevel<MIDLEVEL)		// underground level is increasing
		{
	  		#ifdef ENABLE_GP
	  			if(oLevel<OVERHEADHIGHLEVEL)
	  			{
			  		if(eeprom1->AUTOSTART)			//autoStart is ON
				  		triggerAutoStart();
	  			}
	  		#else
			  	if(eeprom1->AUTOSTART)			//autoStart is ON
				  	triggerAutoStart(); 
	 		#endif
		}

  	#ifdef ENABLE_GP
  		if (oLevel==OVERHEADCRITICALLEVEL && overheadLevel>OVERHEADCRITICALLEVEL && uLevel>CRITICALLEVEL) // overhead tank is empty, and underground not low
  		{
			  	if(eeprom1->AUTOSTART)			//autoStart is ON
				{
				 	triggerAutoStart();
				}
				else
				{
					simEventTemp[18] = sim1->registerEvent('W'); //report To SIM overhead tank is empty.
				}
  		}
  	#endif
	}

  #ifdef ENABLE_GP
	updateOverheadLevel(oLevel);
  #endif
	updateUndergroundLevel(uLevel);
}
#endif

#ifdef ENABLE_CURRENT
void Motor_MGR::autoSetCurrent()
{
	if(motorState() && !startSequenceOn && !starDeltaTimerOn && !stopSequenceOn && AllPhaseState())
	{
		unsigned short int temp = analogRead(PIN_CURRENT);
		if(temp<160)
		{
			eeprom1->setCurrentDetection(false);
			sim1->setMotorMGRResponse('Y');		//ampere cleared
			return;
		}
		unsigned short int tempUnder = temp * (float)eeprom1->UNDERLOADPER / 100.0;
		unsigned short int tempOver = temp * (float)eeprom1->OVERLOADPER / 100.0;
		
		// temp = temp * (float)eeprom1->OVERLOADPER /100.0;
		if(tempOver>1022 || tempUnder < 128)
		{
			sim1->setMotorMGRResponse('7');		//change ampere jumper
			return;
		}
		else
		{
			eeprom1->setNormalLoadValue(temp);
			eeprom1->setUnderloadValue(tempUnder);
			eeprom1->setOverloadValue(tempOver);
			
			eeprom1->setCurrentDetection(true);
			sim1->setMotorMGRResponse('K');		//ampere settings complete
		}
	}
	else
	{
		eeprom1->setCurrentDetection(false);
		sim1->setMotorMGRResponse('Y');		//ampere cleared
	}
}

unsigned short int Motor_MGR::getCurrentConsumed()
{
	return (analogRead(PIN_CURRENT));
}

void Motor_MGR::checkCurrentConsumption()
{
	if(!motorState() || !eeprom1->CURRENTDETECTION || starDeltaTimerOn
		|| millis()-lastCurrentReadingTime<500)
		return;

	// if(enableCurrentBuffer)
	// {
		if(enableCurrentBuffer && millis()-tempStartSequenceTimer>30000)
			enableCurrentBuffer=false;
		// return;
	// }

	lastCurrentReadingTime=millis();
	unsigned short int temp = analogRead(PIN_CURRENT);

	
	unsigned short int overLoadDetectValue=10000;
	byte temp2;
// implement bufffer of previous input.. and check 2 previous inputs for deciding overload or underload
	if(enableCurrentBuffer && temp>(eeprom1->NORMALVALUE*2))
	{
		temp2 = CR_OVER2;
		overLoadDetectValue=18000;
	}
	else if(!enableCurrentBuffer && temp>(eeprom1->NORMALVALUE*2))
	{
		temp2 = CR_OVER;
		overLoadDetectValue=4000;
	}
	else if(!enableCurrentBuffer && temp> (float)eeprom1->NORMALVALUE*(1.5))
	{
		temp2 = CR_OVER;
		overLoadDetectValue=7500;
	}
	else if (!enableCurrentBuffer && temp>eeprom1->OVERLOADVALUE)
	{
		temp2 = CR_OVER;
	}
	else if(temp < eeprom1->UNDERLOADVALUE && !enableCurrentBuffer)		// only consider noLoad after 30 secs
	{
		temp2 = CR_UNDER;
	}
	else 
	{
		temp2= CR_NORMAL;
	}

	if(lastCurrentReading == temp2)
	{
		if(millis()-currentEventFilterTempTime>overLoadDetectValue)
		{
			if(temp2==CR_OVER)
			{
				stopMotor(false,true);
				simEventTemp[12] = sim1->registerEvent('B');			//register overload Event
			}
			else if(temp2==CR_UNDER)
			{
				stopMotor(false,true);
				simEventTemp[13] = sim1->registerEvent('J');			// register Underload Event
			}
		}
	}
	else
	{
		currentEventFilterTempTime = millis();
		lastCurrentReading=temp2;
	}
}
#endif
 
bool Motor_MGR::getMotorState()
{
  bool p1, p2, p3;
  readSensorState(p1, p2, p3);
  motorState(p2);
  // updateSensorState(p1, p2, p3);
  return p2;
}

void Motor_MGR::readSensorState(bool &p1, bool &p2, bool &p3)
{
  noInterrupts();
  p1 = digitalRead(PIN_3PHASE);
  p2 = !digitalRead(PIN_MFEEDBACK);
  p3 = digitalRead(PIN_ACPHASE);
  interrupts();

#ifndef disable_debug
  _NSerial->print("3P:");
  _NSerial->println(p1);
  _NSerial->print("MF:");
  _NSerial->println(p2);
  // _NSerial->print("ACF:");
  // _NSerial->println(p3);
  _NSerial->print("PAC:");
  _NSerial->println(p4);
#endif
}

void Motor_MGR::updateSensorState(bool &p1, bool &p2, bool &p3)
{
  AllPhaseState(p1); // allPhase = p1;
  motorState(p2); // mFeedback = p2;
  ACPowerState(p3); // phaseAC = p4;
  
  setLED(AllPhaseState() && ACPowerState());

  if (!ACPowerState() || !AllPhaseState())// if (!phaseAC || !allPhase)
	startTimerOn = false;

  if (!(bool)eeprom1->AUTOSTART)
  {
	if (ACPowerState())
	{
		#ifndef disable_debug
	  	_NSerial->println(F("A ON"));
		#endif
	  if (!motorState())// if (!mFeedback)
	  {
		#ifndef disable_debug
		_NSerial->println(F("M OFF"));
		#endif
		digitalWrite(PIN_MSTOP,HIGH);
	  }
	}
	else
	{
		#ifndef disable_debug
	  _NSerial->println(F("B ON"));
		#endif
	  digitalWrite(PIN_MSTOP, HIGH);
	}
  }
  else 					//AUtoStart is On
  {
  	if(ACPowerState())	// on AC Power,
  	{
  		#ifndef ENABLE_GP
			digitalWrite(PIN_AUTOLED,HIGH); 	//AUTO LED is turned on.
  		#endif
		if (AllPhaseState() && !stopSequenceOn)				//auto start is on , and AC is Present in 3 phase, so , can start with switch or external force.
		{
			digitalWrite(PIN_MSTOP,LOW);
		}
  	}
	else  			// on battery, so stop AUTO LED to save power
	{
		#ifndef ENABLE_GP
			digitalWrite(PIN_AUTOLED,LOW);
  		#endif
	}
  }
}

void Motor_MGR::resetAutoStart(bool setChange)
{
  if (!(bool)eeprom1->AUTOSTART && !motorState() && ACPowerState())
  {
  	digitalWrite(PIN_MSTOP, HIGH);
  	startTimerOn=false;
  }
  else if ((bool)eeprom1->AUTOSTART && !stopSequenceOn && ACPowerState())
  {
	digitalWrite(PIN_MSTOP, LOW);
	if (setChange) // && eeprom1->ACPowerState() && !eeprom1->motorState())
	{
	  triggerAutoStart();
	}
  }
}

void Motor_MGR::triggerAutoStart()
{
  if (!motorState())
  {
	if (AllPhaseState() && ACPowerState())
	{
	  startTimerOn = true;
	  tempStartTimer = millis();
	}
  }
}

bool Motor_MGR::motorState()
{
  return mFeedback;
}

inline void Motor_MGR::motorState(bool b)
{
  mFeedback = b;
   if(b)
  	digitalWrite(PIN_MOTORLED,HIGH);
  else
  	digitalWrite(PIN_MOTORLED,LOW);
}

bool Motor_MGR::ACPowerState()
{
  return phaseAC;
}

inline void Motor_MGR::ACPowerState(bool b)
{
  phaseAC = b;
}

bool Motor_MGR::AllPhaseState()
{
	if(eeprom1->BYPASS)
	{
		if(!allPhase || ACPowerState())
			return true;
		else
			return false;
	}
	else
	{
  		return allPhase;
	}
}

inline void Motor_MGR::AllPhaseState(bool b)
{
  allPhase = b;
}

void Motor_MGR::operateOnEvent()
{
  bool t3Phase, tMotor, tacPhase;
  readSensorState(t3Phase, tMotor, tacPhase);
 	noInterrupts();
  eventOccured = false;
  	interrupts();

  if((t3Phase == AllPhaseState()) && (tMotor == motorState()) && (tacPhase == ACPowerState()))
  	return;

  if (motorState())		//motorOn
  {
	if (t3Phase && !tMotor && !tacPhase)	//acPower Cut Off
	{
		// waitCheckACTimerOn = false;		//stop any unknown reason of motor off event
		stopMotor(false,true);
		setLED(TURN_OFF);
		simEventTemp[6] = sim1->registerEvent('C'); //report To SIM Motor Off due to POWER CUT OFF
		// _NSerial->println("Got C");
	}
	else if ((tacPhase && ACPowerState()) &&
			 (t3Phase && AllPhaseState()) &&
			 (!tMotor))		//motor switched off due to any reason
	{
		unknownMotorOff();
		// tempWaitCheckACTimer = millis();
	 	// waitCheckACTimerOn = true;
	#ifndef disable_debug
		  _NSerial->print("Got");
		  _NSerial->println("U");
	#endif
	  //report to SIM Motor Off due to Unknown Reason
	  // simEventTemp[2]=sim1->registerEvent('U');
	  // stopMotor();
	}
	else if (!t3Phase && tacPhase) ////single phasing occured
	{
	  	// operateOnSinglePhasing();
	  	if(!eeprom1->BYPASS)
	  	{
		  	tempSinglePhasingTimer = millis();
		  	singlePhasingTimerOn = true;
#ifndef disable_debug
		  	_NSerial->print("Got");
		  	_NSerial->println("F");
#endif
	  	}
	}
	// else if (!mFeedback && (!tacPhase && ACPowerState()) && (t3Phase && AllPhaseState()))	//2 Phases Got Off
	// {
	  // waitCheckACTimerOn = false;		//stop any unknown reason of motor off event
	  // stopMotor();
	  // simEventTemp[9] = sim1->registerEvent('D');
	  // semiState = true;
	// }
	// No need to check if single phasing had occured, but all phase present now.
  }
  else				//motorOff
  {
	if (tMotor)		// motor turn on manually
	{
	  if (t3Phase && tacPhase)
	  {
		if (startTimerOn)
		  startTimerOn = false;
		setLED(TURN_ON);
		simEventTemp[7] = sim1->registerEvent('S');	//register To SIM Motor has started
	  }
	  else
	  {
		stopMotor(false,true);
	  }
	}
	else	// changes in AC lines, i.e. got power in 3 phase, 2phase or 1 phase, or lost ACpower.
	{
	  waitStableLineOn = true;
	  waitStableLineTimer = millis();
	}
  }
  updateSensorState(t3Phase, tMotor, tacPhase);
}

byte Motor_MGR::checkLineSensors()
{
#ifndef disable_debug
  _NSerial->println("CLSR");
  _NSerial->print("3P:");
  _NSerial->println(AllPhaseState());
  _NSerial->print("MF:");
  _NSerial->println(motorState());
  _NSerial->print("PAC:");
  _NSerial->println(ACPowerState());
#endif

  if (ACPowerState() && AllPhaseState())		//all phase present
	return AC_3PH;
  // else if ((ACPowerState() || ACFeedbackState())  && !AllPhaseState())	//2 phase present
  else if (!AllPhaseState())	//2 phase present
	return AC_2PH;
  else if (!ACPowerState() && AllPhaseState())	//1 phase or No Phase present
	return AC_OFF;
 //  else if (!ACPowerState() && !AllPhaseState())	//No phase present
	// return AC_OFF;
}

bool Motor_MGR::waitStableLineOver()
{
  return (waitStableLineOn && millis() - waitStableLineTimer >= (waitStableLineTime * 100));
}

void Motor_MGR::operateOnStableLine()
{
  waitStableLineOn = false;

  byte temp = checkLineSensors();
  if (temp == AC_3PH)
  {
  	setLED(TURN_ON);
// #ifndef disable_debug
	// _NSerial->print("AUTO:");
	// _NSerial->println(eeprom1->AUTOSTART);
	// _NSerial->print("TIME:");
	// _NSerial->println(eeprom1->AUTOSTARTTIME);
// #endif
	if (eeprom1->AUTOSTART)			//startMotor if autoStart=ON
	  triggerAutoStart();
	else
	{
	  if (!eeprom1->DND)	//if DND off then
		simEventTemp[4] = sim1->registerEvent('G'); //register TO SIM AC power ON
	}
  }
  else if (temp == AC_2PH) //Got Power in 2 phase
  {
  	setLED(TURN_OFF);  	
	if (!eeprom1->DND)	//if DND off then
	  simEventTemp[9] = sim1->registerEvent('A'); //register TO SIM 2 phase power ON
  }
  else if (temp == AC_OFF)	//Lost Power in All Phase
  {
  	setLED(TURN_OFF);
	if (!eeprom1->DND)		//if DND off
	  simEventTemp[5] = sim1->registerEvent('L'); //register To SIM AC Power OFF
  }
}

bool Motor_MGR::startMotorTimerOver()
{
  return (millis() - tempStartTimer >= (((unsigned long int)eeprom1->AUTOSTARTTIME * 1000)));
}

// bool Motor_MGR::stopMotorTimerOver()
// {
  // return (millis() - tempStopTimer >= (stopTimerTime * 100));
// }

// bool Motor_MGR::waitCheckACTimerOver()
// {
//   return (waitCheckACTimerOn && (millis() - tempWaitCheckACTimer > (waitCheckACTime * 100)));
// }

void Motor_MGR::unknownMotorOff()
{
  // waitCheckACTimerOn = false;
  //report to SIM Motor Off due to Unknown Reason
  stopMotor(false,true);
  simEventTemp[2] = sim1->registerEvent('U');
}

bool Motor_MGR::singlePhasingTimerOver()
{
	// if(digitalRead(PIN_3PHASE)==LOW)
	// {
  		return (singlePhasingTimerOn && millis() - tempSinglePhasingTimer > ((unsigned int)singlePhasingTime * 100));
	// }
	// else
	// {
	// 	singlePhasingTimerOn=false;
	// 	return false;
	// }
}

inline void Motor_MGR::operateOnSinglePhasing()
{
	setLED(TURN_OFF);
  	stopMotor(false,true);
  	simEventTemp[3] = sim1->registerEvent('F');
  //reportSinglePhasing TO SIM..
}

void Motor_MGR::startMotor(bool commanded)
{
	startTimerOn = false;
#ifndef disable_debug
  _NSerial->print("Start");
  // _NSerial->println("Motor ");
#endif
  if (ACPowerState() && AllPhaseState())
  {
	if (!motorState())
	{
	  // if (!(bool)eeprom1->AUTOSTART)
		// stopSequenceOn=false;

	#ifdef ENABLE_WATER
		if(getWaterSensorState()==CRITICALLEVEL)
		{
			if (commanded)
			{
				sim1->setMotorMGRResponse('T');	//cannot start motor due to some problem
			}
			else
			{
				simEventTemp[0] = sim1->registerEvent('N');//register To SIM motor not started due to ANY REASON
			}
			return;
		}

		#ifdef ENABLE_GP
		if(getOverHeadWaterSensorState()==OVERHEADHIGHLEVEL)
		{
				if(commanded)
				{
					sim1->setMotorMGRResponse('V');	//cannot start motor as OverHead Tank Full.
				}
				else
				{
					simEventTemp[17] = sim1->registerEvent('V');//register To SIM motor not started due to ANY REASON
				}
				return;
		}
		#endif
	#endif
	  digitalWrite(PIN_MSTOP, LOW);
	  digitalWrite(PIN_MSTART, LOW);
	  setLED(TURN_ON);
	  tempStartSequenceTimer = millis();
	  startSequenceOn = true;
	  enableCurrentBuffer=false;
	  motorState(true);
  	#ifdef ENABLE_CURRENT
		lastCurrentReading=CR_NORMAL;
  	#endif
	  gotOnCommand = commanded;
	}
	else
	{
	  if (commanded)
		sim1->setMotorMGRResponse('1');		//motor is already on
	}
  }
  else
  {
	if (commanded)
	{
	  sim1->setMotorMGRResponse('N');	//cannot start motor due to some problem
	}
	else
	{
		simEventTemp[0] = sim1->registerEvent('N');//register To SIM motor not started due to ANY REASON
	}
  }
}

void Motor_MGR::stopMotor(bool commanded, bool forceStop,bool offButton)
{
#ifndef disable_debug
  _NSerial->print("Stop");
  _NSerial->println("M ");
#endif
  if (forceStop || motorState())
  {
	// startSequenceOn=false;
	// startTimerOn = false;
	// starDeltaTimerOn=false;
	// digitalWrite(PIN_MSTART, HIGH);

	singlePhasingTimerOn = false;
	digitalWrite(PIN_MSTOP, HIGH);
	tempStopSequenceTimer = millis();
	stopSequenceOn = true;
	motorState(false);
	gotOffCommand = commanded;
	offButtonPressed=offButton;
	#ifdef ENABLE_CURRENT
		lastCurrentReading=CR_NORMAL;			//to make the current readings normal
	#endif
  }
  else
  {
	if (commanded)
	  sim1->setMotorMGRResponse('2');	//motor is already off
  }
}

void Motor_MGR::terminateStopRelay()
{
  if (stopSequenceOn && millis() - tempStopSequenceTimer > (stopSequenceTimerTime * 100))
  {
	if ((bool)eeprom1->AUTOSTART && ACPowerState())
	  digitalWrite(PIN_MSTOP, LOW);

	stopSequenceOn = false;

	if (!getMotorState())		//motor has turned off
	{
	  if (gotOffCommand)
	  {
		gotOffCommand = false;
		sim1->setMotorMGRResponse('O');		//motor has stopped
	  }
	  else if(offButtonPressed)
	  {
		offButtonPressed=false;
	  	simEventTemp[8] = sim1->registerEvent('O'); //register TO SIM motor has turned off
	  }
	}
	else	//motor is still on, with either phase 1 or phase 2 feedback on with ACPhase on
	{
	  if (gotOffCommand)
	  {
		gotOffCommand = false;
		sim1->setMotorMGRResponse('P');		//cannot turn off motor
	  }
	  else
	  {
		simEventTemp[1] = sim1->registerEvent('P');
	  }
	}
	offButtonPressed=false;
  }
}

void Motor_MGR::terminateStarDeltaTimer()
{
	if(starDeltaTimerOn && millis() - tempStartSequenceTimer > ((unsigned int)eeprom1->starDeltaTimerTime *1000))
	{
		digitalWrite(PIN_MSTART,HIGH);
		starDeltaTimerOn=false;
		#ifdef ENABLE_CURRENT
			enableCurrentBuffer=true;
			tempStartSequenceTimer=millis();
		#endif
	}
}

void Motor_MGR::terminateStartRelay()
{
  if (startSequenceOn &&  millis() - tempStartSequenceTimer > (startSequenceTimerTime * 100))
  {
  	if(((unsigned int)eeprom1->starDeltaTimerTime *10) <= startSequenceTimerTime)
  	{
		digitalWrite(PIN_MSTART, HIGH);
		enableCurrentBuffer=true;
		tempStartSequenceTimer=millis();
  	}
  	else
  	{
  		starDeltaTimerOn=true;
  	}
	startSequenceOn = false;

// #ifndef disable_debug
	// _NSerial->print("Start");
	// _NSerial->println("Over");
// #endif

	bool motor = getMotorState();
	if (gotOnCommand)
	{
	  gotOnCommand = false;
	  if (motor)
	  {
		sim1->setMotorMGRResponse('S'); // motor has started
	  }
	  else
	  {
		stopMotor(false, true);
		sim1->setMotorMGRResponse('N');	//cannot start motor due to some problem
	  }
	}
	else
	{
	  if (motor)
		simEventTemp[7] = sim1->registerEvent('S');// ;//register To SIM Motor has started
	  else
	  {
		stopMotor(false, true);
		simEventTemp[0] = sim1->registerEvent('N');//register To SIM motor not started due to ANY REASON
	  }
	}
  }
}

void Motor_MGR::statusOnCall()
{
	byte b = checkLineSensors();
	if (b == AC_OFF)
	{
	  sim1->setMotorMGRResponse('L');	//motor off, no light
	}
	else if (b == AC_2PH)	//power only in 2 phase
	{
	  sim1->setMotorMGRResponse('A');
	}
	else if (b == AC_3PH)
	{
		bool temp = getMotorState();
	  	if (temp)
	  	{
			sim1->setMotorMGRResponse('1');	//motor is on
	  	}
		else
		{
		  	sim1->setMotorMGRResponse('3');	//motor off, light on
		}
	}
}

#ifdef ENABLE_M2M

void Motor_MGR::setM2MEventState(byte eventNo, byte state)
{
	if(m2mEvent[eventNo]==ME_SERVICING)
	{
		if(state==ME_NOTAVAILABLE)
		{
			state=ME_CLEARED;
			simEventTemp[mapTable[eventNo]]=false;	//regsiter relevant Normal Event
		}
	}
	m2mEvent[eventNo]=state;		
}

void Motor_MGR::M2MEventManager()
{
	byte j=2;
	while(j--)
	{
		if(m2mEvent[j]==ME_WAITREGISTER)
		{
			sim1->registerM2MEvent(j);
		}
	}
	// for(byte j = 0 ; j<2; j++)
	// {
	// 	if(m2mEvent[j]==ME_WAITREGISTER)
	// 	{
	// 		sim1->registerM2MEvent(j);
	// 	}
	// }
}
#endif

void Motor_MGR::SIMEventManager()
{
	#ifdef ENABLE_WATER
		#ifdef ENABLE_GP
		byte i=19;
		#else
		byte i=17;
		#endif
	#else
		#ifdef ENABLE_CURRENT
			byte i=14;
		#else
			byte i=12;
		#endif
	#endif
	while(i--)
	{
		if (!simEventTemp[i])
		  simEventTemp[i] = sim1->registerEvent(simEvent[i]);
	}
 //  for (byte i = 0; i < j; i++)
 //  {
	// if (!simEventTemp[i])
	//   simEventTemp[i] = sim1->registerEvent(simEvent[i]);
 //  }
}

void Motor_MGR::setLED(bool t)
{
	if(t) digitalWrite(PIN_3PHASELED,HIGH);
	else digitalWrite(PIN_3PHASELED,LOW);
}

bool Motor_MGR::checkSleepElligible()
{
	#ifdef ENABLE_WATER
		#ifdef ENABLE_GP
		byte j=19;
		#else
		byte j=17;
		#endif
	#else
		#ifdef ENABLE_CURRENT
			byte j=14;
		#else
			byte j=12;
		#endif
	#endif
	bool event=true;
	while(j--)
	{
		if(!simEventTemp[j]) 
		{
			event=false;
			break;
		}
	}
	#ifdef ENABLE_M2M
	if(event && m2mEvent[0] == ME_WAITREGISTER || m2mEvent[1]==ME_WAITREGISTER)
		event=false;
	#endif
	
	// #ifdef ENABLE_WATER
	return (!ACPowerState() && !eventOccured && event && !waitStableLineOn && !singlePhasingTimerOn
			&& !startTimerOn && !startSequenceOn && !stopSequenceOn); 	//!waitCheckACTimerOn &&
	// #else
	// 	return (!ACPowerState() && !eventOccured && event && !waitStableLineOn && !singlePhasingTimerOn
	// 		&& !startTimerOn && !startSequenceOn && !stopSequenceOn); 	//!waitCheckACTimerOn &&
	// #endif
}

void Motor_MGR::operateOnButtonEvent()
{
	noInterrupts();
	buttonEventOccured=false;
	interrupts();

	if(digitalRead(PIN_STARTBUTTON)==LOW)
	{
		lastPressTime=millis();
		lastButtonEvent=BTNEVENTSTART;
	}
	else if (digitalRead(PIN_STOPBUTTON)==LOW)
	{
		lastPressTime=millis();
		lastButtonEvent=BTNEVENTSTOP;
	}
	#if ! defined(ENABLE_GP) && ! defined(ENABLE_CURRENT)
	else if(digitalRead(PIN_AUTOBUTTON)==LOW)
	{
		lastButtonEvent=BTNEVENTAUTO;
		lastPressTime=millis();
	}
	#endif	

	#if ! defined(ENABLE_GP) && ! defined(ENABLE_CURRENT)
		byte pin = PIN_AUTOBUTTON;
		if(lastButtonEvent == BTNEVENTSTART)  pin = PIN_STARTBUTTON;
	#else
		byte pin = PIN_STARTBUTTON;
	#endif
		if(lastButtonEvent == BTNEVENTSTOP)  pin = PIN_STOPBUTTON;
	
	if(digitalRead(pin)==HIGH)
		lastButtonEvent=0;

	// bool b= digitalRead(PIN_AUTOBUTTON);
	// if(!b)		//auto button is pressed
	// {
	// 	if(millis() - lastPressTime > 500)
	// 	{
	// 		lastPressTime=millis();
	// 	}
	// }
	// else if(b)		//auto button is released
	// {
	// 	if(millis()- lastPressTime > 100)		// operate on auto button press, only if pressed for more than 100 mills
	// 	{
	// }
	// #endif
}

inline void Motor_MGR::buttonFilter()
{
	if(lastButtonEvent>0 && millis() - lastPressTime > 100)
	{
		if(lastButtonEvent==BTNEVENTSTART && digitalRead(PIN_STARTBUTTON)==LOW)
		{
				lastButtonEvent=0;
				startMotor();
		}
		else if(lastButtonEvent==BTNEVENTSTOP && digitalRead(PIN_STOPBUTTON)==LOW)
		{
			lastButtonEvent=0;
			stopMotor(false,false,true);
		}
		#if ! defined(ENABLE_GP) && ! defined(ENABLE_CURRENT)
		else if(lastButtonEvent==BTNEVENTAUTO && digitalRead(PIN_AUTOBUTTON)==LOW)
		{
			lastButtonEvent=0;
	    	eeprom1->saveAutoStartSettings(!eeprom1->AUTOSTART);  //set AutoStart to True in EEPROM
			resetAutoStart(true);
			if(eeprom1->AUTOSTART)
				simEventTemp[10] = sim1->registerEvent('8');
			else
				simEventTemp[11] = sim1->registerEvent('9');
		}
		#endif
	}
}

void Motor_MGR::update()
{

	if(!startSequenceOn && !stopSequenceOn)
	{
		checkCurrentConsumption();

		noInterrupts();
			byte tempEventOccured=eventOccured;
			byte tempButtonEventOccured=buttonEventOccured;
			// #ifdef ENABLE_WATER
			// 	byte tempWaterEventOccured=waterEventOccured;
			// #endif
		interrupts();

		if(tempEventOccured)
			operateOnEvent();

		if(tempButtonEventOccured)
			operateOnButtonEvent();

		if(lastButtonEvent) 
			buttonFilter();

		#ifdef ENABLE_WATER
			// if(waterEventOccured)
			// 	operateOnWaterEvent();
		#endif
	}
 //  if (!startSequenceOn && !stopSequenceOn && eventOccured)
	// operateOnEvent();

  if (waitStableLineOn && waitStableLineOver())
	  operateOnStableLine();
	// if ()

 //  if (waitCheckACTimerOn)
	// if (waitCheckACTimerOver())
	//   unknownMotorOff();

	if(singlePhasingTimerOn)
	{
		bool b = digitalRead(PIN_3PHASE);
		if(!b)		//3 phase pin is low
		{
			if(singlePhasingTimerOver())
			{
				operateOnSinglePhasing();
			}
		}
		else
		{
			singlePhasingTimerOn=false;
		}
	}
  // if (singlePhasingTimerOn && singlePhasingTimerOver())
	 //  operateOnSinglePhasing();
	// if ()

  if (startTimerOn)
	if (startMotorTimerOver())
	  startMotor();

 //  if (stopTimerOn)
	// if (stopMotorTimerOver())
	//   stopMotor();

  if (startSequenceOn)
	terminateStartRelay();

  if(starDeltaTimerOn)
  	terminateStarDeltaTimer();

  if (stopSequenceOn)
	terminateStopRelay();

  SIMEventManager();
  #ifdef ENABLE_M2M
  	M2MEventManager();
  #endif
  // enterSleep=checkSleepElligible();
}
