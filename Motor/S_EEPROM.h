#ifndef S_EEPROM_h
#define S_EEPROM_h

#include <Arduino.h>
#include <EEPROM.h>
#include "Definitions.h"

class S_EEPROM
{
    void loadNumberSettings();
    // void loadTempSettings();
    void loadAutoStartSettings();
    #ifdef ENABLE_WATER
        void loadPreventOverFlowSettings();
    #endif
    void loadEventStageSettings();
    void loadAutoStartTimeSettings();
    void loadDNDSettings();
    void loadBypassSettings();
    void loadResponseSettings();
    void loadCCID();
    void loadStarDeltaTimer();

    void setAutoLed();
    

    // void loadAlterNumber();
    // void loadAlterNumberSettings();
    // void clearLoadedNumbers();

    // void updateNumberChanges();
    bool write_StringEE(unsigned short int Addr, String input);
    String read_StringEE(unsigned short int Addr, byte length);
    bool eeprom_read_string(unsigned short int addr, char* buffer, byte bufSize);
    bool eeprom_write_string(unsigned short int addr, const char* str);
    bool eeprom_is_addr_ok(unsigned short int addr);
    bool eeprom_write_bytes(unsigned short int startAddr, const byte* array, byte numBytes);

  public:
    byte numbersCount;
    // String primaryNumber;
    // String secondary[4];

    // String alterNumber;
    byte alterNumberSetting;
    byte alterNumberPresent;
    // unsigned short int HIGHTEMP;
    byte simCCIDPresent;


    unsigned long int PROGSIZE;
    bool programSizeSet;

    byte AUTOSTART;
    byte BYPASS;

    byte EVENTSTAGE;
    
    #ifdef ENABLE_WATER
        byte PREVENTOVERFLOW;
    #endif
    unsigned short int AUTOSTARTTIME;
    unsigned short int starDeltaTimerTime;

    byte DND;
    char RESPONSE;

    S_EEPROM();

    // #ifndef disable_debug
    String getNumbers();
    // #endif

    byte checkExists(String &number);
    void saveAlterNumberSetting(bool);
    // void saveTempSettings(unsigned short int temp);
    void saveAutoStartSettings(bool);

    #ifdef ENABLE_WATER
        void savePreventOverFlowSettings(bool);
    #endif

    void saveAutoStartTimeSettings(unsigned short int);
    void saveStarDeltaTimer(unsigned short int);
    void saveEventStageSettings(byte);
    void saveBypassSettings(bool);
    void saveDNDSettings(bool);
    void saveResponseSettings(char);
    unsigned long int getProgramSize();
    byte getUpdateStatus();
    void discardUpdateStatus();
    
    void saveProgramSize(unsigned long int);
    void updateFirmware(bool,bool);

    void setCCID(String &);
    bool getCCID(String &);

    void loadAllData();
    bool addNumber(String &number);
    // bool saveBalNumber(String &str);
    // bool getBalNumber(String &str);
    bool isPrimaryNumber(String str);
    bool isAlterNumber(String str);
    String getActiveNumber();

    String getDeviceId();

    bool addAlternateNumber(String &number);
    bool removeNumber(String &number);
    void clearNumbers(bool admin);
};
#endif
