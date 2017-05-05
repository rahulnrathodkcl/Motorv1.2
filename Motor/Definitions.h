#ifndef DEF_h
#define DEF_h

#include <SoftwareSerial.h>
#include <HardwareSerial.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/power.h>

#define TURN_ON true
#define TURN_OFF false


#define adminNumber F("7041196959")
#define STR_SAPBR_PARAM "AT+SAPBR=3,1,\""
#define STR_SAPBR_GPRS "ConType\",\"GPRS\"\r\n"
#define STR_SAPBR_APN "APN\",\""
#define STR_SAPBR_START "AT+SAPBR=1,1\r\n"
#define STR_SAPBR_STOP "AT+SAPBR=0,1\r\n"

#define WATCHDOG_OFF    (0)
// #define WATCHDOG_16MS   (_BV(WDE))
// #define WATCHDOG_250MS  (_BV(WDP2) | _BV(WDE))

#define disable_debug
// #define software_SIM
// #ifdef disable_debug
// #undef software_SIM
// #endif

#define PIN_STOP A0
#define PIN_START 13
// #define PIN_BAT_CHARGE A3

// #define PIN_BAT_SENSOR_PWR A1				//not needed
// #define PIN_BAT_SENSOR A2
// #define VOLTAGERATIO 2						//2

#define PIN_3PHASE 10
#define PIN_MFEEDBACK 11
#define PIN_ACFEEDBACK 12
#define PIN_BATLEVEL 9
#define PIN_TURNOFF 7

#define PIN_ACPHASE 3

#define PIN_DTR 4
#define PIN_RING 2
#define PIN_LED A5

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
// #define startVoltageAddress 16
// #define stopVoltageAddress 20

#define numbersCountAddress 24
#define mobileNumberAddress 28

#define alterNumberPresentAddress 122
#define alterNumberSettingAddress 126
#define alterNumberAddress 130  
// #define balNumberPresentAddress 150
// #define balNumberAddress 154 // leave 18 bytes for storing bal number


//NOT TO CHANGE____ ASSOCIATED WITH BOOTLOADER 
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
//Constants FOR SPI communicatins
//Receive Data From Slave in (unsigned short int (2 bytes))
// #define ASK_RPM 0x20
// #define ASK_TEMP 0x21
