//Version 2 Dated 29052016
#ifndef SIM_h
#define SIM_h

#include <HardwareSerial.h>
//#include <SoftwareSerial.h>
#include "Motor_MGR.h"
#include "S_EEPROM.h"
#include "Definitions.h"
#include <Arduino.h>

//#define disable_debug

class Motor_MGR;

class SIM
{
  private:
    S_EEPROM* eeprom1;
    Motor_MGR* motor1;

    bool responseToAction;
    // String adminNumber;
    byte soundWaitTime; //x100 = mSec
    unsigned long soundWait;
    bool bplaySound;
    char playFile;
    bool inCall;
    // String balStr;

    //bool starPresent;

    unsigned short int callCutWaitTime;  //x100 = mSec
    unsigned long callCutWait;

    bool isMsgFromAdmin;
    //byte attemptsToCall;
    // bool callDialled;
    // bool callAlerted;

    // bool sendCSQResponse;
    // byte batPer;
    // unsigned short batVolt;
    // bool sendStatusResponse;
    // bool initStatusResponse;
    bool sendCUSDResponse;
    // bool initCUSDResponse;

    bool commandsAccepted;
    unsigned short  acceptCommandsTime;
    unsigned long tempAcceptCommandTime;

    char currentStatus;
    char currentCallStatus;

    byte nr;
    bool callAccepted;

    bool makeResponse;
    char actionType;

    bool freezeIncomingCalls;

    char currentOperation;
    bool obtainNewEvent;
    unsigned long obtainEventTimer;

    void anotherConstructor();

    void delAllMsg();
    bool isNumeric(String &str);
    void sendReadMsg(String str);
    bool isMsgBody(String &str);
    bool isAdmin(String str);
    bool isPrimaryNumber(String str);
    void gotMsgBody(String &str);
    bool isNewMsg(String &str);

    bool isNumber(String &str);
    bool checkNumber(String);

    void stopCallWaiting();
    void acceptCommands();
    void rejectCommands();
    bool extendedSendCommand(String cmd,byte timeout);

    bool startGPRS(String);
    bool stopGPRS();
    // bool extendedSendCommand(String &cmd,String vstr,unsigned short int len,unsigned short int timeout);
    bool connectToFTP(String);
    bool setFile(String);
    bool getProgramSize();
    bool downloadFirmware();
    bool isGPRSConnected();
    bool prepareForFirmwareUpdate(String &);

    bool getBlockingResponse(String &cmd,bool (SIM::*func)(String &));
    
    bool isCUSD(String &str);
    bool isCBC(String &);
    bool isCSQ(String &);
    bool sendBlockingATCommand(String,bool =false);
    String readString();
    bool matchString(String, String);
    bool stringContains(String &sstr, String mstr, byte sstart, byte sstop);
    bool isRinging(String);
    bool isDTMF(String &str);
    bool isCut(String);
    bool isSoundStop(String);
    char callState(String);
    String getActiveNumber();
    void makeCall();
    void endCall();
    void acceptCall();
    void sendSMS(String, bool);
    void operateDTMF(String str);
    inline void subDTMF();
    void operateRing();
    bool playSoundElligible();
    void triggerPlaySound();
    void playSoundAgain(String);
    void playSound(char c, bool x = true);
    void stopSound();
    void delay(byte);

    bool callTimerExpire();
    void makeResponseAction();
    bool rejectCommandsElligible();
    void checkNetwork(String);//(String str);
    // void checkStatusData(String &);
    String makeStatusMsg(byte batPer,byte network);

    bool checkEventGone();
    // void networkCounterMeasures();
    // void __attribute__((noinline)) watchdogConfig(uint8_t x);
    // static inline void watchdogReset();

    bool checkPrgReq(String,bool nomsg);
    void initRestartSeq();
    void jumpToBootloader() __attribute__ ((naked));
    void setObtainEvent();
    inline bool isCallReady(String);
    void checkRespSMS(char t1);

#ifndef disable_debug
#ifdef software_SIM
    HardwareSerial* _NSerial;
    SoftwareSerial* _SSerial;
#else
    SoftwareSerial* _NSerial;
    HardwareSerial* _SSerial;
#endif
#else
#ifdef software_SIM
    SoftwareSerial* _SSerial;
#else
    HardwareSerial* _SSerial;
#endif
#endif

  public:

    bool initialized;
    // -- Used with interrupt RING
    bool inInterrupt;
    unsigned long int tempInterruptTime;
    //---

#ifndef disable_debug
#ifdef software_SIM
    SIM(HardwareSerial* serial, SoftwareSerial* serial1);
#else
    SIM(SoftwareSerial* serial, HardwareSerial* serial1);
#endif
#else
#ifdef software_SIM
    SIM(SoftwareSerial* serial);
#else
    SIM(HardwareSerial* serial);
#endif
#endif
    void startSIMAfterUpdate();
    void sendUpdateStatus(byte);
    void sendCommand(char cmd, bool newline);
    void sendCommand(String cmd, bool newline);

    void setClassReference(S_EEPROM* e1, Motor_MGR* m1);
    bool initialize();

    bool registerEvent(char eventType);

    void operateOnMsg(String str, bool admin,bool noMsg);
    void setCallBackFunctions(void (*ImdEvent)(bool));
    bool busy();
    // void setNetLight(byte);

    void setMotorMGRResponse(char status);
    bool checkSleepElligible();
    bool checkNotInCall();
    void update();
};
#endif
