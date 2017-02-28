#ifndef DEF_h
#define DEF_h

#include <SoftwareSerial.h>
#include <HardwareSerial.h>

// #define disable_debug
 #define software_SIM
// #ifdef disable_debug
	// #undef software_SIM
// #endif



#define PIN_STOP A5
#define PIN_START A4
#define PIN_BAT_CHARGE A3

#define PIN_BAT_SENSOR_PWR A1				//not needed
#define PIN_BAT_SENSOR A2
#define VOLTAGERATIO 2						//2

#define PIN_PHASE1 9
#define PIN_PHASE2 10
#define PIN_ACPHASE 11

#define PIN_DTR 3
#define PIN_RING 2

//EEPROM 
#define autoStartAddress 0
#define autoStartTimeAddress 4
#define highTempAddress 8

#define numbersCountAddress 12
#define mobileNumberAddress 16

#define alterNumberPresentAddress 110
#define alterNumberSettingAddress 114
#define alterNumberAddress 118
#define dndAddress 122
#define responseAddress 124




#define EEPROM_MIN_ADDR 0
#define EEPROM_MAX_ADDR 1023



#define STR_MOTOR "MOTOR "
#define STR_ON "ON"
#define STR_OFF "OFF"

//____________________
#endif
//-----------------------------------------------
//Constants FOR SPI communicatins
	//Receive Data From Slave in (unsigned short int (2 bytes))
	// #define ASK_RPM 0x20
	// #define ASK_TEMP 0x21	
