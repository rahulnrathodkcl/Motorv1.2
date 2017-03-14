#ifndef S_EEPROM_h
#define S_EEPROM_h

#include <Arduino.h>
#include <EEPROM.h>
#include "Definitions.h"

class S_EEPROM
{
    void loadNumbers();
    void loadTempSettings();
    void loadAutoStartSettings();
    void loadAutoStartTimeSettings();
    void loadDNDSettings();
    void loadResponseSettings();
    void loadStartVoltageSettings();
    void loadStopVoltageSettings();
    void loadAlterNumber();
    void loadAlterNumberSettings();
    void clearLoadedNumbers();
    
    void updateNumberChanges();
    bool write_StringEE(int Addr, String input);
    String read_StringEE(int Addr, int length);
    bool eeprom_read_string(int addr, char* buffer, int bufSize);
    bool eeprom_write_string(int addr, const char* str);
    bool eeprom_is_addr_ok(int addr);
    bool eeprom_write_bytes(int startAddr, const byte* array, int numBytes);

  public:
    byte numbersCount;
    bool varMotorState;
    bool varACPowerState;
    bool varInCall;

    String primaryNumber;
    String secondary[4];
    
    String alterNumber;
    byte alterNumberSetting;
    byte alterNumberPresent;
    unsigned short int HIGHTEMP;
    unsigned short int STARTVOLTAGE;
    unsigned short int STOPVOLTAGE;
    bool resetBattery;
    
    byte AUTOSTART;
    unsigned short int AUTOSTARTTIME;
    
    byte DND;
    char RESPONSE;

    S_EEPROM();

    byte checkExists(String &number);
    void saveAlterNumberSetting(bool);
    void saveTempSettings(unsigned short int temp);
    void saveAutoStartSettings(bool);
    void saveAutoStartTimeSettings(unsigned short int);
    void saveDNDSettings(bool);
    void saveResponseSettings(char);
    void saveStopVoltageSettings(unsigned short int,bool a=false);
    void saveStartVoltageSettings(unsigned short int,bool a=false);
    
    void loadAllData();
    bool addNumber(String &number);
    bool addAlternateNumber(String &number);
    bool removeNumber(String &number);
    void clearNumbers(bool admin);
    
    bool motorState();
    void motorState(bool);

    bool ACPowerState();
    void ACPowerState(bool);

    bool inCall();
    void inCall(bool);
      
};
#endif