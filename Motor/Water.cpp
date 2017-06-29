#include "Water.h"

Water::Water()
{	
	eventOccured=false;
}


void Water::readSensorState(bool &low, bool &mid, bool &high)
{
  	eventOccured = false;
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

void Water::updateSensorState(bool &low, bool &mid, bool &high)
{
	lowSensorState(low);
	midSensorState(mid);
	highSensorState(high);
}

void Water::operateOnEvent()
{
	if(millis()-tempEventBufferTime<(eventBufferTime*100))
		return;

	readSensorState();
	
	
	updateSensorState();
}

void Water::getWaterSensorState()
{
	
}

void Water::update()
{
	if(eventOccured)
		operateOnEvent();
}