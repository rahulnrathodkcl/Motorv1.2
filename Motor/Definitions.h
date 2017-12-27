#ifndef DEF_h
#define DEF_h

// #include <SoftwareSerial.h>
#include <HardwareSerial.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/power.h>

#define TURN_ON true
#define TURN_OFF false

#define BVTHRESHOLD 363

#define BTNEVENTSTART 1
#define BTNEVENTSTOP 2
#define BTNEVENTAUTO 3


#define adminNumber "7041196959"
#define adminNumber1 "7698439201"
#define adminNumber2 "7383614214"
#define adminNumber3 "7383622678"
#define adminNumber4 "9586135978"

// #define adminNumber F("7041196959")
// #define adminNumber1 F("7698439201")
// #define adminNumber2 F("7383614214")
// #define adminNumber3 F("7383622678")
// #define adminNumber4 F("9586135978")

#define STR_SAPBR_PARAM "AT+SAPBR=3,1,\""
#define STR_SAPBR_GPRS "ConType\",\"GPRS\"\r\n"
#define STR_SAPBR_APN "APN\",\""
#define STR_SAPBR_START "AT+SAPBR=1,1\r\n"
#define STR_SAPBR_STOP "AT+SAPBR=0,1\r\n"

#define WATCHDOG_OFF    (0)
#define I_WATCHDOG_8S     (_BV(WDP3) | _BV(WDP0) | _BV(WDIE))
#define I_WATCHDOG_4S     (_BV(WDP3) | _BV(WDIE))


#define MAXNUMBERS 15

#define disable_debug
#define ENABLE_CURRENT 
// #define ENABLE_WATER
// #define ENABLE_GP
// #define ENABLE_M2M

#ifdef ENABLE_M2M
	#ifndef ENABLE_WATER
		#undef ENABLE_M2M
	#endif
#endif

// #ifdef ENABLE_CURRENT
// 	#ifdef ENABLE_GP
// 		#undef ENABLE_GP
// 	#endif
// #endif

// #define software_SIM
// #ifdef disable_debug
// #undef software_SIM
// #endif

#define PIN_STOPBUTTON A0
#define PIN_STARTBUTTON A1
#define PIN_AUTOBUTTON A5
#define PIN_AUTOLED A4
#define PIN_MOTORLED A2

#define PIN_MSTOP 13
#define PIN_MSTART 12
// #define PIN_BAT_CHARGE A3

// #define PIN_BAT_SENSOR_PWR A1				//not needed
// #define PIN_BAT_SENSOR A2
// #define VOLTAGERATIO 2						//2

#define PIN_3PHASE 10
#define PIN_MFEEDBACK 11
#define PIN_ACFEEDBACK 12
#define PIN_BATLEVEL 9
#define PIN_TURNOFF 8

#define PIN_ACPHASE 3

#define PIN_DTR 4
#define PIN_RING 2
#define PIN_3PHASELED A3

	#define PIN_LOWSENSOR 6
	#define PIN_MIDSENSOR 7
	#define PIN_HIGHSENSOR 5
#ifdef ENABLE_WATER
	#define HIGHLEVEL 0x03
	#define MIDLEVEL 0x02
	#define LOWLEVEL 0x01
	#define CRITICALLEVEL 0x00
	#define preventOverFlowAddress 50

	#ifdef ENABLE_GP
		#undef PIN_AUTOLED
		#undef PIN_AUTOBUTTON
		#define OVERHEADCRITICALLEVEL 0x00
		#define OVERHEADMIDLEVEL 0x01
		#define OVERHEADHIGHLEVEL 0x02
		#define PIN_OLOWSENSOR A5
		#define PIN_OHIGHSENSOR A4
	#endif
#endif

#ifdef ENABLE_CURRENT
		// #undef PIN_AUTOLED
		#undef PIN_AUTOBUTTON

	#ifdef ENABLE_GP
		#undef PIN_OLOWSENSOR
		#undef PIN_OHIGHSENSOR
		#undef ENABLE_GP
	#endif

	#define PIN_CURRENT A5
	#define CR_OVER2 2
	#define CR_OVER 1
	#define CR_UNDER 2
	#define CR_NORMAL 0

#endif
// #define PIN_PHASE1 9
// #define PIN_PHASE2 10
// #define PIN_ACPHASE 11

// #define PIN_DTR 3
// #define PIN_RING 2

//EEPROM
#define autoStartAddress 0
#define autoStartTimeAddress 4
#define highTempAddress 8
#define dndAddress 12
#define responseAddress 14
#define starDeltaTimerAddress 18
// #define startVoltageAddress 16
// #define stopVoltageAddress 20
#define alterNumberPresentAddress 22
#define alterNumberSettingAddress 26
#define alterNumberAddress 30  

#define bypassAddress 54
#define eventStageAddress 58

#define SEND_TO_M2M_MASTER 0x02

#ifdef ENABLE_M2M
	#define SEND_TO_M2M_REMOTE 0x01

	#define m2mSettingAddress 62
	#define m2mRemotePresentAddress 66 
	#define m2mRemoteVerifyAddress 70
	#define m2mRemoteNumberAddress 74

	#define ME_CLEARED 0x00
	#define ME_WAITREGISTER 0x01
	#define ME_SERVICING 0x02
	#define ME_NOTAVAILABLE 0x03
#endif

#define m2mPresentAddress 88
#define m2mVerifyAddress 92
#define m2mNumberAddress 96

#define noCallAddress 150
#define noCallStartTimeHourAddress 152
#define noCallStartTimeMinuteAddress 154
#define noCallStopTimeHourAddress 156
#define noCallStopTimeMinuteAddress 158
#define lowVoltAddress 160

#ifdef ENABLE_CURRENT
	// #define STARTINGCURRENT 800
	#define currentDetectionAddress 164
	#define normalLoadAddress 168
	#define overloadAddress 172
	#define underloadAddress 176
	#define underloadPerAddress 180
	#define overloadPerAddress 184
#endif

#define numbersCountAddress 300
#define mobileNumberAddress 304

// #define balNumberPresentAddress 150
// #define balNumberAddress 154 // leave 18 bytes for storing bal number
//NOT TO CHANGE____ ASSOCIATED WITH BOOTLOADER 
#define simCCIDPresentAddress 800
#define simCCIDLengthAddress 804
#define simCCIDAddress 808 // minimum length of CCID is 18. max is 22, so leave 30 bytes.
#define deviceIdAddress 840		// deviceId is a long int.

#define prgUpdateRequestAddress 900
#define prgSizeAddress 904
#define prgUpdateStatusAddress 908
#define VerifyStatusAddress 912
#define prgUpdateTryAddress 916


//_______________________

#define EEPROM_MIN_ADDR 0
#define EEPROM_MAX_ADDR 1023

#define STR_MOTOR "MOTOR "
#define STR_ON "ON"
#define STR_OFF "OFF"

#define AC_3PH 0x01
#define AC_2PH 0x02
#define AC_1PH 0x03
#define AC_OFF 0x04

// #define L_REGULAR 0x0A
// #define L_SLEEP 0x0B

//____________________
#endif
//-----------------------------------------------
