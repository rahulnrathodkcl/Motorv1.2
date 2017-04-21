//Version 2 Dated : 29052016
#include "SIM.h"

/*
  Gets the Phone Numbers From the EEPROM Class.
*/
#ifndef disable_debug
#ifdef software_SIM
SIM::SIM(HardwareSerial* serial, SoftwareSerial* serial1)
{
  _NSerial = serial;
  _NSerial->begin(19200);
  _SSerial = serial1;
  _SSerial->begin(19200);
  anotherConstructor();
}
#else
SIM::SIM(SoftwareSerial* serial, HardwareSerial* serial1)
{
  _NSerial = serial;
  _NSerial->begin(19200);
  _SSerial = serial1;
  _SSerial->begin(19200);
  anotherConstructor();
}
#endif

#else
#ifdef software_SIM
SIM::SIM(SoftwareSerial* serial)
{
  _SSerial = serial;
  _SSerial->begin(19200);
  anotherConstructor();
}
#else
SIM::SIM(HardwareSerial* serial)
{
  _SSerial = serial;
  _SSerial->begin(19200);
  anotherConstructor();
}
#endif
#endif


void SIM::anotherConstructor()
{
  adminNumber = "7041196959";
  initialized = false;

  acceptCommandsTime = 200;
  commandsAccepted = false;

  // pinMode(PIN_RING,INPUT);
  pinMode(PIN_DTR, OUTPUT);
  rejectCommands();

  soundWaitTime = 5;
  bplaySound = false;

  callDialled = false;
  // attemptsToCall=0;

  actionType = 'N';
  makeResponse = false;
  responseToAction = false;

  callCutWaitTime = 580;
  nr = 0;
  currentStatus = 'N';
  currentCallStatus = 'N';
  callAccepted = false;
  freezeIncomingCalls = false;
  obtainNewEvent = true;
  isMsgFromAdmin = false;
  // acceptCommands();
}

void SIM::startSIMAfterUpdate(byte updateStatus)
{
  sendBlockingATCommand("AT+CFUN=1\r\n");
  String promptStr;

  if(updateStatus==0x01)
    promptStr=F("Update Success");
  else if(updateStatus==0x02)
    promptStr=F("Checksum Error");
  else if(updateStatus==0x03)
    promptStr=F("Max Attempt Exceed");

    #ifndef disable_debug
      _NSerial->println(promptStr);
    #endif
    // sendSMS(promptStr,true);
}

void SIM::delAllMsg()
{
  sendCommand("AT+CMGDA=\"DEL ALL\"", true);
}

bool SIM::isNumeric(String &str)
{
  for (byte i = 0; i < str.length(); i++)
  {
    if ( !isDigit(str.charAt(i)))
      return false;
  }
  return true;
}

// bool SIM::connectToServer(String serverIpAddress)
// {
//   String cmd="AT+CIPSTART=\"TCP\",\"";
//   cmd=cmd+serverIpAddress;
//   cmd = cmd + "\",80";

//   unsigned long int temp = millis();
//   if(sendBlockingATCommand(cmd))
//   {
//     while(millis()-temp<15000)
//     {
//         cmd=readString();
//         if(cmd=="CONNECT OK\r")
//           return true;
//     }
//   }
//   return false;
// }

// bool SIM::sendHTTPRequest(String req)
// {
//   bool gotresp=false;
//   unsigned long int temp = sizeof(req);
//   String cmd="AT+CIPSEND=" + temp;
//   temp=millis();
//   sendCommand(cmd);
//   while(millis()-t<2000)
//   {
//       cmd=readString();
//       if(cmd==">\r")
//       {
//         gotresp=true;
//         break;
//       }
//   }

//   if(gotresp)
//   {
//     sendCommand(req);
//     return true;
//   }
//   else
//     return false;
// }


// byte SIM::getProgInfo(String fileName)
// {
//   String cmd="GET ";
//   cmd=cmd+fileName;
//   cmd=cmd + " ";
//   cmd = cmd + "HTTP/1.0";
//   byte retValue;

//   if(sendHTTPRequest(cmd))
//   {
//     unsigned long int temp = millis();
//     bool gotStartSymbol=false;
//     bool gotStopSymbol=false;
//     while(millis()-temp<20000)
//     {
//       if(_SSerial->available())
//       {
//         cmd=_SSerial->readStringUntil('\n');
//         if(cmd=="START\r")
//         {
//           temp=millis();
//           gotStartSymbol=true;
//           continue;
//         }
//         else if(cmd=="STOP\r")
//         {
//             gotStopSymbol=true;
//             break;
//         }
//         if(gotStartSymbol)
//         {        
//           if(stringContains(newData, "MSIGN", 5, str.length() - 1))
//           {
//               if(stringContains(newData,"MEGA328P",8,str.length()-1))
//               {
//                   if(!eeprom1->checkSign(MS_MEGA328P))
//                     return INVALID_SIGN;
//               }
//               else if(stringContains(newData,"MEGA128A",8,str.length()-1))
//               {
//                   if(!eeprom1->checkSign(MS_MEGA128A))
//                       return INVALID_SIGN;
//               }
//           }
//           else if(stringContains(newData, "PROGSIZE", 8, str.length() - 1))
//           { 
//               unsigned long int temp = newData.toInt();
//               if(temp<=0)
//                 return INVALID_SIZE;
//               eeprom1->saveProgSize(temp);
//           }
//         }
//       }
//     }
//     return (INVALID_SIZE+INVALID_SIGN);
//   }
//   return 0;
// }

// bool SIM::getFile(byte fileType, String fileName,String &firmwareFile)
// {
//   String cmd="GET ";
//   cmd=cmd+fileName;
//   cmd=cmd + " ";
//   cmd = cmd + "HTTP/1.0";

//   if(sendHTTPRequest(cmd))
//   {
//     unsigned long int temp = millis();
//     bool gotStartSymbol=false;
//     bool gotStopSymbol=false;
//     while(millis()-temp<20000)
//     {
//       if(_SSerial->available())
//       {
//         if(!gotStartSymbol)
//         {
//             cmd=_SSerial->readStringUntil('\n');
//             if(cmd=="START\r")
//             {
//               temp=millis();
//               gotStartSymbol=true;
//               continue;
//             }
//         }
//         else if(cmd=="STOP\r")
//         {
//             gotStopSymbol=true;
//             break;
//         }
      
//         if(gotStartSymbol)
//         {        
//           decodeString(cmd);
//           cmd=cmd+"\n";
//           firmwareFile=firmwareFile+cmd;
//         }
//       }
//     }
//     if(gotStartSymbol && gotStopSymbol)
//       return true;
//   }
//   return false;
// }

// bool SIM::decodeString(byte fileType,String &newData, String &returnFile)
// {
//   switch(fileType)
//   {
//     case FT_INFO:
      
//     break;

//     case FT_PROG:
//         if()
//     break;
//   }
//   return true;
// }

// void SIM::getProgramSize(String ipaddress,String version)
// {
//   String fileName=ipaddress;
//   fileName=fileName + "/";
//   fileName = fileName+ Version;
//   fileName = fileName + "/";
//   fileName = fileName + "progDesc.txt";

//   String data;
//   if(getFile(fileName,data))
//   {

//     return true;
//   }
//   return false;
// }

// void SIM::downloadFirmware()
// {
// }

bool SIM::extendedSendCommand(String cmd,byte timeout)
{
  sendCommand(cmd,false);
  unsigned long temp =millis();
  while(millis()-temp< timeout*100)
  {
    if(_SSerial->available())
    {
      String str = _SSerial->readStringUntil('\n');
      if(str=="OK\r")
        return true;
      else if(str=="ERROR\r")
        return false;
    }
  }
  return false;
}

bool SIM::startGPRS(String apn)
{
    //AT+SAPBR=3,1,"ConType","GPRS"
  	// String m1="AT+SAPBR=3,1,\"";
  	// String cmd;
  	// cmd=m1;
  	// cmd=cmd+"ConType\",\"GPRS\"\r\n";
    String cmd=F(STR_SAPBR_PARAM);
    cmd = cmd + F(STR_SAPBR_GPRS);
    if(extendedSendCommand(cmd,50))
    {
        cmd = F(STR_SAPBR_PARAM);
        cmd = cmd + F(STR_SAPBR_APN);
        cmd = cmd + apn;
        cmd = cmd + "\"\r\n";
        //AT+SAPBR=3,1,"APN","bsnlnet"
        if(extendedSendCommand(cmd,50))
        {
            if(sendBlockingATCommand(F(STR_SAPBR_START),true))
                return true;
        }
    }
  return false;
}

bool SIM::stopGPRS()
{
		return(sendBlockingATCommand(F(STR_SAPBR_STOP),true));
}

bool SIM::connectToFTP(String ipaddress)
{
  String cmd;
  //AT+FTPCID=1
  if(sendBlockingATCommand(F("AT+FTPCID=1\r\n"),true))
  {
    //AT+FTPSERV="43.252.117.34"
    cmd=F("AT+FTPSERV=\"");
    cmd=cmd+ipaddress;
    cmd=cmd+"\"\r\n";
    if(sendBlockingATCommand(cmd,true))
    {
      // AT+FTPUN="FTP-User"
      // AT+FTPPW="123456789"
      if(sendBlockingATCommand(F("AT+FTPUN=\"rahulnrathodkcl\"\r\n"),true))
      {
        if(sendBlockingATCommand(F("AT+FTPPW=\"123456789\"\r\n"),true))
        {
            return true;
        }
      }
    }
  }
  return false;
}

bool SIM::setFile(String filename)
{
		String m1=F("AT+FTPGET");
    String cmd;
    cmd=m1;
    cmd=cmd + "NAME=\"";   //m.bin\"\r\n;
    cmd=cmd + filename;
    cmd= cmd + "\"\r\n";

    if(sendBlockingATCommand(cmd,true))
    {
    		cmd=m1;
    		cmd=m1+"PATH=\"/\"\r\n";
        if(sendBlockingATCommand(cmd),true)
            return true;  
    }
    return false;
}

bool SIM::getProgramSize()
{

  if(setFile("m.hex"))
  {
  	if(eeprom1->programSizeSet)
  		return true;
      // AT+FTPSIZE
      // OK
      // +FTPSIZE: 1,0,22876
      // String str ="AT+FTPSIZE\r\n";
      // String str2="+FTPSIZE: 1,0,";

      // if(extendedSendCommand(str,str2,14,60000))
      // {
      //     eeprom1->saveProgramSize(str.toInt());
      //     return true;
      // }

      unsigned long int temp=millis();
      sendCommand(F("AT+FTPSIZE\r\n"),false);
      String str;
      while(millis()-temp<120000L)
      {
          if(_SSerial->available())
          {
              str=_SSerial->readStringUntil('\n');
              #ifndef disable_debug
                _NSerial->println(str);
              #endif
              if(stringContains(str,F("+FTPSIZE: 1,0,"),14,str.length()-1))
              {
                eeprom1->saveProgramSize(str.toInt());
                #ifndef disable_debug
                  _NSerial->println(str);
                #endif
                return true;
              }
          }        
      }
  }
  return false;
}

// bool SIM::extendedSendCommand(String &cmd,String vstr,unsigned short int len,unsigned short int timeout)
// {
//   sendCommand(cmd,false);
//   unsigned long int temp=millis();
//   String str;
//     while(millis()-temp<timeout)
//     {
//         if(_SSerial->available())
//         {
//             str=_SSerial->readStringUntil('\n');
//             if(stringContains(str,vstr,len,cmd.length()-1))
//             {
//                 cmd=str;
//                 return true;
//             }
//         }          
//     }
//     return false;
// }

bool SIM::downloadFirmware()
{
    unsigned long int size=eeprom1->getProgramSize();
    unsigned long int temp;
    
    //  +FTPGETTOFS: 0,2396
    String m1="+FTPGETTOFS";
    String cmd;
    String v1= m1 + ": 0,";
    v1 = v1 + size;
    v1 = v1 + "\r";

    // AT+FTPGETTOFS=0,"m.hex"\r\n
    cmd="AT"; 
    cmd=cmd+m1;
    cmd=cmd+ "=0,\"m.hex\"\r\n";
    temp=millis();
    sendCommand(cmd,false);
    while(millis()-temp<120000L)
    {
        if(_SSerial->available())
        {
            cmd=_SSerial->readStringUntil('\n');
            #ifndef disable_debug
                _NSerial->println(cmd);
            #endif
            if(cmd==v1)
            {
              #ifndef disable_debug
                _NSerial->println("DC");
              #endif   
                return true;
            }
        }          
    }
    return false;
    // return extendedSendCommand(cmd,v1,25,60000);           
}

bool SIM::isGPRSConnected()
{
    
  //+SAPBR: 1,1,"0.0.0.0"
  sendCommand("AT+SAPBR=2,1\r\n",false);
  unsigned long int temp =millis();
  while(millis()-temp<5000)
	{
		if(_SSerial->available())
		{
			String str=_SSerial->readStringUntil('\n');
      #ifndef disable_debug
          _NSerial->println(str);
      #endif
			if(stringContains(str,"+SAPBR: 1,3",11,str.length()-1))
        return false;
      else if(stringContains(str,"+SAPBR: 1,1",11,str.length()-1))
				return true;
		}
	}
  return false;
  //removed code from here to send speicfic command, and wait for the needed response for specified time..
  // String str="AT+SAPBR=2,1\r\n";
  // String str2="+SAPBR: 1,1";
  // return extendedSendCommand(str,str2,11,5000);
}

bool SIM::prepareForFirmwareUpdate(String &ipaddress)
{
	bool gprs=false;
	if(!(gprs=isGPRSConnected()))
	{	
		if(startGPRS("bsnlnet"))
			gprs=true;
	}

	if(gprs)
	{
		if(connectToFTP(ipaddress))
		{
				if(getProgramSize())
				{
					if(downloadFirmware())
					{
						return true;
					}
				}
		}
	}
	return false;
}


void SIM::watchdogConfig(uint8_t x) {
  WDTCSR = _BV(WDCE) | _BV(WDE);
  WDTCSR = x;
}

void SIM::watchdogReset() {
  __asm__ __volatile__ (
    "wdr\n"
  );
}

// typedef void (*AppPtr_t)(void) __attribute__ ((noreturn));
// 
// AppPtr_t AppStartPtr = (AppPtr_t)0x0000;
// AppStartPtr();


// #asm("jmp 0xE00") // jump to bootloader

void SIM::jumpToBootloader() // Restarts program from beginning but does not reset the peripherals and registers
{
asm volatile ("  jmp 0X7800");  //bootloader vector start address set using BOOTSZ fuse. 
}  
// void SIM::jumpToBootloader()
// {
  // __asm__ __volatile__ (
    // "jmp 0x7800\n"
  // );
// }


void SIM::operateOnMsg(String str, bool admin = false)
{
  if ((stringContains(str, F("CLEARALL"), 8, str.length() - 1)) && admin) //if (str == "CLEARALL" && admin)
  {
    eeprom1->clearNumbers(admin);
  }
  else if(admin && stringContains(str,F("PRGSIZE"),7,str.length()-1))
  {
      eeprom1->saveProgramSize(str.toInt());
      isMsgFromAdmin=admin;
      // sendSMS("PSET",true);
  }
  else if(admin && stringContains(str,F("PRGUPD"),6,str.length()-1))
  {
		  isMsgFromAdmin=admin;
    	// sendSMS("SUP",true);
		  isMsgFromAdmin=admin;
      // bool verify=false;
      // if(stringContains(str,"V",1,str.length()-1))
      //   verify=true;
  		if(prepareForFirmwareUpdate(str))
  		{
          stopGPRS();
          sendBlockingATCommand(F("AT+CFUN=0\r\n"),true);
          // sendSMS("DC",true);
          // noInterrupts();
          eeprom1->updateFirmware(true,true);
          // delay(1000);
          unsigned long int temp= millis();
          while(millis()-temp<4000)
          {}
          
          jumpToBootloader();
          // #asm("jmp 0xE00") // jump to bootloader

          // wdt_enable(WDTO_30MS);
          // while(1) {};
          // watchdogReset();
					// watchdogConfig(WATCHDOG_16MS);  
          // while(1);
  		}
  		else
  		{
          eeprom1->programSizeSet=false;
		    	eeprom1->updateFirmware(false,false);
          #ifndef disable_debug
            _NSerial->println("ED");
          #endif
  				// sendSMS("ED",true);
  		}
  }
  else
  {
    unsigned short int data;
    // if (stringContains(str, "TE", 4, str.length() - 1))
    // {
      // if (isNumeric(str))
      // {
        // data = str.toInt();
        // if (data < 50) data = 50;
        // if (data > 80) data = 80;
        // eeprom1->saveTempSettings(data);  //Store in EEPROM the TEMP value
      // }
    // }
    if (stringContains(str, F("DEFAULT"), 7, str.length() - 1))
    {
      eeprom1->saveAutoStartSettings(true);
      eeprom1->saveDNDSettings(false);
      eeprom1->saveResponseSettings('C');
      eeprom1->saveAutoStartTimeSettings(50);
    }
    else if (stringContains(str, F("AUTOON"), 6, str.length() - 1))
    {
      eeprom1->saveAutoStartSettings(true);  //set AutoStart to True in EEPROM
      motor1->resetAutoStart(true);
    }
    else if (stringContains(str, F("AUTOOFF"), 7, str.length() - 1))
    {
      eeprom1->saveAutoStartSettings(false);  //set AUtoStart to False in EEPROM
      motor1->resetAutoStart(true);
    }
    else if (stringContains(str, F("DNDON"), 5, str.length() - 1))
      eeprom1->saveDNDSettings(true);  //set DND to true in EEPROM
    else if (stringContains(str, F("DNDOFF"), 6, str.length() - 1))
      eeprom1->saveDNDSettings(false);  //set DND to False in EEPROM
    else if (stringContains(str, F("RESPC"), 5, str.length() - 1))
      eeprom1->saveResponseSettings('C');  //set DND to False in EEPROM
    else if (stringContains(str, F("RESPA"), 5, str.length() - 1))
      eeprom1->saveResponseSettings('A');  //set DND to False in EEPROM
    else if (stringContains(str, F("STATUS"), 6, str.length() - 1))
    {
      bool t3 = motor1->ACPowerState();
      bool t5 = motor1->motorState();
      String resp;
      resp = "AC:";
      resp = resp + (t3 ? " ON\n" : " OFF\n");
      resp = resp + "M:";
      resp = resp + (t5 ? " ON\n" : " OFF\n");
      if (eeprom1->AUTOSTART)
        resp = resp + "AUTOON";
      else
        resp = resp + "AUTOOFF";
      isMsgFromAdmin = admin;
      sendSMS(resp, true);
    }
    else if (stringContains(str, F("AUTOTIME"), 8, str.length() - 1))
    {
      if (isNumeric(str))
      {
        data = str.toInt();
        if (data < 50) data = 50;
        if (data > 480) data = 480;
        eeprom1->saveAutoStartTimeSettings(data);  //Store in EEPROM the AUTO START TIME
      }
    }
    else if (stringContains(str, F("NET"), 3, str.length() - 1))
    {
      isMsgFromAdmin = admin;
      sendCommand(F("AT+CSQ"), true);
      sendCSQResponse = true;
    }
    else if (stringContains(str, F("BAL"), 3, str.length() - 1))
    {
      isMsgFromAdmin = admin;
      String s2;
      s2 = "AT+CUSD=1,\"";
      s2.concat(str);
      s2.concat("\"");
      sendCUSDResponse = true;
      sendCommand(s2, true);
    }

    if (stringContains(str, "M-", 2, 12))
    {
      if (str.length() == 10 && isNumeric(str))
      {
        bool t = eeprom1->removeNumber(str);
#ifndef disable_debug
        _NSerial->print("Rem:");
        _NSerial->println((bool)t);
#endif
      }
    }
    else if (stringContains(str, "M+", 2, 12))
    {
      if (str.length() == 10 && isNumeric(str))
      {
        bool t = eeprom1->addNumber(str);
#ifndef disable_debug
        _NSerial->print("Add:");
        _NSerial->println((bool)t);
#endif
      }
    }
    else if (stringContains(str, "AMON", 4, str.length() - 1))
    {
      if (eeprom1->alterNumberPresent)
        eeprom1->saveAlterNumberSetting(true);
    }
    else if (stringContains(str, "AMOFF", 5, str.length() - 1))
    {
      eeprom1->saveAlterNumberSetting(false);
    }
    else if (stringContains(str, "AM+", 3, 13))
    {
      if (str.length() == 10 && isNumeric(str))
      {
        bool t = eeprom1->addAlternateNumber(str);
#ifndef disable_debug
        _NSerial->print("A ");
        _NSerial->print("Add:");
        _NSerial->println((bool)t);
#endif
      }
    }
  }
}

inline bool SIM::isCUSD(String &str)
{
  return stringContains(str, "+CUSD:", 6, str.length() - 1);
}

inline bool SIM::isCSQ(String &str)
{
  return stringContains(str, "+CSQ", 5, str.length() - 3);
}

inline void SIM::sendReadMsg(String str)
{
  String s;
  s = "AT+CMGR=";
  s.concat(str);
  sendCommand(s, true);
}

inline bool SIM::isMsgBody(String &str)
{
  return stringContains(str, "+CMGR:", 24, 34);
}

bool SIM::isAdmin(String str)
{
  if (str == adminNumber)
    return true;
  return false;
}

bool SIM::isPrimaryNumber(String str)
{
  if (eeprom1->numbersCount > 0)
  {
    if (eeprom1->primaryNumber == str)
      return true;
    if (eeprom1->alterNumberSetting && eeprom1->alterNumber == str)
      return true;
  }
  return false;
}

void SIM::gotMsgBody(String &str)
{
  bool admin = isAdmin(str);
  if (admin || isPrimaryNumber(str))
  {
    str = readString(); //_SSerial->readStringUntil('\n');
#ifndef disable_debug
    _NSerial->print("MSG:");
    _NSerial->println(str);
#endif
    operateOnMsg(str, admin);
  }
  delAllMsg();
}

bool SIM::isNewMsg(String &str)
{
  return stringContains(str, "+CMTI:", 12, str.length() - 1);
}

void SIM::setClassReference(S_EEPROM* e1, Motor_MGR* m1)
{
  eeprom1 = e1;
  motor1 = m1;
}

bool SIM::initialize()
{
  byte attempts = 0;
try_again:
  if (sendBlockingATCommand("AT\r\n"))
  {
    if (sendBlockingATCommand(F("AT+CLIP=1\r\n")))
    {
      if (sendBlockingATCommand(F("AT+CLCC=1\r\n")) && sendBlockingATCommand(F("AT+CMGF=1\r\n")) &&  sendBlockingATCommand(F("AT+CNMI=2,1,0,0,0\r\n")))
      {
#ifndef disable_debug
        _NSerial->println("INIT");
#endif
        initialized = true;
        return true;
      }
    }
  }
  if (!initialized && attempts == 0)
  {
    attempts++;
    goto try_again;
  }
#ifndef disable_debug
  _NSerial->println("INIT X");
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
  _NSerial->print("no:");
  _NSerial->println(number);
#endif

  if (number == adminNumber)
    return true;
  else if (eeprom1->checkExists(number) != 0xFF)
    return true;

  return false;
}

void SIM::acceptCommands()
{
  if (!commandsAccepted)
  {
#ifndef disable_debug
    _NSerial->print("com");
    _NSerial->print("Acc:");
    _NSerial->println(millis());
#endif
    digitalWrite(PIN_DTR,HIGH);
    commandsAccepted = true;
    tempAcceptCommandTime = millis();
    while (millis() - tempAcceptCommandTime <= 100)
    {}
  }
  else
  {
    tempAcceptCommandTime = millis();
  }
}

void SIM::rejectCommands()
{
#ifndef disable_debug
  _NSerial->print("com");
  _NSerial->print("Rej");
  _NSerial->println(millis());
#endif
  digitalWrite(PIN_DTR,LOW);
  commandsAccepted = false;
}

void SIM::sendCommand(char cmd, bool newline = false)
{
  acceptCommands();
  if (!newline)
    _SSerial->print(cmd);
  else
    _SSerial->println(cmd);
}

void SIM::sendCommand(String cmd, bool newline = false)
{
  acceptCommands();
  if (cmd == "")
    _SSerial->println();
  else
  {
    if (!newline)
      _SSerial->print(cmd);
    else
      _SSerial->println(cmd);
  }
}

bool SIM::sendBlockingATCommand(String cmd,bool extendTime)
{
  sendCommand(cmd);
#ifndef disable_debug
  _NSerial->print(cmd);
#endif

  unsigned long t = millis();
  String str;
  unsigned long int t2=3000L;
  if(extendTime)
    t2=60000L;
  while (millis() - t < t2)
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

bool SIM::stringContains(String &sstr, String mstr, byte sstart, byte sstop)
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
  return (str == "RING\r");
}

bool SIM::isDTMF(String &str)
{
  return stringContains(str, "+DTMF: ", 7, 8);
}

bool SIM::isCut(String str)
{
  if (currentStatus == 'I' && (currentCallStatus == 'I' || currentCallStatus == 'O'))
  {
#ifndef disable_debug
    _NSerial->print("STR:");
    _NSerial->println(str);
    // _NSerial->println("STR END");
#endif
  }

  if (matchString(str, F("NO CARRIER\r")))
    return true;
  else if (matchString(str, F("BUSY\r")))
    return true;
  else if (matchString(str, F("NO ANSWER\r")))
    return true;
  else if (matchString(str, "ERROR\r"))
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


  if (stringContains(str, F("+CLCC: 1,0,2"), 11, 12))    //dialling
    return 'D'; //call made
  if (stringContains(str, F("+CLCC: 1,0,3"), 11, 12))    //alerting
    return 'R'; //call made
  else if (stringContains(str, F("+CLCC: 1,0,0"), 11, 12)) //active call
    return 'I'; //call accepted
  else if (stringContains(str, F("+CLCC: 1,0,6"), 11, 12)) //call ended
    return 'E'; //call ended
  else
    return 'N';
}

String SIM::getActiveNumber()
{
  if (eeprom1->numbersCount > 0)
  {
    if (!eeprom1->alterNumberSetting)
      return (eeprom1->primaryNumber);
    else
      return (eeprom1->alterNumber);
  }
  else
    return (adminNumber); //="AT+CMGS=\"+917698439201\"";
}

void SIM::makeCall()
{
  acceptCommands();
  _SSerial->flush();

  String command;
  command = "ATD+91";
  command.concat(getActiveNumber());
  command.concat(";");
  sendCommand(command, true);

  unsigned long temp = millis();
  while (millis() - temp < 100)
  {    }
  sendCommand("", true);
#ifndef disable_debug
  _NSerial->print("Call");
  _NSerial->println("Made");
#endif
  // eeprom1->inCall(true);
  callCutWait = millis();
  currentStatus = 'R';
  currentCallStatus = 'O';
  // attemptsToCall++;
}

void SIM::endCall()
{
  nr = 0;
  _SSerial->flush();
  sendCommand("AT+CHUP", true);
  unsigned long temp = millis();
  while (millis() - temp < 100)
  {  }
  sendCommand("", true);
  _SSerial->flush();
  freezeIncomingCalls = false;
  callDialled = false;
  // attemptsToCall=0;

  // eeprom1->inCall(false);
  callAccepted = false;
  responseToAction = false;
  currentStatus = 'N';
  currentCallStatus = 'N';

  obtainEventTimer = millis();
  obtainNewEvent = false;
  //  starPresent=false;
#ifndef disable_debug
  _NSerial->print("Call");
  _NSerial->println("End");
#endif
}

void SIM::setObtainEvent()
{
  if (!obtainNewEvent  && millis() - obtainEventTimer > 1000)
    obtainNewEvent = true;
}

void SIM::acceptCall()
{
  callAccepted = true;
  _SSerial->flush();
  sendCommand("ATA", true);
  _SSerial->flush();
  currentStatus = 'I';
  currentCallStatus = 'I';
  playSound('M');
}

void SIM::sendSMS(String msg = "", bool predefMsg = false)
{
  _SSerial->flush();
  String responseString;

  if (!predefMsg)
  {
    switch (actionType)
    {
      case 'S':
        responseString = STR_MOTOR;
        responseString += STR_ON;
        break;
      case 'O':
      case 'U':
      case 'C':
      case 'F':
        responseString = STR_MOTOR;
        responseString += STR_OFF;
        break;
      default:
        return;
    }
  }
  else
    responseString = msg;

// #ifndef disable_debug
  // _NSerial->println("SMS");
// #endif
  String command;
  command = "AT+CMGS=\"+91";
  	// command=command+adminNumber;
  if (isMsgFromAdmin)
    command.concat(adminNumber);
  else
    command.concat(getActiveNumber());

  command.concat("\"");

  _SSerial->flush();
  sendCommand(command, true);
  _SSerial->flush();
  unsigned long int temp = millis();
  while (millis() - temp < 1000)
  {}
  sendCommand(responseString, true);
  _SSerial->flush();
  temp = millis();
  while (millis() - temp < 1000)
  {}
  _SSerial->flush();
  sendCommand((char)26, true);
  _SSerial->flush();
  temp = millis();
  while (millis() - temp < 1000)
  {}
  isMsgFromAdmin = false;
}

void SIM::operateDTMF(String str)
{
  if (str == "1") //Motor On
  {
    currentOperation = 'S';
    subDTMF();
    motor1->startMotor(true);
  }
  else if (str == "2") //Motor Off
  {
    currentOperation = 'O';
    subDTMF();
    motor1->stopMotor(true);
  }
  else if (str == "3") //Status
  {
    currentOperation = 'T';
    subDTMF();
    motor1->statusOnCall();
  }
  // else if (str == "4") //Set AUTOTIMER ON
  // eeprom1->saveAutoStartSettings(true);  //set AutoStart to True in EEPROM
  // else if (str == "5") //Set AUTOTIMER OFF
  // eeprom1->saveAutoStartSettings(false);  //set AUtoStart to False in EEPROM
}

inline void SIM::subDTMF()
{
  //    // starPresent=true;
  callCutWait = millis();
  stopSound();
}

void SIM::operateRing()
{
  nr++;
  if (nr <= 2)
  {

    if (nr == 1)
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

    callCutWait = millis();

    if (str.length() >= 10 && isNumeric(str))
    {
      if (nr > 1 && !checkNumber(str))
        endCall();
    }
  }
  else if (nr == 3)
  {
    callCutWait = millis();
    acceptCall();
  }
}

bool SIM::playSoundElligible()
{
  return (bplaySound && ((millis() - soundWait) > (soundWaitTime * 100)));
}

void SIM::triggerPlaySound()
{
  _SSerial->flush();
  sendCommand("AT+CREC=4,\"");
  sendCommand("C:\\User\\FTP\\");
  sendCommand(playFile);
  sendCommand(".amr\",0,100,1\r", true);
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

void SIM::playSound(char actionType, bool newAction)
{
  _SSerial->flush();
  stopSound();

  soundWait = millis();
  bplaySound = true;
  if (newAction)
    this->actionType = actionType
;  playFile = actionType;
}

void SIM::stopSound()
{
  _SSerial->flush();
  sendCommand(F("AT+CREC=5\r"), true);
  _SSerial->flush();
}

bool SIM::callTimerExpire()
{
  return ((millis() - callCutWait) >= (callCutWaitTime * 100));
}

void SIM::makeResponseAction()
{
  makeResponse = false;
  if (eeprom1->RESPONSE == 'A' || eeprom1->RESPONSE == 'C')
    makeCall();
}

bool SIM::registerEvent(char eventType)
{
  if (!initialized)
  {
#ifndef disable_debug
    _NSerial->println("NO SIM");
#endif
    return true;
  }

  if (currentStatus == 'N' && currentCallStatus == 'N' && obtainNewEvent)
  {
    freezeIncomingCalls = true;
    acceptCommands();

#ifndef disable_debug
    _NSerial->print("E:");
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
  return (commandsAccepted && millis() - tempAcceptCommandTime >= (acceptCommandsTime * 100));
}

void SIM::checkNetwork(String str)
{
  if (str == F("+CPIN: NOT READY\r"))
  {
    endCall();
    networkCounterMeasures();
  }
}

void SIM::networkCounterMeasures()
{
  _SSerial->flush();
  sendBlockingATCommand("AT+CFUN=4\r\n");
  _SSerial->flush();
  unsigned long t = millis();
  while (millis() - t < 100)
  {}
  sendBlockingATCommand("AT+CFUN=1\r\n");
  _SSerial->flush();
  t=millis();
  while (millis() - t < 100)
  {}
}

void SIM::setMotorMGRResponse(char response)
{
  if (currentOperation == 'S') //start Motor
  {
    responseToAction = true;
    if (response == 'L')
      playSound('N');  //cannot start motor
    else if (response == 'O')
      playSound('1');  //motor is already on
    else if (response == 'D')
      playSound('S');  //motor has started
    // endCall();
  }
  else if (currentOperation == 'O') //switch off motor
  {
    responseToAction = true;
    if (response == 'L')
      playSound('P');    //cannot stop motor
    else if (response == 'O')
      playSound('2');  //motor is already off
    else if (response == 'D')
      playSound('O');  //motor has stopped
    // endCall();
  }
  else if (currentOperation = 'T')
  {
    responseToAction = true;
    if (response == 'L') //motor off, no light
      playSound('L');
    else if (response == 'A') //motor off, light on
      playSound('A');
    else if (response == 'B') //motor off, light on
      playSound('B');
    else if (response == 'O') //motor off, light on
      playSound('3');
    else if (response == 'D')
      playSound('1');  //motor is on
  }
}

inline bool SIM::isCallReady(String str)
{
  return matchString(str, "Call Ready\r");
}

bool SIM::checkSleepElligible()
{
  return( !commandsAccepted   &&  !sendCSQResponse        &&  !sendCUSDResponse
      &&  currentStatus=='N'  &&  currentCallStatus=='N'  &&  obtainNewEvent
      &&  !makeResponse       &&  !freezeIncomingCalls);
}

// void SIM::setNetLight(byte light)
// {
//   bool isSleep=commandsAccepted;
//     if(light==L_REGULAR)
//       sendBlockingATCommand(F("AT+SLEDS=2,64,3000\r\n"));
//     else
//       sendBlockingATCommand(F("AT+SLEDS=2,64,10000\r\n"));
//     if(!isSleep)
//       rejectCommands();
// }

void SIM::update()
{
  if (rejectCommandsElligible() && !motor1->ACPowerState())
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
      char t1 = actionType;
      if (!callDialled)
      {
        endCall();
#ifndef disable_debug
        _NSerial->print("DIAL");
        _NSerial->println("OFF");
#endif
        actionType = t1;
        if (eeprom1->RESPONSE == 'A')
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
    str = readString();

    if (isCUSD(str) && sendCUSDResponse)
    {
      sendCUSDResponse = false;
      sendSMS(str, true);
      sendCommand("AT+CUSD=0", true);
    }
    else if (isCSQ(str) && sendCSQResponse)
    {
      sendCSQResponse = false;
      unsigned short int t2 = str.toInt();
      t2 = t2 * 3;
      str = " ";
      str = str + t2;
      sendSMS(str, true);
    }
    else if (isNewMsg(str))
    {
      sendReadMsg(str);
    }
    else if (isMsgBody(str))
    {
      gotMsgBody(str);
    }
    else if (isCallReady(str))
    {
#ifndef disable_debug
      _NSerial->println("INIT");
#endif
      initialized = true;
    }
    else
      checkNetwork(str);

    if (!freezeIncomingCalls &&  (currentStatus == 'N' || currentStatus == 'R') && (currentCallStatus == 'N' || currentCallStatus == 'I')) //Ringing Incoming Call
    {
      if (isRinging(str)) //  chk_ringing(str) == true)
      {
        // eeprom1->inCall(true);
        currentStatus = 'R';
        currentCallStatus = 'I';
        operateRing();
      }
      else if (isCut(str))
      {
        endCall();
      }
    }
    else if (!freezeIncomingCalls && currentStatus == 'I' && currentCallStatus == 'I') //IN CALL INCOMING CALL
    {
      if (isCut(str)) //chk_cut(str) == true)
      {
        endCall();
      }
      else if (isDTMF(str)) //chk_DTMF(str) == true)
      {
        operateDTMF(str);
      }
    }
    else if ((currentStatus == 'N' || currentStatus == 'R') && currentCallStatus == 'O') // OUTGOING CALL
    {
      if (callState(str) == 'D')
      {
				#ifndef disable_debug
        _NSerial->print("DIAL");
        _NSerial->println("ON");
				#endif
        callDialled = true;
      }
      else if (callState(str) == 'R')
      {
        callCutWait = millis();
        currentStatus = 'R';
        currentCallStatus = 'O';
      }
      else if (isCut(str) || callState(str) == 'E') //
      {
        endCall();
      }
      else if (callState(str) == 'I') //else if (stringContains(str, "+CLCC: 1,0,0", 11, 12) == true)
      {
				#ifndef disable_debug
        _NSerial->println("Accept");
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
