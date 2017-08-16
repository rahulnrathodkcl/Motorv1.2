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

  #ifndef ENABLE_GP
  pinMode(PIN_AUTOBUTTON,INPUT_PULLUP);
  #endif

  pinMode(PIN_MOTORLED,OUTPUT);

  gotOffCommand = false;
  gotOnCommand = false;

  eventOccured = false;
  startTimerOn = false;

  stopTimerTime = 600;
  stopTimerOn = false;

  waitCheckACTime = 20;
  waitCheckACTimerOn = false;

  singlePhasingTime = 10;
  singlePhasingTimerOn = false;

  startSequenceTimerTime = 20;
  starDeltaTimerOn=false;
  startSequenceOn = false;

  stopSequenceTimerTime = 20;
  stopSequenceOn = false;

  waitStableLineTime = 50;
  waitStableLineOn = false;

  AllPhaseState(false); // allPhase = false;
  motorState(false);// mFeedback = false;
  ACFeedbackState(false); // acFeedback = false;
  ACPowerState(false);//  phaseAC = false;

  #ifdef ENABLE_WATER
  	setWaterDefaults();
  #endif

lastPressTime=0;


  for (byte i = 0; i < 12; i++)
	simEventTemp[i] = true;

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
  
  // resetAutoStart();
}

#ifdef ENABLE_WATER

void Motor_MGR::lowSensorState(bool temp)
{
	lowLevelSensor=temp;	
}

bool Motor_MGR::lowSensorState()
{
	return lowLevelSensor;
}

void Motor_MGR::midSensorState(bool temp)
{
	midLevelSensor=temp;
}

bool Motor_MGR::midSensorState()
{
	return midLevelSensor;
}

void Motor_MGR::highSensorState(bool temp)
{
	highLevelSensor=temp;
}

bool Motor_MGR::highSensorState()
{
	return highLevelSensor;
}

#ifdef ENABLE_GP

void Motor_MGR::overHeadLowSensorState(bool temp)
{
	oLowLevelSensor=temp;	
}

bool Motor_MGR::overHeadLowSensorState()
{
	return oLowLevelSensor;
}

void Motor_MGR::overHeadHighSensorState(bool temp)
{
	oHighLevelSensor=temp;	
}

bool Motor_MGR::overHeadHighSensorState()
{
	return oHighLevelSensor;
}

void Motor_MGR::readOverHeadWaterSensorState(bool &olow,bool &ohigh)
{
  	noInterrupts();
  	olow = digitalRead(PIN_OLOWSENSOR);
	ohigh = digitalRead(PIN_OHIGHSENSOR);
	interrupts();
}

void Motor_MGR::updateOverHeadWaterSensorState(bool &olow,bool &ohigh)
{
	overHeadLowSensorState(olow);
	overHeadHighSensorState(ohigh);
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
	updateOverHeadWaterSensorState(olow,ohigh);
	return ans;
}

void Motor_MGR::overHeadWaterStatusOnCall()
{
	byte temp = getOverHeadWaterSensorState();
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
		overHeadLowSensorState(false);
		overHeadHighSensorState(false);
	#endif

	waterEventBufferTime=200; 		// 200x100= 20,000ms = 20s
	waterEventOccured=false;

	lowSensorState(false);
	midSensorState(false);
	highSensorState(false);

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
}

void Motor_MGR::readWaterSensorState(bool &low,bool &mid,bool &high)
{
	waterEventOccured = false;
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

void Motor_MGR::updateWaterSensorState(bool &low,bool &mid,bool &high)
{
	lowSensorState(low);
	midSensorState(mid);
	highSensorState(high);
}

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
	updateWaterSensorState(l,m,h);
	return ans;
}

void Motor_MGR::waterStatusOnCall()
{
	byte temp = getWaterSensorState();
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

void Motor_MGR::operateOnWaterEvent()
{
	if(millis()-tempWaterEventTime<(waterEventBufferTime*100))	
		return;
	
	bool low,mid,high;
	readWaterSensorState(low,mid,high);

	#ifdef ENABLE_GP
		bool olow,ohigh;
		readOverHeadWaterSensorState(olow,ohigh);
	#endif

	if((low == lowSensorState()) && (mid == midSensorState()) && (high == highSensorState()))		// no changes in sensor state
	{
		#ifdef ENABLE_GP
		if(olow == overHeadLowSensorState() && (ohigh == overHeadHighSensorState()))
		{
			return;
		}
		#else
			return;
		#endif
	}

  if (motorState())		//motorOn
  {
  	if(low && !lowSensorState())				//water is below the motor, so stop the motor.
  	{
  		stopMotor(false,true);
		simEventTemp[12] = sim1->registerEvent('I'); //report To SIM Motor Off due to insufficient water level
  	}
  	else if (!low && !mid && midSensorState())  //increase in water level
  	{
		simEventTemp[16] = sim1->registerEvent('Z'); //report To SIM water level is increasing..
  	}
  	else if(!low && mid && !midSensorState())	//decrease in water level
  	{
		simEventTemp[13] = sim1->registerEvent('D'); //report To SIM water level is decrease..
  	}
  	else if(!low && !mid && !high && highSensorState())
  	{
  		if(eeprom1->PREVENTOVERFLOW)
  		{
  			stopMotor(false,true);
			simEventTemp[14] = sim1->registerEvent('H'); //report To SIM well is full, so stopped motor
  		}
  		else
  		{
			simEventTemp[15] = sim1->registerEvent('E'); //report To SIM well is full.
  		}
  	}
  	#ifdef ENABLE_GP
  	else if(!olow && !ohigh && overHeadHighSensorState())		//overhead tank is full
  	{
  		stopMotor(false,true);
		simEventTemp[17] = sim1->registerEvent('V'); //report To SIM Motor Off due to overhead tank full
  	}
  	else if (olow && ohigh && !overHeadLowSensorState())
  	{
		simEventTemp[18] = sim1->registerEvent('W'); //report To SIM , overhead tank empty.
  	}
  	#endif
  }
  else    //motor is off
  {
  	if(!low && !mid && midSensorState()) 		//water is increasing, reached mid sensor
  	{
  		#ifdef ENABLE_GP
  			if(ohigh)
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
  	else if (olow && !low)// overhead tank is empty, and underground not low
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
  	else if(!low && !mid && !high && highSensorState())
  	{
		simEventTemp[15] = sim1->registerEvent('E'); //report To SIM well is full.
  	}
  }
  #ifdef ENABLE_GP
  	updateOverHeadWaterSensorState(olow,ohigh);
  #endif
  updateWaterSensorState(low,mid,high);
}
#endif

bool Motor_MGR::getMotorState()
{
  bool p1, p2, p3, p4;
  readSensorState(p1, p2, p3, p4);
  // ACPowerState(p4);

  // if(p3 && !phaseAC)
  // 	// battery1->gotACPower();
  // else if(!p3 && phaseAC)
  // 	// battery1->lostACPower();
  // motorState(p2);
  updateSensorState(p1, p2, p3, p4);
  return p2;
}

void Motor_MGR::readSensorState(bool &p1, bool &p2, bool &p3, bool &p4)
{
  eventOccured = false;
  noInterrupts();
  p1 = digitalRead(PIN_3PHASE);
  p2 = !digitalRead(PIN_MFEEDBACK);
  p3 = 0;//!digitalRead(PIN_ACFEEDBACK);
  p4 = digitalRead(PIN_ACPHASE);
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

void Motor_MGR::updateSensorState(bool &p1, bool &p2, bool &p3, bool &p4)
{
  AllPhaseState(p1); // allPhase = p1;
  motorState(p2); // mFeedback = p2;
  ACFeedbackState(p3); // acFeedback = p3;
  ACPowerState(p4); // phaseAC = p4;

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
		if (AllPhaseState())				//auto start is on , and AC is Present in 3 phase, so , can start with switch or external force.
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

bool Motor_MGR::ACFeedbackState()
{
  return acFeedback;
}

inline void Motor_MGR::ACFeedbackState(bool b)
{
  acFeedback = b;
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
  return allPhase;
}

inline void Motor_MGR::AllPhaseState(bool b)
{
  allPhase = b;
}

void Motor_MGR::operateOnEvent()
{
  bool t3Phase, tMotor, tacFeedback, tacPhase;
  readSensorState(t3Phase, tMotor, tacFeedback, tacPhase);

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
  updateSensorState(t3Phase, tMotor, tacFeedback, tacPhase);
}

byte Motor_MGR::checkLineSensors()
{
#ifndef disable_debug
  _NSerial->println("CLSR");
  _NSerial->print("3P:");
  _NSerial->println(AllPhaseState());
  _NSerial->print("MF:");
  _NSerial->println(motorState());
  // _NSerial->print("ACF:");
  // _NSerial->println(ACFeedbackState());
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

  // else if (!semiState && temp == AC_1PH)		//Got Power in 1 phase
  // {
	// semiState = true;
	// if (!eeprom1->DND)	//if DND off then
	  // simEventTemp[10] = sim1->registerEvent('A'); //register TO SIM 1 phase power ON
  // }
}

bool Motor_MGR::startMotorTimerOver()
{
  return (millis() - tempStartTimer >= (((unsigned long int)eeprom1->AUTOSTARTTIME * 1000)));
}

bool Motor_MGR::stopMotorTimerOver()
{
  return (millis() - tempStopTimer >= (stopTimerTime * 100));
}

bool Motor_MGR::waitCheckACTimerOver()
{
  return (waitCheckACTimerOn && (millis() - tempWaitCheckACTimer > (waitCheckACTime * 100)));
}

void Motor_MGR::unknownMotorOff()
{
  // waitCheckACTimerOn = false;
  //report to SIM Motor Off due to Unknown Reason
  stopMotor(false,true);
  simEventTemp[2] = sim1->registerEvent('U');
}

bool Motor_MGR::singlePhasingTimerOver()
{
  return (singlePhasingTimerOn && millis() - tempSinglePhasingTimer > (singlePhasingTime * 100));
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
	  motorState(true);
	  gotOnCommand = commanded;
	}
	else
	{
	  if (commanded)
		sim1->setMotorMGRResponse('O');
	}
  }
  else
  {
	if (commanded)
	  sim1->setMotorMGRResponse('L');	//cannot start motor due to some problem
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
  }
  else
  {
	if (commanded)
	  sim1->setMotorMGRResponse('O');
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
		sim1->setMotorMGRResponse('D');		//motor has stopped
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
		sim1->setMotorMGRResponse('L');		//cannot turn off motor
	  }
	  else
	  {
		simEventTemp[1] = sim1->registerEvent('P');
	  }
	  //register to SIM cannot turn off motor due to some problem
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
	}
}

void Motor_MGR::terminateStartRelay()
{
  if (startSequenceOn &&  millis() - tempStartSequenceTimer > (startSequenceTimerTime * 100))
  {
  	if(((unsigned int)eeprom1->starDeltaTimerTime *10) <= startSequenceTimerTime)
  	{
		digitalWrite(PIN_MSTART, HIGH);
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

	if (gotOnCommand)
	{
	  gotOnCommand = false;
	  if (getMotorState())
	  {
		sim1->setMotorMGRResponse('D'); // motor has started
	  }
	  else
	  {
		stopMotor(false, true);
		sim1->setMotorMGRResponse('L');	//cannot start motor due to some problem
	  }
	}
	else
	{
	  if (getMotorState())
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
  bool temp = getMotorState();
  if (temp)
	sim1->setMotorMGRResponse('D');	//motor is on
  else
  {
	byte b = checkLineSensors();

	if (b == AC_OFF)
	  sim1->setMotorMGRResponse('L');	//motor off, no light
		// else if (b == AC_1PH)	// power only in 1 phase
	  // sim1->setMotorMGRResponse('A');	//motor off, no light
	else if (b == AC_2PH)	//power only in 2 phase
	  sim1->setMotorMGRResponse('A');
	else if (b == AC_3PH)
	  sim1->setMotorMGRResponse('O');	//motor off, light on
  }
}

void Motor_MGR::SIMEventManager()
{
	#ifdef ENABLE_WATER
		#ifdef ENABLE_GP
		byte j=19;
		#else
		byte j=17;
		#endif
	#else
		byte j=12;
	#endif
  for (byte i = 0; i < j; i++)
  {
	if (!simEventTemp[i])
	  simEventTemp[i] = sim1->registerEvent(simEvent[i]);
  }
}

void Motor_MGR::setLED(bool t)
{
	if(t) digitalWrite(PIN_3PHASELED,HIGH);
	else digitalWrite(PIN_3PHASELED,LOW);
}

bool Motor_MGR::checkSleepElligible()
{
	bool event=true;
	byte i=12;
	while(--i)
	{
		if(!simEventTemp[i]) 
		{
			event=false;
			break;
		}
	}
	return (!ACPowerState() && event && !waitStableLineOn && !waitCheckACTimerOn && !singlePhasingTimerOn
			&& !startTimerOn && !stopTimerOn && !startSequenceOn && !stopSequenceOn);
}

void Motor_MGR::operateOnButtonEvent()
{
	buttonEventOccured=false;

	if(digitalRead(PIN_STARTBUTTON)==LOW)
		startMotor();
	else if (digitalRead(PIN_STOPBUTTON)==LOW)
		stopMotor(false,false,true);
	#ifndef ENABLE_GP
	else if(digitalRead(PIN_AUTOBUTTON)==LOW)
	{
		if(millis() - lastPressTime > 500)
		{
			lastPressTime=millis();

	    	eeprom1->saveAutoStartSettings(!eeprom1->AUTOSTART);  //set AutoStart to True in EEPROM
			resetAutoStart(true);
			if(eeprom1->AUTOSTART)
				simEventTemp[10] = sim1->registerEvent('8');
			else
				simEventTemp[11] = sim1->registerEvent('9');
		}
	}
	#endif
}

void Motor_MGR::update()
{
	if(!startSequenceOn && !stopSequenceOn)
	{
		if(eventOccured)
			operateOnEvent();

		if(buttonEventOccured)
			operateOnButtonEvent();

		#ifdef ENABLE_WATER
			if(waterEventOccured)
				operateOnWaterEvent();
		#endif
	}
 //  if (!startSequenceOn && !stopSequenceOn && eventOccured)
	// operateOnEvent();

  if (waitStableLineOn)
	if (waitStableLineOver())
	  operateOnStableLine();

  if (waitCheckACTimerOn)
	if (waitCheckACTimerOver())
	  unknownMotorOff();

  if (singlePhasingTimerOn)
	if (singlePhasingTimerOver())
	  operateOnSinglePhasing();

  if (startTimerOn)
	if (startMotorTimerOver())
	  startMotor();

  if (stopTimerOn)
	if (stopMotorTimerOver())
	  stopMotor();

  if (startSequenceOn)
	terminateStartRelay();

  if(starDeltaTimerOn)
  	terminateStarDeltaTimer();

  if (stopSequenceOn)
	terminateStopRelay();

  SIMEventManager();

  // enterSleep=checkSleepElligible();
}
