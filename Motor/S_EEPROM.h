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
        #ifdef ENABLE_M2M
            void loadM2MSettings();
        #endif
    #endif

    void loadM2MClientSettings();

    void loadEventStageSettings();
    void loadAutoStartTimeSettings();
    void loadDNDSettings();
    void loadBypassSettings();
    void loadResponseSettings();
    void loadCCID();
    void loadStarDeltaTimer();

    #ifndef ENABLE_GP
    void setAutoLed();
    #endif
    

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


    byte m2mPresent;
    byte m2mVerified;

    // unsigned short int HIGHTEMP;
    byte simCCIDPresent;

    unsigned long int PROGSIZE;
    bool programSizeSet;

    byte AUTOSTART;
    byte BYPASS;


    byte EVENTSTAGE;
    
    #ifdef ENABLE_WATER
        #ifndef ENABLE_M2M 
            byte PREVENTOVERFLOW;
        #else
            byte M2M;
            byte m2mRemotePresent;
            byte m2mRemoteVerified;
        #endif
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
        #ifndef ENABLE_M2M
            void savePreventOverFlowSettings(bool);
        #else
            void saveM2MSettings(bool);
            void addM2MRemoteNumber(String &number);
            bool isM2MRemoteNumber(String &number);

            String getM2MRemoteNumber();
            void setM2MRemoteVerified(bool temp);
        #endif
    #endif


    void addM2MNumber(String &number);      //for client
    void setM2MVerify(bool temp);           //for client

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
    bool isM2MNumber(String str);

    String getActiveNumber();
    String getM2MNumber();

    String getDeviceId();

    bool addAlternateNumber(String &number);
    bool removeNumber(String &number);
    void clearNumbers(bool admin);
    String getIndexedNumber(byte index);
};
#endif
