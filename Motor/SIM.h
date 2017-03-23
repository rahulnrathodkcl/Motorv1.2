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
    String adminNumber;
    byte soundWaitTime; //x100 = mSec
    unsigned long soundWait;
    bool bplaySound;
    char playFile;

    //bool starPresent;

    unsigned short int callCutWaitTime;  //x100 = mSec
    unsigned long callCutWait;

    bool isMsgFromAdmin;
    //byte attemptsToCall;
    bool callDialled;
    bool sendCSQResponse;
    bool sendCUSDResponse;

    bool commandsAccepted;
    byte acceptCommandsTime;
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
    
    void acceptCommands();
    void rejectCommands();
    void sendCommand(char cmd,bool newline);
    void sendCommand(String cmd,bool newline);

    bool isCSQ(String &);
    bool sendBlockingATCommand(String);
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
    void sendSMS(String,bool);
    void operateDTMF(String str);
    inline void subDTMF();
    void operateRing();
    bool playSoundElligible();
    void triggerPlaySound();
    void playSoundAgain(String);
    void playSound(char c,bool x=true);
    void stopSound();
    bool callTimerExpire();
    void makeResponseAction();
    bool rejectCommandsElligible();
    void checkNetwork(String);//(String str);
    void networkCounterMeasures();

    void setObtainEvent();
    inline bool isCallReady(String);

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

    void setClassReference(S_EEPROM* e1,Motor_MGR* m1);
    bool initialize();

    bool registerEvent(char eventType);

    void operateOnMsg(String str,bool admin);
    bool isCUSD(String &str);
    void setCallBackFunctions(void (*ImdEvent)(bool));

    void setMotorMGRResponse(char status);
    void update();
};
#endif