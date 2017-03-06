// To detect the presence of AC mains phase (R Y).
// along with phase (B Y) and (R B).
// Also report the status of motor, using (B Y) and (R B) phase lines.
// Also report single phase failure, and all out AC failure.



#ifndef Motor_MGR_h
#define Motor_MGR_h

#include "Definitions.h"
#include "BATTERY_MGR.h"
#include "S_EEPROM.h"
#include "SIM.h"
#include <Arduino.h>

class BATTERY_MGR;
class SIM;

class Motor_MGR
{
	BATTERY_MGR* battery1;
	S_EEPROM* eeprom1;
	SIM* sim1;

	bool simEventTemp[9];
	char simEvent[9];

	bool gotOffCommand;
	bool gotOnCommand;

	bool phase1;
	bool phase2;
	bool phaseAC;

	bool startTimerOn;
	unsigned long int tempStartTimer;

	bool stopTimerOn;
	unsigned long int tempStopTimer;
	unsigned int stopTimerTime;	
	
	bool waitCheckACTimerOn;
	unsigned long tempWaitCheckACTimer;
	byte waitCheckACTime;

	bool singlePhasingTimerOn;
	unsigned long int tempSinglePhasingTimer;
	byte singlePhasingTime;

	bool startSequenceOn;
	unsigned long int tempStartSequenceTimer;
	byte startSequenceTimerTime;

	bool stopSequenceOn;
	unsigned long int tempStopSequenceTimer;
	byte stopSequenceTimerTime;

	bool waitStableLineOn;
	unsigned long int waitStableLineTimer;
	byte waitStableLineTime;

	void anotherConstructor(SIM* sim1,S_EEPROM* eeprom1,BATTERY_MGR* battery1);
	void readSensorState(bool &p1,bool &p2,bool &p3);
	void updateSensorState(bool &p1,bool &p2,bool &p3);	
	void operateOnEvent();
	bool startMotorTimerOver();
	bool stopMotorTimerOver();
	
	bool waitStableLineOver();
	void operateOnStableLine();
	
	void setACPowerState(bool b);

	bool singlePhasingTimerOver();
	void operateOnSinglePhasing();
	bool waitCheckACTimerOver();
	void unknownMotorOff();

	void terminateStopRelay();
	void terminateStartRelay();
	void SIMEventManager();

	#ifndef disable_debug
  		#ifdef software_SIM
		    HardwareSerial* _NSerial;
		#else
		    SoftwareSerial* _NSerial;
		#endif
	#endif

public:

	#ifndef disable_debug
  		#ifdef software_SIM
			Motor_MGR(HardwareSerial *s,SIM* sim1,S_EEPROM* eeprom1,BATTERY_MGR* battery1);
		#else
			Motor_MGR(SoftwareSerial *s,SIM* sim1,S_EEPROM* eeprom1,BATTERY_MGR* battery1);
		#endif
	#else
		Motor_MGR(SIM* sim1,S_EEPROM* eeprom1,BATTERY_MGR* battery1);
	#endif

	bool eventOccured;

	bool getChargeState();			//used by SIM
	unsigned short int getBatVolt();				//used by SIM
	bool getMotorState();
	void startMotor(bool commanded=false);
	void stopMotor(bool commanded=false);
	void statusOnCall();
	void update();
};
#endif