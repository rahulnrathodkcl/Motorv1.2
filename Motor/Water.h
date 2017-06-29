#ifndef WATER_h
#define WATER_h

#include "Definitions.h"
#include "Motor_MGR.h"

class Motor_MGR;

class Water
{

	Motor_MGR* motor1;
	bool lowLevelSensor;
	bool midLevelSensor;
	bool highLevelSensor;

	void operateOnEvent();
	byte eventBufferTime;
	unsigned long tempEventBufferTime;

	void readSensorState(bool &low,bool &mid,bool &high);
	void updateSensorState(bool &low,bool &mid,bool &high);

	void lowSensorState(bool);
	bool lowSensorState();
	void midSensorState(bool);
	bool midSensorState();
	void highSensorState(bool);
	bool highSensorState();


public:
	bool eventOccured;	
	unsigned long eventTime;

	Water();
	void update();
};
#endif