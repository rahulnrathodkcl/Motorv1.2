//Version 2 Dated : 29052016
#include "SIM.h"
//#include <SoftwareSerial.h>
//#include "Defintions.h"
//#include "Defintions.h"
/*
  Gets the Phone Numbers From the EEPROM Class.
  Use #use_mega to use for SELF START with Serial And Serial1
  Use #disable_debug to remove data sent to Serial port in SELF START AND SWITCH OFF.
*/

  #ifndef disable_debug

    #ifdef software_SIM
      SIM::SIM(HardwareSerial* serial,SoftwareSerial* serial1)
      {
        _NSerial=serial;
        _NSerial->begin(19200);
        _SSerial=serial1;
        _SSerial->begin(19200);
        anotherConstructor();
      }
    #else
      SIM::SIM(SoftwareSerial* serial,HardwareSerial* serial1)
      {
        _NSerial=serial;
        _NSerial->begin(19200);
        _SSerial=serial1;
        _SSerial->begin(19200);
        anotherConstructor();
      }
    #endif

  #else
      #ifdef software_SIM
        SIM::SIM(SoftwareSerial* serial)
        {
          _SSerial=serial;
          _SSerial->begin(19200);
          anotherConstructor();
        }
      #else
        SIM::SIM(HardwareSerial* serial)
        {
          _SSerial=serial;
          _SSerial->begin(19200);
          anotherConstructor();
        }
      #endif
  #endif


void SIM::anotherConstructor()
{
  adminNumber="7698439201";
  initialized=false;

  acceptCommandsTime=200;
  commandsAccepted=false;
 
  //  starPresent=false;
  // pinMode(PIN_RING,INPUT);
  pinMode(PIN_DTR,OUTPUT);
  rejectCommands();

  soundWaitTime = 5;
  bplaySound = false;

  callDialled=false;
  attemptsToCall=0;

  actionType = 'N';
  makeResponse = false;
  responseToAction=false;

  callCutWaitTime = 580;
  nr = 0;
  currentStatus = 'N';
  currentCallStatus = 'N';
  callAccepted = false;
  freezeIncomingCalls=false;
  obtainNewEvent=true;
}

void SIM::delAllMsg()
{
  sendCommand("AT+CMGDA=\"DEL ALL\"",true);
}

bool SIM::isNumeric(String &str)
{
  for(byte i=0;i<str.length();i++)
  {
    if( !isDigit(str.charAt(i)))
      return false;
  }
  return true;
}

void SIM::operateOnMsg(String str,bool admin=false)
{
    if((stringContains(str,"CLEARALL",8,str.length()-1)) && admin)//if (str == "CLEARALL" && admin)
    {
      eeprom1->clearNumbers(admin);
    }
    else
    {
      int data;
      if(stringContains(str,"TEMP",4,str.length()-1))
      {
        if(isNumeric(str))
        {
            data=str.toInt();
            if(data<50) data=50;
            if(data>80) data=80;
            eeprom1->saveTempSettings(data);  //Store in EEPROM the TEMP value            
        }
      }
      else if(stringContains(str,"DEFAULT",7,str.length()-1))
      {
          eeprom1->saveAutoStartSettings(true);
          eeprom1->saveDNDSettings(false);
          eeprom1->saveResponseSettings('C');
          eeprom1->saveStartVoltageSettings(620);
          eeprom1->saveStopVoltageSettings(690);  
          eeprom1->saveAutoStartTimeSettings(30);  
      }
      else if(stringContains(str,"AUTOON",6,str.length()-1))
          eeprom1->saveAutoStartSettings(true);  //set AutoStart to True in EEPROM            
      else if(stringContains(str,"AUTOOFF",7,str.length()-1))
          eeprom1->saveAutoStartSettings(false);  //set AUtoStart to False in EEPROM
      else if(stringContains(str,"DNDON",5,str.length()-1))
          eeprom1->saveDNDSettings(true);  //set DND to true in EEPROM
      else if(stringContains(str,"DNDOFF",6,str.length()-1))
          eeprom1->saveDNDSettings(false);  //set DND to False in EEPROM
      else if(stringContains(str,"RESPC",5,str.length()-1))
          eeprom1->saveResponseSettings('C');  //set DND to False in EEPROM
      else if(stringContains(str,"RESPA",5,str.length()-1))
          eeprom1->saveResponseSettings('A');  //set DND to False in EEPROM
      else if(stringContains(str,"BATSTART",8,str.length()-1))
          eeprom1->saveStartVoltageSettings(str.toInt());  //set DND to False in EEPROM
      else if(stringContains(str,"BATSTOP",7,str.length()-1))
          eeprom1->saveStopVoltageSettings(str.toInt());  //set DND to False in EEPROM
      else if(stringContains(str,"STATUS",6,str.length()-1))
      {
          bool t3 = eeprom1->ACPowerState();
          bool t4= motor1->getChargeState();
          bool t5 = eeprom1->motorState();
          float t6 = motor1->getBatVolt();
          unsigned short int t7;
          t6 = t6/100.0;
          t7=t6;
          String resp;
          resp = "AC:";
          resp = resp + (t3?" ON\n":" OFF\n");
          resp = resp + "CHARGING:";
          resp = resp + (t4?" ON\n":" OFF\n");
          resp = resp + "MOTOR:";
          resp = resp + (t5?" ON\n":" OFF\n");          
          resp = resp + "BATTERY:";
          resp = resp + t7;
          resp = resp + ".";
          t7 = ((t6-t7)*100);
          resp = resp + t7;
          sendSMS(resp,true);
      }
      else if(stringContains(str,"AUTOTIME",8,str.length()-1))
      {
        if(isNumeric(str))
        {
            data=str.toInt();
            if(data<0) data=0;
            if(data>480) data=480;
            eeprom1->saveAutoStartTimeSettings(data);  //Store in EEPROM the AUTO START TIME
        }
      }
      else if(stringContains(str,"AT+CSQ",6,str.length()-1))
      {
        sendCommand("AT+CSQ",true);
        sendCSQResponse=true;
      }
      else if(stringContains(str,"BAL",3,str.length()-1))
      {
        String s2;
        s2="AT+CUSD=1";
        sendCommand(s2,true);

        s2.concat(",\"");
        s2.concat(str);
        s2.concat("\"");
        sendCUSDResponse=true;
        sendCommand(s2,true);
      }

      if(stringContains(str,"M-",2,12))
      {
        if(str.length()==10 && isNumeric(str))
        {
          bool t=eeprom1->removeNumber(str);
          #ifndef disable_debug
            _NSerial->print("Remove:");
            _NSerial->println((bool)t);
          #endif
        }
      }
      else if (stringContains(str,"M+",2,12))
      {
        if (str.length() == 10 && isNumeric(str))
        {
          bool t=eeprom1->addNumber(str);
          #ifndef disable_debug
            _NSerial->print("Add:");
            _NSerial->println((bool)t);
          #endif
        }
      }
      else if(stringContains(str,"AMON",4,str.length()-1))
      {
        if(eeprom1->alterNumberPresent)
          eeprom1->saveAlterNumberSetting(true);
      }
      else if(stringContains(str,"AMOFF",5,str.length()-1))
      {
        eeprom1->saveAlterNumberSetting(false);
      }
      else if (stringContains(str,"AM+",3,13))
      {
        if (str.length() == 10 && isNumeric(str))
        {
          bool t=eeprom1->addAlternateNumber(str);
          #ifndef disable_debug
            _NSerial->print("Alter ");
            _NSerial->print("Add:");
            _NSerial->println((bool)t);
          #endif
        }
      }
    }
}

inline bool SIM::isCUSD(String &str)
{
  return stringContains(str,"+CUSD:",6,str.length()-1);
}

inline bool SIM::isCSQ(String &str)
{
  return stringContains(str, "+CSQ", 5, str.length()-3);
}

inline void SIM::sendReadMsg(String str)
{
  String s;
  s="AT+CMGR=";
  s.concat(str);
  sendCommand(s,true);
}

inline bool SIM::isMsgBody(String &str)
{
  return stringContains(str,"+CMGR:",24,34);
}

bool SIM::isAdmin(String str)
{
  if(str==adminNumber)
    return true;
  return false;
}

bool SIM::isPrimaryNumber(String str)
{
  if(eeprom1->numbersCount>0)
  {
    if(eeprom1->primaryNumber==str)
      return true; 
    if (eeprom1->alterNumberSetting && eeprom1->alterNumber==str)
      return true;
  }
  return false;
}

void SIM::gotMsgBody(String &str)
{
  bool admin=isAdmin(str);
  if(admin || isPrimaryNumber(str))
  {
    str=readString();//_SSerial->readStringUntil('\n');
    #ifndef disable_debug
      _NSerial->print("MSGRCV:");
      _NSerial->println(str);
    #endif
    operateOnMsg(str,admin);
  }
  delAllMsg();
}

bool SIM::isNewMsg(String &str)
{
  //+CMTI: "SM",1
  return stringContains(str,"+CMTI:",12,str.length()-1);
}

void SIM::setClassReference(S_EEPROM* e1,Motor_MGR* m1)
{
  eeprom1=e1;
  motor1=m1;
}

bool SIM::initialize()
{
  byte attempts=0;
  try_again:
  if (sendBlockingATCommand("AT\r\n"))
  {
      if (sendBlockingATCommand("AT+CLIP=1\r\n"))
      {
        if (sendBlockingATCommand("AT+CLCC=1\r\n") && sendBlockingATCommand("AT+CMGF=1\r\n") &&  sendBlockingATCommand("AT+CNMI=2,1,0,0,0\r\n") )
        {
          #ifndef disable_debug
          _NSerial->println("INIT OK");
          #endif
          initialized=true;
          return true;
        }
      }
  }
  if(!initialized && attempts==0)
  {
    attempts++;
    goto try_again;  
  }
  #ifndef disable_debug
    _NSerial->println("INIT UNSUCCESSFULL");
  #endif
  return false;
}

bool SIM::isNumber(String &str)
{
return (stringContains(str, "+CLIP: \"", 11, 21));
}

bool SIM::checkNumber(String number)
{
  #ifndef disable_debug
    _NSerial->print("Number:");
    _NSerial->println(number);
  #endif

  if (number==adminNumber)
    return true;
  else if(eeprom1->checkExists(number)!=0xFF)
    return true;
  
  return false;
}

void SIM::acceptCommands()
{
  if(!commandsAccepted)
  {
    #ifndef disable_debug
      _NSerial->print("com");
      _NSerial->print("Acc:");
      _NSerial->println(millis());
    #endif
    //digitalWrite(PIN_DTR,LOW);
    commandsAccepted=true;
    tempAcceptCommandTime=millis();  
    while(millis()-tempAcceptCommandTime<=100)
    {}
  }
  else
  {
    tempAcceptCommandTime=millis();  
  }
}

void SIM::rejectCommands()
{
  #ifndef disable_debug
    _NSerial->print("com");
    _NSerial->print("Reject");
    _NSerial->println(millis());
  #endif
  //digitalWrite(PIN_DTR,HIGH);
  commandsAccepted=false;
}
void SIM::sendCommand(char cmd,bool newline=false)
{
  acceptCommands();
    if(!newline)
    _SSerial->print(cmd);
    else
    _SSerial->println(cmd);
}

void SIM::sendCommand(String cmd,bool newline=false)
{
  acceptCommands();
  if(cmd=="")
    _SSerial->println();
  else
  {
    if(!newline)
      _SSerial->print(cmd);
    else
      _SSerial->println(cmd);
  }
}

bool SIM::sendBlockingATCommand(String cmd)
{
  sendCommand(cmd);
  #ifndef disable_debug
  _NSerial->print(cmd);
  #endif

  unsigned long t = millis();
  String str;
  while (millis() - t < 3000)
  {
    if (_SSerial->available() > 0)
    {
      str = readString();
      #ifndef disable_debug
      _NSerial->println(str);
      #endif

      if (matchString(str, "OK\r") == true)
        return true;
      else if (matchString(str, "ERROR\r") == true)
        return false;
    }
  }
  return false;
}

String SIM::readString()
{
  String str = "";
  if (_SSerial->available() >  0)
  {
    acceptCommands();
    str = _SSerial->readStringUntil('\n');
    #ifndef disable_debug
    _NSerial->println(str);
    #endif
  }
  return str;
}

bool SIM::matchString(String m1, String m2)
{
  return (m1 == m2);
}

bool SIM::stringContains(String &sstr, String mstr, int sstart, int sstop)
{
  if (sstr.startsWith(mstr))
  {   
        sstr = sstr.substring(sstart, sstop);
    return true;
  }
  return false;
}

bool SIM::isRinging(String str)
{
  return(str == "RING\r");
}

bool SIM::isDTMF(String &str)
{
  return stringContains(str, "+DTMF: ", 7, 8);
}

bool SIM::isCut(String str)
{
  if(currentStatus=='I' && (currentCallStatus=='I' || currentCallStatus=='O'))
  {
    #ifndef disable_debug
    _NSerial->print("STR RCV : ");
    _NSerial->print(str);
    _NSerial->println("STR END");
    #endif
  }

  if(matchString(str, "NO CARRIER\r"))
    return true;
  else if(matchString(str, "BUSY\r"))
    return true;
  else if(matchString(str, "NO ANSWER\r"))
    return true;
  else if(matchString(str, "ERROR\r"))
    return true;
  return false;
}

bool SIM::isSoundStop(String str)
{
  return (matchString(str, "+CREC: 0\r"));
}

char SIM::callState(String str)
{
  #ifndef disable_debug
  _NSerial->print("str:");
  _NSerial->println(str);
  #endif


  if (stringContains(str, "+CLCC: 1,0,2", 11, 12))    //dialling
    return 'D'; //call made
  if (stringContains(str, "+CLCC: 1,0,3", 11, 12))    //alerting
    return 'R'; //call made
  else if (stringContains(str, "+CLCC: 1,0,0", 11, 12)) //active call
    return 'I'; //call accepted
  else if (stringContains(str, "+CLCC: 1,0,6", 11, 12)) //call ended
    return 'E'; //call ended
  else
    return 'N';
}

String SIM::getActiveNumber()
{
    if(eeprom1->numbersCount>0)
    {
      if(!eeprom1->alterNumberSetting)
        return(eeprom1->primaryNumber);
      else
        return(eeprom1->alterNumber);
    }
    else
     return(adminNumber);//="AT+CMGS=\"+917698439201\"";  
}

void SIM::makeCall()
{
  acceptCommands();
  _SSerial->flush();
 
    String command;
    command="ATD+91";
    command.concat(getActiveNumber());
    command.concat(";");
    sendCommand(command,true);

    double temp=millis();
    while(millis()-temp<100)
    {    }
    sendCommand("",true);
    #ifndef disable_debug
    _NSerial->print("Call");
    _NSerial->println("Made");
    #endif
    eeprom1->inCall(true);
    callCutWait = millis();
    currentStatus = 'R';
    currentCallStatus = 'O';
    attemptsToCall++;
}

void SIM::endCall()
{
  nr = 0;
  _SSerial->flush();
  sendCommand("AT+CHUP",true);
  double temp=millis();
  while(millis()-temp<100)
  {  }
  sendCommand("",true);
  _SSerial->flush();
  freezeIncomingCalls=false;
  callDialled=false;
  attemptsToCall=0;

  eeprom1->inCall(false);
  callAccepted = false;
  responseToAction=false;
  currentStatus = 'N';
  currentCallStatus = 'N';
  
  obtainEventTimer=millis();
  obtainNewEvent=false;
//  starPresent=false;
  #ifndef disable_debug
  _NSerial->print("Call");
  _NSerial->println("End");
  #endif
}

void SIM::setObtainEvent()
{
  if(!obtainNewEvent  && millis()-obtainEventTimer>1000)
      obtainNewEvent=true;
}

void SIM::acceptCall()
{
    callAccepted = true;
    _SSerial->flush();
    sendCommand("ATA",true);
  _SSerial->flush();
  currentStatus = 'I';
  currentCallStatus = 'I';
  playSound('M');
}

void SIM::sendSMS(String msg="",bool predefMsg=false)
{
  _SSerial->flush();
  String responseString;

  if(!predefMsg)
  {
    switch (actionType)
    {
      case 'S':
        responseString = STR_MOTOR;
        responseString += STR_ON;
        break;
      case 'O':
      case 'U':
        responseString = STR_MOTOR;
        responseString += STR_OFF;
        break;
      default:
        return;
    }
  }
  else
    responseString=msg;

  #ifndef disable_debug
  _NSerial->println("SMS..");
  #endif
    String command;
    command="AT+CMGS=\"+91";
    command.concat(getActiveNumber());
    command.concat("\"");

    _SSerial->flush();
    sendCommand(command,true);
    _SSerial->flush();
    unsigned long int temp=millis();
    while(millis()-temp<1000)
    {}
    sendCommand(responseString,true);
    _SSerial->flush();
    temp=millis();
    while(millis()-temp<1000)
    {}
    _SSerial->flush();
    sendCommand((char)26,true);
    _SSerial->flush();    
}

void SIM::operateDTMF(String str)
{
//      // if(starPresent)
      // {
      //   byte temp = str.toInt();
      //   if(temp==9)
      //   {
//      //     starPresent=false;
      //     callCutWait=millis();
      //     stopSound();
      //     playSound('M');
      //     return;
      //   }
      //   return;
      // }

      if (str == "1") //Motor On
      {
          currentOperation='S';
          subDTMF();
          motor1->startMotor(true);
      }
      else if (str == "2") //Motor Off
      {
          currentOperation='O';
          subDTMF();
          motor1->stopMotor(true);
      }
      else if (str == "3") //Status
      {
          currentOperation='T';
          subDTMF();
          motor1->statusOnCall();
      }
}

inline void SIM::subDTMF()
{
//    // starPresent=true;
    callCutWait=millis();
    stopSound();  
}

void SIM::operateRing()
{
  nr++;
  if (nr <= 2)
  {

    if(nr==1)
    {
      sendCommand("AT+DDET=1\r\n");
      _SSerial->flush();
    }
    
    String str;
    do
    {
      str = readString();
    }
    while (isNumber(str) == false);
    
    callCutWait=millis();

    if(str.length()>=10 && isNumeric(str))
    {
      if (nr>1 && !checkNumber(str))
        endCall();
    }
  }
  else if(nr==3)
  {
      callCutWait = millis();
      acceptCall();     
  }
}

bool SIM::playSoundElligible()
{
  return (bplaySound == true && ((millis() - soundWait) > (soundWaitTime * 100)));
}

void SIM::triggerPlaySound()
{
  _SSerial->flush();
  sendCommand("AT+CREC=4,\"");
  sendCommand("C:\\");
  sendCommand(playFile);
  sendCommand(".amr\",0,100,1\r",true);
  _SSerial->flush();
  bplaySound = false;
}

// void SIM::playSoundAgain(String str)
// {
//   if (isSoundStop(str))
//   {
////       if (starPresent)
//       {
//           if(playFile==actionType)
//             playFile='N';
//           else
//             playFile=actionType;
//       }
//       playSound(playFile);
//   }
// }

void SIM::playSound(char actionType,bool newAction)
{
  _SSerial->flush();
  soundWait = millis();
  bplaySound = true;
  if(newAction)
    this->actionType=actionType;
  playFile = actionType;
}

void SIM::stopSound()
{
  _SSerial->flush();
  sendCommand("AT+CREC=5\r",true);
  _SSerial->flush();
}

bool SIM::callTimerExpire()
{
  return ((millis() - callCutWait) >= (callCutWaitTime*100));
}

void SIM::makeResponseAction()
{
  makeResponse = false;
  if (eeprom1->RESPONSE == 'A' || eeprom1->RESPONSE == 'C')
    makeCall();
}

bool SIM::registerEvent(char eventType)
{
    if(!initialized)
    {
      #ifndef disable_debug
      _NSerial->println("NO SIM");
      #endif    
      return true;
    }

    if(currentStatus=='N' && currentCallStatus=='N' && obtainNewEvent)
    {
      freezeIncomingCalls=true;
      acceptCommands();

      #ifndef disable_debug
      _NSerial->print("Imm event:");
      _NSerial->print(eventType);
      #endif
      
      actionType = eventType;
      makeResponseAction();
      return true;
    }
    else
      return false;
}


bool SIM::rejectCommandsElligible()
{
  return (commandsAccepted && millis()-tempAcceptCommandTime>=(acceptCommandsTime*100));
}

void SIM::checkNetwork(String str)
{
  if(str=="+CPIN: NOT READY\r")
  {
    endCall();
    networkCounterMeasures();
  }
}

void SIM::networkCounterMeasures()
{
  _SSerial->flush();
  sendCommand("AT+CFUN=0\r\n");
  _SSerial->flush();
  unsigned long t=millis();
  while(millis()-t<20)
  {}
  sendCommand("AT+CFUN=1\r\n");
  _SSerial->flush();
  while(millis()-t<20)
  {}
}

void SIM::setMotorMGRResponse(char response)
{
  if(currentOperation=='S')  //start Motor
  {
    responseToAction=true;
    if(response=='L')
      playSound('N');  //cannot start motor
    else if(response=='O')
      playSound('1');  //motor is already on
    else if(response=='D')
      playSound('S');  //motor has started
      // endCall();
  }
  else if(currentOperation=='O') //switch off motor
  {
    responseToAction=true;
    if(response=='L')
      playSound('P');    //cannot stop motor
    else if(response=='O')
      playSound('2');  //motor is already off
    else if(response=='D')
      playSound('O');  //motor has stopped
      // endCall();
  }
  else if (currentOperation='T')
  {
    responseToAction=true;
    if(response=='L')   //motor off, no light
      playSound('L');
    else if(response=='O')   //motor off, light on
      playSound('3');
    else if(response=='D')
      playSound('1');  //motor is on
  }
}

void SIM::update()
{
  if(rejectCommandsElligible())
  {
    rejectCommands();
  }

  if (currentStatus == 'N')
  {
    setObtainEvent();

    if (makeResponse)
      makeResponseAction();
  }
  else if (currentStatus == 'I' || currentStatus == 'R')
  {
    if (callTimerExpire())
    {
      char t1=actionType;
      byte t4=attemptsToCall;
      if(!callDialled)
      {
        endCall();
        #ifndef disable_debug
          _NSerial->print("CALL DIAL");
          _NSerial->println("OFF");
        #endif
        if(t4<2)
        {
          #ifndef disable_debug
            _NSerial->println("RE REG EVENT");
          #endif
          attemptsToCall=t4;
          registerEvent(t1);
        }
        else if(eeprom1->RESPONSE=='A')
          sendSMS();
      }
      else
      {
        endCall();
      }
    }

    if (playSoundElligible())
      triggerPlaySound();
  }

  while (_SSerial->available() > 0)
  {
    String str;
    str=readString();
        
      if(isCUSD(str) && sendCUSDResponse)
      {
        sendCUSDResponse=false;
        sendSMS(str,true);
      }
      else if(isCSQ(str) && sendCSQResponse)
      {
        sendCSQResponse=false;
        unsigned short int t2 = str.toInt();
        t2=t2*3;
        str=" ";
        str=str+t2;
        sendSMS(str,true);
      }
      else if(isNewMsg(str))
      {
          sendReadMsg(str);
      }
      else if(isMsgBody(str))
      {
        gotMsgBody(str);
      }
      else
        checkNetwork(str);

    if (!freezeIncomingCalls &&  (currentStatus == 'N' || currentStatus == 'R') && (currentCallStatus == 'N' || currentCallStatus == 'I')) //Ringing Incoming Call
    {
      if (isRinging(str) == true) //  chk_ringing(str) == true)
      {
        eeprom1->inCall(true);
        currentStatus = 'R';
        currentCallStatus = 'I';
        operateRing(); 
      }
    }
    else if (!freezeIncomingCalls && currentStatus == 'I' && currentCallStatus == 'I') //IN CALL INCOMING CALL
    {
      if (isCut(str) == true ) //chk_cut(str) == true)
      {
        endCall();
      }
      else if (isDTMF(str) == true) //chk_DTMF(str) == true)
      {
        operateDTMF(str);
      }
      else
      {
        // playSoundAgain(str);
      }
    }
    else if ((currentStatus == 'N' || currentStatus == 'R') && currentCallStatus == 'O')
    {
      if(callState(str)=='D')
      { 
          #ifndef disable_debug
            _NSerial->print("CALL DIAL");
            _NSerial->println("ON");
          #endif
          callDialled=true;
      }
      else if (callState(str) == 'R')
      {
        callCutWait = millis();
        currentStatus = 'R';
        currentCallStatus = 'O';
      }
      else if (isCut(str) || callState(str) == 'E') //else if (isCut(str) || stringContains(str, "+CLCC: 1,0,6", 11, 12) == true)
      {
        endCall();
        if (eeprom1->RESPONSE == 'A')
            sendSMS();
      }
      else if (callState(str) == 'I') //else if (stringContains(str, "+CLCC: 1,0,0", 11, 12) == true)
      {
        #ifndef disable_debug
        _NSerial->println("Call Accepted");
        #endif
        callCutWait = millis();
        currentStatus = 'I';
        currentCallStatus = 'O';
        callAccepted = true;
        playSound(actionType);
      }
    }
    else if (currentStatus == 'I' && currentCallStatus == 'O') //IN CALL OUTGOING CALL
    {
      if (isCut(str) || callState(str) == 'E')
        endCall();
      else if (isDTMF(str)) //chk_DTMF(str) == true)
      {
        operateDTMF(str);
      }
      else
      {
        // playSoundAgain(str);
      }
    }
  }
}