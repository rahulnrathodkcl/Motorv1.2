// To detect the presence of AC mains phase (R Y).
// along with phase (B Y) and (R B).
// Also report the status of motor, using (B Y) and (R B) phase lines.
// Also report single phase failure, and all out AC failure.



#ifndef Motor_MGR_h
#define Motor_MGR_h

#include "Definitions.h"
#include "S_EEPROM.h"
#include "SIM.h"
#include <Arduino.h>


class SIM;

class Motor_MGR
{
    S_EEPROM* eeprom1;
    SIM* sim1;

    #ifdef ENABLE_WATER
        #ifdef ENABLE_GP
            bool simEventTemp[19];
            char simEvent[19];
        #else
            bool simEventTemp[17];
            char simEvent[17];
        #endif

        #ifdef ENABLE_M2M
            byte m2mEvent[2];
            byte mapTable[2];
        #endif
    #else
        bool simEventTemp[12];
        char simEvent[12];
    #endif
    
    bool gotOffCommand;
    bool gotOnCommand;
    bool offButtonPressed;

    bool allPhase;
    bool mFeedback;
    bool phaseAC;

    bool startTimerOn;
    unsigned long int tempStartTimer;

    // bool stopTimerOn;
    // unsigned long int tempStopTimer;
    // unsigned int stopTimerTime;

    // bool waitCheckACTimerOn;
    // unsigned long tempWaitCheckACTimer;
    // byte waitCheckACTime;

    bool singlePhasingTimerOn;
    unsigned long int tempSinglePhasingTimer;
    byte singlePhasingTime;

    bool startSequenceOn;
    bool starDeltaTimerOn;
    unsigned long int tempStartSequenceTimer;
    byte startSequenceTimerTime;

    bool stopSequenceOn;
    unsigned long int tempStopSequenceTimer;
    byte stopSequenceTimerTime;

    bool waitStableLineOn;
    unsigned long int waitStableLineTimer;
    byte waitStableLineTime;

    unsigned long lastPressTime;
    byte lastButtonEvent;
    
    #ifdef ENABLE_WATER
        byte undergroundLevel;
        // bool lowLevelSensor;
        // bool midLevelSensor;
        // bool highLevelSensor;

        #ifdef ENABLE_GP
            byte overheadLevel;
            // bool oLowLevelSensor;
            // bool oHighLevelSensor;
        #endif
        byte waterEventBufferTime;
    
        void readWaterSensorState(bool &low,bool &mid,bool &high);
        void updateUndergroundLevel(byte level);
        // void updateWaterSensorState(bool &low,bool &mid,bool &high);
        #ifdef ENABLE_GP
            // void overHeadLowSensorState(bool);
            // void overHeadHighSensorState(bool);

            void readOverHeadWaterSensorState(bool &olow,bool &ohigh);
            void updateOverheadLevel(byte level);

            // void updateOverHeadWaterSensorState(bool &olow,bool &ohigh);            
            byte getOverHeadWaterSensorState();
        #endif
        // void lowSensorState(bool);
        // void midSensorState(bool);
        // void highSensorState(bool);
        void setWaterDefaults();
        byte getWaterSensorState();
        void operateOnWaterEvent();
    #endif

    void anotherConstructor(SIM* sim1, S_EEPROM* eeprom1);
    void readSensorState(bool &p1, bool &p2, bool &p3);
    void updateSensorState(bool &p1, bool &p2, bool &p3);
    void triggerAutoStart();
    void motorState(bool);
    void ACPowerState(bool);
    void AllPhaseState(bool);

    void operateOnEvent();
    void operateOnButtonEvent();

    bool waitStableLineOver();
    void operateOnStableLine();

    bool startMotorTimerOver();
    // bool stopMotorTimerOver();
    // void setACPowerState(bool b);

    bool singlePhasingTimerOver();
    void operateOnSinglePhasing();
    bool waitCheckACTimerOver();
    void unknownMotorOff();

    void terminateStopRelay();
    void terminateStartRelay();
    void terminateStarDeltaTimer();
    #ifdef ENABLE_M2M
        void M2MEventManager();
    #endif

    void SIMEventManager();
    void setLED(bool);
    inline void buttonFilter();


#ifndef disable_debug
#ifdef software_SIM
    HardwareSerial* _NSerial;
#else
    SoftwareSerial* _NSerial;
#endif
#endif

  public:

#ifndef disable_debug
#ifdef software_SIM
    Motor_MGR(HardwareSerial *s, SIM* sim1, S_EEPROM* eeprom1);
#else
    Motor_MGR(SoftwareSerial *s, SIM* sim1, S_EEPROM* eeprom1);
#endif
#else
    Motor_MGR(SIM* sim1, S_EEPROM* eeprom1);
#endif

    volatile bool eventOccured;
    volatile bool buttonEventOccured;
    
    #ifdef ENABLE_WATER
        volatile bool waterEventOccured;  
        unsigned long tempWaterEventTime;
        // bool lowSensorState();
        // bool midSensorState();
        // bool highSensorState();
        void waterStatusOnCall();
        #ifdef ENABLE_GP
            // bool overHeadLowSensorState();
            // bool overHeadHighSensorState();
            void overHeadWaterStatusOnCall();
        #endif

        #ifdef ENABLE_M2M
            void setM2MEventState(byte eventNo, byte state);
        #endif
    #endif


    void resetAutoStart(bool setChange = false);
    void waterEvent();

    bool motorState();
    bool ACPowerState();
    bool AllPhaseState();
    byte checkLineSensors();
    bool getMotorState();
    void startMotor(bool commanded = false);
    void stopMotor(bool commanded = false, bool forceStop = false,bool offButton=false);
    void statusOnCall();
    bool checkSleepElligible();
    void update();
};
#endif