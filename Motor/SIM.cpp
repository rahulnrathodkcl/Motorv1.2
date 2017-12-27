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
 // #ifndef ENABLE_M2M
 //  lastSetTime=0;
 //  #endif
  // adminNumber = "7041196959";
  initialized = false;
  inCall=false;
  // balStr.reserve(12);

  acceptCommandsTime = 150;
  commandsAccepted = false;

  // pinMode(PIN_RING,INPUT);
  pinMode(PIN_DTR, OUTPUT);
  rejectCommands();

  soundWaitTime = 5;
  bplaySound = false;

  // callDialled = false;
  // callAlerted = false;
  // attemptsToCall=0;

  actionType = 'N';
  // responseToAction = false;

  callCutWaitTime = 580;
  nr = 0;
  currentStatus = 'N';
  currentCallStatus = 'N';
  callAccepted = false;
  freezeIncomingCalls = false;
  obtainNewEvent = true;
  isMsgFromAdmin = false;
  eventStaged=false;
  stagedEventType = 'N'; 
  isRegisteredNumber=false;
  retries=0;
  // retryOn=false;

  #ifdef ENABLE_CURRENT
  zeroPressed=false;
  #endif

  #ifdef ENABLE_M2M
    m2mAck=false;
    m2mEventCalls=m2mEventNo=0;
    // m2mEventCalls=stagedM2MEventNo=m2mEventNo=0;
    m2mEventStaged=false;
    m2mEvent=false;

    keyPressed=false;
  #endif
  

  // acceptCommands();
}

void SIM::startSIMAfterUpdate()
{
  sendBlockingATCommand_P(PSTR("AT+CFUN=1,1\r\n"));
  // sendBlockingATCommand(F("AT+CFUN=1,1\r\n"));
}

void SIM::sendUpdateStatus(byte updateStatus)
{
  if(!initialized)
    return;

    if(updateStatus>0 && updateStatus!=0xFF)
    {
      // String promptStr=F("UPDSUCC");;
      char promptStr[9];
      strcpy_P(promptStr,PSTR("UPDSUCC"));
      
      if(updateStatus==0x02)
        strcpy_P(promptStr,PSTR("CERR"));
        // promptStr=F("CERR");
      else if(updateStatus==0x03)
        strcpy_P(promptStr,PSTR("MAXCEDD"));
        // promptStr=F("MAXCEDD");
    
      #ifndef disable_debug
        _NSerial->println(promptStr);
      #endif
      isMsgFromAdmin=true;
      sendSMS(promptStr,true);
    }
}

inline void SIM::delAllMsg()
{
  sendBlockingATCommand_P(PSTR("AT+CMGDA=\"DEL ALL\"\r\n"));
  // sendBlockingATCommand(F("AT+CMGDA=\"DEL ALL\"\r\n"));
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
bool SIM::extendedSendCommand(char *cmd,byte timeout)
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


// bool SIM::extendedSendCommand(String cmd,byte timeout)
// {
//   sendCommand(cmd,false);
//   unsigned long temp =millis();
//   while(millis()-temp< timeout*100)
//   {
//     if(_SSerial->available())
//     {
//       String str = _SSerial->readStringUntil('\n');
//       if(str=="OK\r")
//         return true;
//       else if(str=="ERROR\r")
//         return false;
//     }
//   }
//   return false;
// }

bool SIM::startGPRS(const char *apn)
{
    //AT+SAPBR=3,1,"ConType","GPRS"
    // String m1="AT+SAPBR=3,1,\"";
    // String cmd;
    // cmd=m1;
    // cmd=cmd+"ConType\",\"GPRS\"\r\n";
    // cmd = cmd + F(STR_SAPBR_GPRS);
    
    // String cmd=F(STR_SAPBR_PARAM);
    // cmd.concat(F(STR_SAPBR_GPRS));
    char buf[40];
    strcpy_P(buf,PSTR(STR_SAPBR_PARAM));
    strcat_P(buf,PSTR(STR_SAPBR_GPRS));

    // String cmd=F(STR_SAPBR_PARAM);
    // cmd.concat(F(STR_SAPBR_GPRS));
        

    // if(extendedSendCommand(cmd,50))
    if(extendedSendCommand(buf,50))
    {
        strcpy_P(buf,PSTR(STR_SAPBR_PARAM));
        strcat_P(buf,PSTR(STR_SAPBR_APN));
        strcat_P(buf,apn);
        strcat_P(buf,PSTR("\"\r\n"));

        // cmd = F(STR_SAPBR_PARAM);
        // cmd.concat(F(STR_SAPBR_APN));
        // cmd.concat(apn);
        // cmd.concat("\"\r\n");
        
        // cmd = cmd + F(STR_SAPBR_APN);
        // cmd = cmd + apn;
        // cmd = cmd + "\"\r\n";
        //AT+SAPBR=3,1,"APN","bsnlnet"
        // if(extendedSendCommand(cmd,50))
        if(extendedSendCommand(buf,50))
        {
            // if(sendBlockingATCommand(F(STR_SAPBR_START),true))
            if(sendBlockingATCommand_P(PSTR(STR_SAPBR_START),true))
            {
                // unsigned long temp = millis();
                // while(millis() - temp < 2000)
                // {}
              delay(20);
                return true;
            }
        }
    }
  return false;
}

// bool SIM::startGPRS(String apn)
// {
//     //AT+SAPBR=3,1,"ConType","GPRS"
//   	// String m1="AT+SAPBR=3,1,\"";
//   	// String cmd;
//   	// cmd=m1;
//   	// cmd=cmd+"ConType\",\"GPRS\"\r\n";
//     // cmd = cmd + F(STR_SAPBR_GPRS);
    
//     // String cmd=F(STR_SAPBR_PARAM);
//     // cmd.concat(F(STR_SAPBR_GPRS));
//     String cmd=F(STR_SAPBR_PARAM);
//     cmd.concat(F(STR_SAPBR_GPRS));
        

//     if(extendedSendCommand(cmd,50))
//     {
//         cmd = F(STR_SAPBR_PARAM);
//         cmd.concat(F(STR_SAPBR_APN));
//         cmd.concat(apn);
//         cmd.concat("\"\r\n");
        
//         // cmd = cmd + F(STR_SAPBR_APN);
//         // cmd = cmd + apn;
//         // cmd = cmd + "\"\r\n";
//         //AT+SAPBR=3,1,"APN","bsnlnet"
//         if(extendedSendCommand(cmd,50))
//         {
//             // if(sendBlockingATCommand(F(STR_SAPBR_START),true))
//             if(sendBlockingATCommand_P(PSTR(STR_SAPBR_START),true))
//             {
//                 // unsigned long temp = millis();
//                 // while(millis() - temp < 2000)
//                 // {}
//               delay(20);
//                 return true;
//             }
//         }
//     }
//   return false;
// }

inline bool SIM::stopGPRS()
{
    return(sendBlockingATCommand_P(PSTR(STR_SAPBR_STOP),true));
		// return(sendBlockingATCommand(F(STR_SAPBR_STOP),true));
}

bool SIM::connectToFTP(String ipaddress)
{
  //AT+FTPCID=1
  // if(sendBlockingATCommand(F("AT+FTPCID=1\r\n"),true))
  if(sendBlockingATCommand_P(PSTR("AT+FTPCID=1\r\n"),true))
  {
    //AT+FTPSERV="43.252.117.34"
    
    String cmd=F("AT+FTPSERV=\"");
    cmd.concat(ipaddress);
    cmd.concat("\"\r\n");

    // cmd=cmd+ipaddress;
    // cmd=cmd+"\"\r\n";
    if(sendBlockingATCommand(cmd,true))
    {
      // AT+FTPUN="FTP-User"
      // AT+FTPPW="123456789"
      // if(sendBlockingATCommand(F("AT+FTPUN=\"rahulnrathodkcl\"\r\n"),true))
      if(sendBlockingATCommand_P(PSTR("AT+FTPUN=\"rahulnrathodkcl\"\r\n"),true))
      {
        // if(sendBlockingATCommand(F("AT+FTPPW=\"123456789\"\r\n"),true))
        if(sendBlockingATCommand_P(PSTR("AT+FTPPW=\"123456789\"\r\n"),true))
        {
            return true;
        }
      }
    }
  }
  return false;
}

// bool SIM::setFile(String filename)
// {
// 		String m1=F("AT+FTPGET");
//     String cmd=m1;
//     cmd.concat("NAME=\"");
//     cmd.concat(filename);
//     cmd.concat("\"\r\n");

//     // cmd=cmd + "NAME=\"";   //m.bin\"\r\n;
//     // cmd=cmd + filename;
//     // cmd= cmd + "\"\r\n";

//     if(sendBlockingATCommand(cmd,true))
//     {
//     		cmd=m1;
//         cmd.concat("PATH=\"/\"\r\n");
//     		// cmd=m1+"PATH=\"/\"\r\n";
//         if(sendBlockingATCommand(cmd),true)
//             return true;  
//     }
//     return false;
// }

bool SIM::setFile(const char *filename)
{
    char buf[40];
    strcpy_P(buf,PSTR("AT+FTPGET"));
    strcat_P(buf,PSTR("NAME=\""));
    strcat_P(buf,filename);
    strcat_P(buf,PSTR("\"\r\n"));

    // String m1=F("AT+FTPGET");
    // String cmd=m1;
    // cmd.concat("NAME=\"");
    // cmd.concat(filename);
    // cmd.concat("\"\r\n");


    // cmd=cmd + "NAME=\"";   //m.bin\"\r\n;
    // cmd=cmd + filename;
    // cmd= cmd + "\"\r\n";

    // if(sendBlockingATCommand(cmd,true))
    if(sendBlockingATCommand(buf,true))
    {
        strcpy_P(buf,PSTR("AT+FTPGET"));
        strcat_P(buf,PSTR("PATH=\"/\"\r\n"));        
        // cmd=m1;
        // cmd.concat("PATH=\"/\"\r\n");
        
        // // cmd=m1+"PATH=\"/\"\r\n";
        if(sendBlockingATCommand_P(buf),true)
            return true;  
    }
    return false;
}

bool SIM::getProgramSize()
{

  // if(setFile("m.hex"))
  if(setFile(PSTR("m.hex")))
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
      sendCommand_P(PSTR("AT+FTPSIZE\r\n"),false);
      // sendCommand(F("AT+FTPSIZE\r\n"),false);
      while(millis()-temp<120000L)
      {
          if(_SSerial->available())
          {
              String str=_SSerial->readStringUntil('\n');
              #ifndef disable_debug
                _NSerial->println(str);
              #endif
              if(stringContains(str,F("+FTPSIZE: 1,0,"),14,str.length()-1))
              {
                // eeprom1->saveProgramSize(atoi(buf));
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

// bool SIM::downloadFirmware()
// {
//     unsigned long int size=eeprom1->getProgramSize();
    
//     //  +FTPGETTOFS: 0,2396
//     String m1="+FTPGETTOFS";
//     String v1=m1;
//     v1.concat(": 0,");
//     v1.concat(size);
//     v1.concat("\r");
//     // v1 = v1 + size;
//     // v1 = v1 + "\r";

//     // AT+FTPGETTOFS=0,"m.hex"\r\n
//     String cmd="AT"; 
//     cmd.concat(m1);
//     cmd.concat("=0,\"m.hex\"\r\n");

//     // cmd=cmd+m1;
//     // cmd=cmd+ "=0,\"m.hex\"\r\n";
//     unsigned long int temp = millis();
//     sendCommand(cmd,false);
//     while(millis()-temp<120000L)
//     {
//         if(_SSerial->available())
//         {
//             cmd=_SSerial->readStringUntil('\n');
//             #ifndef disable_debug
//                 _NSerial->println(cmd);
//             #endif
//             if(cmd==v1)
//             {
//               #ifndef disable_debug
//                 _NSerial->println("DC");
//               #endif   
//                 return true;
//             }
//         }          
//     }
//     return false;
//     // return extendedSendCommand(cmd,v1,25,60000);           
// }

bool SIM::downloadFirmware()
{
    unsigned long int size=eeprom1->getProgramSize();
    //  +FTPGETTOFS: 0,2396
    // String m1="+FTPGETTOFS";
    // String v1=m1;
    // v1.concat(": 0,");
    // v1.concat(size);
    // v1.concat("\r");

    char buf2[30];
    strcpy_P(buf2,"+FTPGETTOFS")
    strcat_P(buf2,PSTR(": 0,"));
    strcat(buf2,itoa(size));
    strcat_P(buf2,PSTR("\r"));
    // v1 = v1 + size;
    // v1 = v1 + "\r";

    // AT+FTPGETTOFS=0,"m.hex"\r\n
    
    // String cmd="AT"; 
    // cmd.concat(m1);
    // cmd.concat("=0,\"m.hex\"\r\n");

    // cmd=cmd+m1;
    // cmd=cmd+ "=0,\"m.hex\"\r\n";
    unsigned long int temp = millis();
    char buf[30];
    strcpy_P(buf,PSTR("AT+FTPGETTOFS=0,\"m.hex\"\r\n"));
    sendCommand_P(buf,false);
    // sendCommand(cmd,false);
    while(millis()-temp<120000L)
    {
        if(_SSerial->available())
        {
          String cmd = _SSerial->readStringUntil('\n');
            #ifndef disable_debug
                _NSerial->println(cmd);
            #endif
          cmd.toCharArray();
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
      if(stringContains(str,F("+SAPBR: 1,"),10,11))
      {
          if(str.charAt(0)=='3')
            return false;
          else if(str.charAt(0)=='1')
            return true;
      }
			// if(stringContains(str,"+SAPBR: 1,3",11,str.length()-1))
   //      return false;
   //    else if(stringContains(str,"+SAPBR: 1,1",11,str.length()-1))
			// 	return true;
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
    if(startGPRS(PSTR("bsnlnet")))
    {
      byte cnt = 12;
      while(--cnt)
      {
        // unsigned long temp = millis();
        // while(millis()-temp<5000)
        // {}
        delay(50);
        if((gprs=isGPRSConnected()))
          break;
      }
    }
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


// void SIM::watchdogConfig(uint8_t x) {
//   WDTCSR = _BV(WDCE) | _BV(WDE);
//   WDTCSR = x;
// }

// void SIM::watchdogReset() {
//   __asm__ __volatile__ (
//     "wdr\n"
//   );
// }

// typedef void (*AppPtr_t)(void) __attribute__ ((noreturn));
// 
// AppPtr_t AppStartPtr = (AppPtr_t)0x0000;
// AppStartPtr();


// #asm("jmp 0xE00") // jump to bootloader

inline void SIM::jumpToBootloader() // Restarts program from beginning but does not reset the peripherals and registers
{
asm volatile ("  jmp 0X7800");  //bootloader vector start address set using BOOTSZ fuse. 
}  
// void SIM::jumpToBootloader()
// {
  // __asm__ __volatile__ (
    // "jmp 0x7800\n"
  // );
// }

inline void SIM::initRestartSeq()
{
    sendBlockingATCommand_P(PSTR("AT+CSCLK=0\r\n"));
    sendBlockingATCommand_P(PSTR("AT&W\r\n"));
    sendBlockingATCommand_P(PSTR("AT+COPS=2\r\n"));

    // sendBlockingATCommand(F("AT+CSCLK=0\r\n"));
    // sendBlockingATCommand(F("AT&W\r\n"));
    // sendBlockingATCommand(F("AT+COPS=2\r\n"));

    // isMsgFromAdmin=true;
    // sendSMS("DC",true);
    eeprom1->updateFirmware(true,true);
    // unsigned long int temp= millis();
    delay(100);
    // while(millis()-temp<10000)
    // {}
    jumpToBootloader();
}

bool SIM::checkPrgReq(String str,bool noMsg)
{
  byte p=0;
  isMsgFromAdmin=true;

  if(stringContains(str,F("PRGUPD"),6,str.length()-1))
  {
      if(currentStatus=='N')
        endCall();
      if(!noMsg) sendSMS(F("SUP"),true);
      // bool verify=false;
      // if(stringContains(str,"V",1,str.length()-1))
      //   verify=true;
      // str.toLowerCase();
      if(prepareForFirmwareUpdate(str))
      {
          isMsgFromAdmin=true;
          if(!noMsg) sendSMS(F("DC"),true);
          stopGPRS();
          initRestartSeq();
      }
      else
      {
          stopGPRS();
          eeprom1->programSizeSet=false;
          eeprom1->updateFirmware(false,false);
          #ifndef disable_debug
            _NSerial->println("ED");
          #endif
          isMsgFromAdmin=true;
          if(!noMsg) sendSMS("ED",true);
      }
          return true;
  }
  // else if(stringContains(str,F("PRGSIZE"),7,str.length()-1))
  // {
  //     p=0x01;
  // }
  // else if(stringContains(str,F("REUPD"),5,str.length()-1))
  // {
  //   p=0x02;
  // }

  // if(p>0)
  // {
  //     eeprom1->saveProgramSize(str.toInt());
  //     String tempStr = F("OK : ");
  //     tempStr.concat(str);
  //     // tempStr = tempStr + str;
  //     if(!noMsg) sendSMS(tempStr,true);
  //     if(p==0x02) initRestartSeq();

  //     return true;
  // }
  isMsgFromAdmin=false;
  return false;
}

void SIM::stopCallWaiting()
{
    sendBlockingATCommand_P(PSTR("AT+CCWA=0,0\r\n"));
    // sendBlockingATCommand(F("AT+CCWA=0,0\r\n"));
}

#ifndef ENABLE_M2M
#ifndef ENABLE_CURRENT
bool SIM::checkNoCallTime()
{
  byte globalHours, globalMinutes;
  getSystemTime(globalHours,globalMinutes);
  if(globalHours >= (eeprom1->NCSTARTHOUR) && globalHours <=(eeprom1->NCSTOPHOUR))
  {
    if(eeprom1->NCSTARTHOUR==eeprom1->NCSTOPHOUR)
    {
      if(globalMinutes>=(eeprom1->NCSTARTMINUTE) && globalMinutes<=(eeprom1->NCSTOPMINUTE))
        return true;      
      else
        return false;
    }
    if(globalHours==eeprom1->NCSTARTHOUR)
    {
      if(globalMinutes>=eeprom1->NCSTARTMINUTE)
        return true;     
      else
        return false;
    }
    if(globalHours==eeprom1->NCSTOPHOUR)
    {
      if(globalMinutes<=(eeprom1->NCSTOPMINUTE))
        return true;
      else
        return false;
    }
    if(globalHours > (eeprom1->NCSTARTHOUR) && globalHours < (eeprom1->NCSTOPHOUR))
      return true;
  }
  return false;
}
#endif
#endif

bool SIM::getBlockingResponse(String &cmd,bool (SIM::*func)(String &))
{
  unsigned long temp = millis();
  sendCommand(cmd,true);
  while(millis() - temp <4000)
  {
    if(_SSerial->available())
    {
      String str = readString();
      if((this->*func)(str))
      {
          cmd=str;
          return true;
      }
    }
  }
  return false;
}

#ifdef ENABLE_M2M

inline void SIM::verifyRemoteNumber()
{
    sendSMS(F("VMM01"),true,SEND_TO_M2M_REMOTE);
}
     
// void SIM::operateOnStagedM2MEvent()
// {
//   if(obtainNewEvent && millis()-tempEventStageTime>(30000L))
//   {
//       freezeIncomingCalls = true;
//       acceptCommands();
//       m2mEventNo=stagedM2MEventNo;
//       m2mEvent=true;
//       m2mEventStaged=false;
//       makeResponseAction();
//   }
// }

void SIM::registerM2MEvent(byte eventNo)
{
  if (!initialized)
  {
    motor1->setM2MEventState(eventNo,ME_CLEARED);
    return;
  }

  if (currentStatus == 'N' && currentCallStatus == 'N' && obtainNewEvent && !eventStaged && !m2mEventStaged)
  {    
      // freezeIncomingCalls = true;
      // acceptCommands();
      motor1->setM2MEventState(eventNo,ME_SERVICING);
      m2mEvent=true;
      m2mEventNo = eventNo;
      m2mEventCalls=0;
      makeResponseAction();
      return;
  }
}
#endif

// bool SIM::checkValidTime(byte &Hours,byte &Minutes)
// {
//   if(Hours>=0 && Hours<24 && Minutes>=0 && Minutes<60)
//     return true;

//   return false;
// }


void SIM::operateOnMsg(String str, bool admin = false,bool noMsg=false,bool alterNumber=false)
{
  isMsgFromAdmin=admin;
  String tempStr=str;
  bool done=false;
  bool processed=false;
  unsigned short int data;
 
    // if (str.startsWith(F("SCWAIT")))// stringContains(str, F("AUTOON"), 6, str.length() - 1))
    // {
      // stopCallWaiting();
      // done=true;
    // }
    // else if(str.startsWith("@"))
    // {
    //     processed=true;
    //     sendCommand(str.substring(1,str.length()),true);
    //     unsigned long temp = millis();
    //     String t2="";
    //     while(millis()-temp<4000)
    //     {
    //       String temp2=readString();
    //         t2=t2+temp2;
    //         if(temp2=="OK\r" || temp2=="ERROR\r")
    //           break;
    //     }
    //     sendSMS(t2,true);
    // }
    if(str.startsWith(F("WAIT")))
    {
      stopCallWaiting();
      done=true;
    }
    else if (str.startsWith(F("CLEARALL")))// (stringContains(str, F("CLEARALL"), 8, str.length() - 1))) //if (str == "CLEARALL" && admin)
    {
      eeprom1->clearNumbers(admin);
      done=true;
    }
    else if (str.startsWith(F("DEFAULT"))) //stringContains(str, F("DEFAULT"), 7, str.length() - 1))
    {
      eeprom1->saveAutoStartSettings(false);
      #ifdef ENABLE_WATER
        #ifdef ENABLE_M2M
          eeprom1->saveM2MSettings(false);
        #else
          eeprom1->savePreventOverFlowSettings(false);
        #endif
      #endif
      #ifdef ENABLE_CURRENT
          eeprom1->setOverloadPer(125);
          eeprom1->setUnderloadPer(75);
          eeprom1->setCurrentDetection(false);
      #endif
      eeprom1->saveEventStageSettings(0);
      eeprom1->saveBypassSettings(false);
      eeprom1->saveDNDSettings(false);
      eeprom1->saveResponseSettings('C');
      eeprom1->saveAutoStartTimeSettings(50);
      eeprom1->saveStarDeltaTimer(2);
      done=true;
    }
    else if (str.startsWith(F("NUM")))
    {
      processed=true; 
      String resp = eeprom1->getNumbers();
      if(resp=="")
      {
        resp=F("No Number");
      }
      sendSMS(resp,true);
    }
    else if (str.startsWith(F("RESET")))// stringContains(str, F("RESET"), 5, str.length() - 1))
    {
       jumpToBootloader();
    } 
    else if (str.startsWith(F("DID")))
    {
      processed=true;
      registerWithAdmin();
    }
    else if (str.startsWith(F("AUTOON")))// stringContains(str, F("AUTOON"), 6, str.length() - 1))
    {
      eeprom1->saveAutoStartSettings(true);  //set AutoStart to True in EEPROM
      motor1->resetAutoStart(true);
      done=true;
    }
    else if (str.startsWith(F("AUTOOFF"))) //stringContains(str, F("AUTOOFF"), 7, str.length() - 1))
    {
      eeprom1->saveAutoStartSettings(false);  //set AUtoStart to False in EEPROM
      motor1->resetAutoStart(true);
      done=true;
    }
    else if (str.startsWith(F("BYPON")))
    {
      eeprom1->saveBypassSettings(true);  //set BYPASS to true in EEPROM
      done=true;
    }
    else if (str.startsWith(F("BYPOFF")))
    {
      eeprom1->saveBypassSettings(false);  //set BYPASS to false in EEPROM
      done=true;
    }
    else if (str.startsWith(F("DNDON")))//stringContains(str, F("DNDON"), 5, str.length() - 1))
    {
      eeprom1->saveDNDSettings(true);  //set DND to true in EEPROM
      done=true;
    }
    else if (str.startsWith(F("DNDOFF")))//stringContains(str, F("DNDOFF"), 6, str.length() - 1))
    {
      eeprom1->saveDNDSettings(false);  //set DND to False in EEPROM
      done=true;
    }
    else if (str.startsWith(F("RESP")))  //stringContains(str, F("RESPC"), 5, str.length() - 1))
    {
      if(str.length()>4)
      {
          char c = str.charAt(4);
          if(c=='C' || c=='A' || c=='T' || c=='N')
          {
            eeprom1->saveResponseSettings(c);  //save specific RESPONSE settings
            done=true;
          }
      }
    }
    #ifdef ENABLE_CURRENT
    // else if(str.startsWith("OVR"))
    else if(stringContains(str, F("OVR"), 3, str.length()-1))
    {
      if (isNumeric(str))
      {
        data = str.toInt();
        if(data>110)
        {
          eeprom1->setOverloadPer(data);
          done=true;
        }
      }
    }
    else if(stringContains(str, F("UNDR"), 4, str.length()-1))
    {
      if (isNumeric(str))
      {
        data = str.toInt();
        if(data<95 && data>0)
        {
          eeprom1->setUnderloadPer(data);
          done=true;
        }
      }
    }
    else if(str.startsWith("ASTAT"))
    {
        String smsg="C:";
        smsg.concat(motor1->getCurrentConsumed());
        smsg.concat("\nN:");
        smsg.concat(eeprom1->NORMALVALUE);
        smsg.concat("\nO:");
        smsg.concat(eeprom1->OVERLOADVALUE);
        smsg.concat("\nU:");
        smsg.concat(eeprom1->UNDERLOADVALUE);
        smsg.concat("\nOP:");
        smsg.concat(eeprom1->OVERLOADPER);
        smsg.concat("\nUP:");
        smsg.concat(eeprom1->UNDERLOADPER);
        sendSMS(smsg,true);
        done=true;
    }
    #endif
    #ifdef ENABLE_WATER
      #ifndef ENABLE_M2M
      else if (str.startsWith(F("OVFON"))) //stringContains(str, F("RESPA"), 5, str.length() - 1))
      {
         eeprom1->savePreventOverFlowSettings(true);  //set DND to False in EEPROM
         done=true;
      } 
      else if (str.startsWith(F("OVFOFF"))) //stringContains(str, F("RESPA"), 5, str.length() - 1))
      {
         eeprom1->savePreventOverFlowSettings(false);  //set DND to False in EEPROM
         done=true;
      } 
      #else    
      else if (str.startsWith(F("M2MON")))
      {
        if(eeprom1->m2mRemotePresent && !eeprom1->m2mRemoteVerified)
        {
          noMsg=true;
          verifyRemoteNumber(); 
        }
      }
      else if (str.startsWith(F("M2MOFF")))
      {
        eeprom1->saveM2MSettings(false);  //set DND to False in EEPROM
        done=true;
      }
      #endif
    #endif
    else if (str.startsWith(F("STATUS")))//stringContains(str, F("STATUS"), 6, str.length() - 1))
    {
      processed=true;
      
      byte batPer = 0xFF;
      byte net = 0xFF;
      // batPer = net = 0xFF;
      
      str = F("AT+CSQ");
      if(getBlockingResponse(str,&SIM::isCSQ))
      {
        net = str.toInt()*3;
      }
      delay(5);
      str = F("AT+CBC");
      if(getBlockingResponse(str,&SIM::isCBC))
      {
        batPer = (str.substring(0,str.lastIndexOf(","))).toInt();
      }
      
      sendSMS(makeStatusMsg(batPer,net),true);
    }
    else if (str.startsWith(F("AMON")) && (admin || alterNumber))//stringContains(str, "AMON", 4, str.length() - 1))
    {
      if (eeprom1->alterNumberPresent)
      {
        eeprom1->saveAlterNumberSetting(true);
        done=true;
      }
    }
    else if (str.startsWith(F("AMOFF")))//stringContains(str, "AMOFF", 5, str.length() - 1))
    {
      eeprom1->saveAlterNumberSetting(false);
      done=true;
    }
    #ifndef ENABLE_M2M
    #ifndef ENABLE_CURRENT
    else if (str.startsWith(F("NCOFF")))    // stop No Call according to Time
    {
      eeprom1->saveNoCallSettings(false);
      done=true;
    }
    else if(str.startsWith(F("GETTIME")))
    {
      byte globalHours,globalMinutes;
      getSystemTime(globalHours,globalMinutes);
      String strTime = F("TIME:");
      strTime.concat(globalHours);
      strTime.concat(globalMinutes);
      sendSMS(strTime, true);
      processed=true;
    }
    else if(stringContains(str,F("SETTIME"),7,11))
    {
      // "yy/MM/dd,hh:mm:ss±zz"
      String strTime = F("AT+CCLK=\"17/10/30,");
      strTime.concat(str.substring(0,2));
      strTime.concat(":");
      strTime.concat(str.substring(2,4));
      strTime.concat(":00+22");
      done=true;
      
    }
    else if(stringContains(str,F("NCTIME"),6,15))
    {
      if(str.charAt(4) == '-')
      {
        String endStr = str.substring(0,2);
        byte startHour = endStr.toInt();
        endStr = str.substring(2,4);
        byte startMinute = endStr.toInt();
        endStr = str.substring(5,7);
        byte stopHour = endStr.toInt();
        endStr = str.substring(7,9);
        byte stopMinute = endStr.toInt();

        if(startHour>=0 && startHour<24 && startMinute>=0 && startMinute<60)
        {
          if(stopHour>=0 && stopHour<24 && stopMinute>=0 && stopMinute<60)  
          {
            if(startHour==stopHour && startMinute==stopMinute)
              eeprom1->saveNoCallSettings(false);
            else
            {
              done=true;
              eeprom1->saveNoCallSettings(true,startHour,startMinute,stopHour,stopMinute);
            }
          }
        }
      }
    }
    #endif
    #endif
    else if (stringContains(str, F("STAGE"), 5, str.length() - 1))
    {
      if (isNumeric(str))
      {
        data = str.toInt();
        if (data < 0) data = 0;
        if (data > 5) data = 5;
        eeprom1->saveEventStageSettings(data);  //Store in EEPROM the EVENT STAGE 
        tempStr = F("STAGE");
        tempStr.concat(data);
        // tempStr = tempStr + data;
        done=true;
      }
    }
    else if (stringContains(str, F("STARTIME"), 8, str.length() - 1))
    {
      if (isNumeric(str))
      {
        data = str.toInt();
        if (data < 2) data = 2;
        if (data > 1200) data = 1200;
        eeprom1->saveStarDeltaTimer(data);
        tempStr = F("STARTIME");
        tempStr.concat(data);
        // tempStr = tempStr + data;
        done=true;
      }
    }
    else if (stringContains(str, F("AUTOTIME"), 8, str.length() - 1))
    {
      if (isNumeric(str))
      {
        data = str.toInt();
        if (data < 50) data = 50;
        if (data > 28800) data = 28800;
        eeprom1->saveAutoStartTimeSettings(data);  //Store in EEPROM the AUTO START TIME
        tempStr = F("AUTOTIME");
        tempStr.concat(data);
        // tempStr = tempStr + data;
        done=true;
      }
    }
    else if (stringContains(str, F("BAL"), 3, str.length() - 1))
    {
      processed=true;
      String s2 = F("AT+CUSD=1,\"");
      s2.concat(str);
      s2.concat("\"");
      sendCUSDResponse = true;
      sendCommand(s2, true);
    }
    else
    {
        if (stringContains(str, "M-", 2, 12))
        {
          if (isNumeric(str))
          {
            bool t = eeprom1->removeNumber(str);
            if(t) done=true;
#ifndef disable_debug
            _NSerial->print("Rem:");
            _NSerial->println((bool)t);
#endif
          }
        }
        else if (stringContains(str, "M+", 2, 12))
        {
          if (isNumeric(str))
          {
            bool t = eeprom1->addNumber(str);
            if(t) done=true;
#ifndef disable_debug
            _NSerial->print("Add:");
            _NSerial->println((bool)t);
#endif
          }
        }
        else if (stringContains(str, "AM+", 3, 13))
        {
          if (isNumeric(str))
          {
            bool t = eeprom1->addAlternateNumber(str);
            if(t) done=true;
#ifndef disable_debug
            _NSerial->print("Add:");
            _NSerial->println((bool)t);
#endif
          }
        }  
        else if (stringContains(str, "MM+", 3, 13))
        {
          if (isNumeric(str))
          {
            eeprom1->addM2MNumber(str);
            done=true;
          }
        }  
#ifdef ENABLE_M2M
        else if (stringContains(str, "MR+", 3, 13))
        {
          if (isNumeric(str))
          {
            eeprom1->addM2MRemoteNumber(str);
            done=true;
          }
        }  
#endif
    }

    if(!processed && !noMsg)
    {
      String temp2=F("OK : ");
      if(!done)
        temp2 = F("ERROR : ");  
      
      temp2.concat(tempStr);      
      // temp2 = temp2 + tempStr;
      sendSMS(temp2,true);
    }
}


// bool SIM::getBalance()
// {
      // String s2;
      // s2 = F("AT+CUSD=1,\"");
      // s2.concat(balStr);
      // s2.concat("\"");
      // sendCommand(s2, true);
// }
inline bool SIM::isCCID(String &str)
{
  return(str.length()>15 && isDigit(str.charAt(0)));
  // return stringContains(str, "+CSQ", 5, str.length() - 3);
}

inline bool SIM::isCBC(String &str)
{
  return(stringContains(str,"+CBC:",str.indexOf(",")+1,str.length()-1));
}

inline bool SIM::isCUSD(String &str)
{
  return stringContains(str, "+CUSD:", 6, str.length() - 1);
}

inline bool SIM::isCCLK(String &str)
{
// +CCLK: "17/10/17,15:12:50+22"
  //yy/MM/dd,hh:mm:ss±zz"
  return(stringContains(str, F("+CCLK: "), 17, 22));
}

inline bool SIM::isCSQ(String &str)
{
  return stringContains(str, "+CSQ", 5, str.length() - 3);
}

void SIM::sendReadMsg(String str)
{
  String s = "AT+CMGR=";
  s.concat(str);
  sendCommand(s, true);
}

inline bool SIM::isMsgBody(String &str)
{
  return stringContains(str, "+CMGR:", 24, 34);
}

inline bool SIM::isAdmin(String str)
{
  return (str == F(adminNumber) || str==F(adminNumber1) || str==F(adminNumber2) || str==F(adminNumber3) || str==F(adminNumber4));
}

void SIM::gotMsgBody(String &str)
{
  bool admin = isAdmin(str);
  bool alterNumber = eeprom1->isAlterNumber(str);

  if ((admin || eeprom1->isPrimaryNumber(str) || alterNumber) || eeprom1->numbersCount==0)
  {
    str = readString(); //_SSerial->readStringUntil('\n');
#ifndef disable_debug
    _NSerial->print("MSG:");
    _NSerial->println(str);
#endif
    bool noMsg=false;
    str.toUpperCase();

    if(eeprom1->numbersCount==0 && !admin)   //number does not exist in memory, and also not admin, and not M2M, and no numbers installed in EEPROM
    {
      if(!str.startsWith(F("****M+")))
      {
        return;
      }
      else
      {
        str=str.substring(4,str.length());        
      }
    }

    if(str.startsWith("#"))
    {
      str=str.substring(1,str.length());
      noMsg=true;
    }
    if(admin)
    {
      if(!checkPrgReq(str,noMsg))
        operateOnMsg(str,admin,noMsg,alterNumber);
    }
    else
      operateOnMsg(str, admin,noMsg,alterNumber);
  }
  else if(eeprom1->isM2MNumber(str))    //only used to verify the remote and m2m numbers. by m2m remote
  {
    str = readString(); //_SSerial->readStringUntil('\n');
    if(str.startsWith(F("VMM01")))
    {
      eeprom1->setM2MVerify(true);
      sendSMS(F("VMR02"),true,SEND_TO_M2M_MASTER);
    }
  }
  #ifdef ENABLE_M2M
  else if(eeprom1->isM2MRemoteNumber(str))    // to be used by M2M Master.
  {
    str = readString(); 
    if(str.startsWith(F("VMR02")))
    {
      eeprom1->setM2MRemoteVerified(true);
      eeprom1->saveM2MSettings(true);
      sendSMS(str,true);
    }
  }
  #endif
  delAllMsg();
}

inline bool SIM::isNewMsg(String &str)
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
  while(attempts==0)
  {
    // if (sendBlockingATCommand("AT\r\n"),true)
    if (sendBlockingATCommand_P(PSTR("AT\r\n"),true))
    {
      // if(sendBlockingATCommand(F("AT+COPS=0\r\n"),true))
      // {
        // if (sendBlockingATCommand(F("AT+CLIP=1\r\n")))
        if (sendBlockingATCommand_P(PSTR("AT+CLIP=1\r\n")))
        {
          // if (sendBlockingATCommand(F("AT+CLCC=1\r\n")) && sendBlockingATCommand(F("AT+CMGF=1\r\n")) &&  sendBlockingATCommand(F("AT+CNMI=2,1,0,0,0\r\n")))
          if (sendBlockingATCommand_P(PSTR("AT+CLCC=1\r\n")) && sendBlockingATCommand_P(PSTR("AT+CMGF=1\r\n")) &&  sendBlockingATCommand_P(PSTR("AT+CNMI=2,1,0,0,0\r\n")))
          {

            sendBlockingATCommand_P(PSTR("AT+CSCLK=1\r\n"));   //slow clocking mode
            sendBlockingATCommand_P(PSTR("AT+CLTS=1\r\n"));    //enable time update by network
            sendBlockingATCommand_P(PSTR("AT&W\r\n"));
            // sendBlockingATCommand(F("AT+CSCLK=1\r\n"));   //slow clocking mode
            // sendBlockingATCommand(F("AT+CLTS=1\r\n"));    //enable time update by network
            // sendBlockingATCommand(F("AT&W\r\n"));
            
            // #ifndef ENABLE_M2M
            //   setTime();
            // #endif

    #ifndef disable_debug
            _NSerial->println("INIT");
    #endif
            initialized = true;
            // sendBlockingATCommand(F("AT+DDET=1\r\n"));
            sendBlockingATCommand_P(PSTR("AT+DDET=1\r\n"));

            String tempStr  = F("AT+CCID");
            if(getBlockingResponse(tempStr,&SIM::isCCID))
            {
                String tempStr2="";
                if(!eeprom1->getCCID(tempStr2) || tempStr2!=tempStr)
                {
                    registerWithAdmin();
                    // delay(20);    //wait for 2 secs.
                    stopCallWaiting();
                    eeprom1->setCCID(tempStr);
                }
            }
            return true;
          }
        }
      // }
    }
  attempts++;
  }

  // if (attempts == 0)
  // {
    // goto try_again;
  // }
#ifndef disable_debug
  _NSerial->println("INIT X");
#endif
  return false;
}

inline void SIM::registerWithAdmin()
{
  isMsgFromAdmin=true;
  sendSMS((eeprom1->getDeviceId()),true);
}

inline bool SIM::isNumber(String &str)
{
  return (stringContains(str, "+CLIP: \"", 11, 21));
}

bool SIM::checkNumber(String number)
{
#ifndef disable_debug
  _NSerial->print("no:");
  _NSerial->println(number);
#endif

  if (isAdmin(number) || ((eeprom1->checkExists(number))!=0xFF))
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
    delay(1);
    // while (millis() - tempAcceptCommandTime <= 100)
    // {}
  }
  else
  {
    tempAcceptCommandTime = millis();
  }
}

inline void SIM::rejectCommands()
{
#ifndef disable_debug
  _NSerial->print("com");
  _NSerial->print("Rej");
  _NSerial->println(millis());
#endif
  digitalWrite(PIN_DTR,LOW);
  commandsAccepted = false;
}

void SIM::sendCommand_P(const char *cmd, bool newline = false)
{
  acceptCommands();
  char c;
  if (!cmd) 
    return;
  while ((c = pgm_read_byte(cmd++)))
    _SSerial->print (c);

  if(newline)
    _SSerial->println();
}

void SIM::sendCommand(char *cmd, bool newline = false)
{
  acceptCommands();
  if (!newline)
    _SSerial->print(cmd);
  else
    _SSerial->println(cmd);
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
  // _NSerial->println(cmd);
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

bool SIM::sendBlockingATCommand_P(const char *cmd,bool extendTime)
{
  sendCommand_P(cmd);
#ifndef disable_debug
  _NSerial->print(cmd);
#endif
  unsigned long t = millis();
  unsigned long int t2=3000L;
  if(extendTime)
    t2=60000L;
  while (millis() - t < t2)
  {
    if (_SSerial->available() > 0)
    {
      String str = readString();
      #ifndef disable_debug
        _NSerial->println(str);
      #endif
      if (str == "OK\r")
        return true;
      else if (str == "ERROR\r")
        return false;
    }
  }
  return false;
}

bool SIM::sendBlockingATCommand(char *cmd,bool extendTime)
{
  sendCommand(cmd);
#ifndef disable_debug
  _NSerial->print(cmd);
#endif
  unsigned long t = millis();
  unsigned long int t2=3000L;
  if(extendTime)
    t2=60000L;
  while (millis() - t < t2)
  {
    if (_SSerial->available() > 0)
    {
      String str = readString();
      #ifndef disable_debug
        _NSerial->println(str);
      #endif
      if (str == "OK\r")
        return true;
      else if (str == "ERROR\r")
        return false;
    }
  }
  return false;
}

bool SIM::sendBlockingATCommand(String cmd,bool extendTime)
{
  sendCommand(cmd);
#ifndef disable_debug
  _NSerial->print(cmd);
#endif
  unsigned long t = millis();
  unsigned long int t2=3000L;
  if(extendTime)
    t2=60000L;
  while (millis() - t < t2)
  {
    if (_SSerial->available() > 0)
    {
      String str = readString();
      #ifndef disable_debug
        _NSerial->println(str);
      #endif
      if (str == "OK\r")
        return true;
      else if (str == "ERROR\r")
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

// bool SIM::matchString(String m1, String m2)
// {
//   return (m1 == m2);
// }


// bool SIM::stringContains(char *sstr,const char *mstr, byte sstart, byte sstop)
// {
//   if(strncmp(sstr,mstr,strlen(mstr))==0)      //sstr starts with mstr
//   {
//     sstr=sstr+sstart;
//     char *temp;
//     temp= sstr + sstop - sstart;
//     *temp='\0';
//     return true;
//  }
// return false;
  
// }

// bool SIM::stringContains(char *temp,String &sstr, String mstr, byte sstart, byte sstop)
// {

//   if(sstr.startsWith(mstr))
//   {
//     byte j=0;
//     while(sstart<sstop)
//     {
//       temp[j++]=sstr.charAt(sstart++);      
//     }    
//     temp[j]='\0';
//     return true;
//   }
  
//   return false;

// }

bool SIM::stringContains(String &sstr, String mstr, byte sstart, byte sstop)
{
  if (sstr.startsWith(mstr))
  {
    sstr.substring(sstart, sstop);
    return true;
  }
  return false;
}

inline bool SIM::isRinging(String str)
{
  return (str == "RING\r");
}

inline bool SIM::isDTMF(String &str)
{
  return(stringContains(str, "+DTMF: ", 7, 8));
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

  if (str == F("NO CARRIER\r"))
    return true;
  else if (str == F("BUSY\r"))
    return true;
  else if (str == F("NO ANSWER\r"))
    return true;
  else if (str == "ERROR\r")
    return true;
  return false;
}

inline bool SIM::isSoundStop(String str)
{
  return (str == "+CREC: 0\r");
}

char SIM::callState(String str)
{
#ifndef disable_debug
  _NSerial->print("str:");
  _NSerial->println(str);
#endif

if(str.startsWith(F("+CLCC: 1,0,")))
{
  char c=(str.substring(11,12)).charAt(0);
  if(c=='2')
    return 'D';
  else if(c=='3')
    return 'R';
  else if(c=='0')
    return 'I';
  else if(c=='6')
    return 'E';
}
return 'N';
  // if (stringContains(str, F("+CLCC: 1,0,2"), 11, 12))    //dialling
  //   return 'D'; //call made
  // if (stringContains(str, F("+CLCC: 1,0,3"), 11, 12))    //alerting
  //   return 'R'; //call made
  // else if (stringContains(str, F("+CLCC: 1,0,0"), 11, 12)) //active call
  //   return 'I'; //call accepted
  // else if (stringContains(str, F("+CLCC: 1,0,6"), 11, 12)) //call ended
  //   return 'E'; //call ended
  // else
  //   return 'N';
}


void SIM::makeCall()
{
  inCall=true;
  acceptCommands();
  _SSerial->flush();

  char cmd[25];
  strcpy_P(cmd,PSTR("ATD+91"));
  // String command = "ATD+91";
  char num[11];
  #ifdef ENABLE_M2M
    if(eeprom1->M2M && m2mEvent)
    {
      eeprom1->getM2MRemoteNumber(num);
    }
    else
    {
        if(retries)
        {
          eeprom1->getIndexedNumber(num,retries);
        }
        else
        {
          eeprom1->getActiveNumber(num);
        }
    }
  #else
  if(retries)
  {
    eeprom1->getIndexedNumber(num,retries);
  }
  else
  {
    eeprom1->getActiveNumber(num);
  }
  #endif
  strcat(cmd,num);
  strcat(cmd,';');
  // command.concat(';');
  sendCommand(cmd, true);
  delay(1);
  // unsigned long temp = millis();
  // while (millis() - temp < 100)
  // {    }
  sendCommand_P("", true);
#ifndef disable_debug
  _NSerial->println("CallMade");
#endif
  // eeprom1->inCall(true);
  callCutWait = millis();
  currentStatus = 'R';
  currentCallStatus = 'O';
  // attemptsToCall++;
}


// void SIM::makeCall()
// {
//   inCall=true;
//   acceptCommands();
//   _SSerial->flush();

//   String command = "ATD+91";
//   #ifdef ENABLE_M2M
//     if(eeprom1->M2M && m2mEvent)
//     {
//       command.concat(eeprom1->getM2MRemoteNumber());
//     }
//     else
//     {
//         if(retries)
//         {
//           command.concat(eeprom1->getIndexedNumber(retries));
//         }
//         else
//         {
//           command.concat(eeprom1->getActiveNumber());
//         }
//     }
//   #else
//   if(retries)
//   {
//     command.concat(eeprom1->getIndexedNumber(retries));
//   }
//   else
//   {
//     command.concat(eeprom1->getActiveNumber());
//   }
//   #endif
//   command.concat(';');
//   sendCommand(command, true);
//   delay(1);
//   // unsigned long temp = millis();
//   // while (millis() - temp < 100)
//   // {    }
//   sendCommand("", true);
// #ifndef disable_debug
//   _NSerial->println("CallMade");
// #endif
//   // eeprom1->inCall(true);
//   callCutWait = millis();
//   currentStatus = 'R';
//   currentCallStatus = 'O';
//   // attemptsToCall++;
// }

void SIM::endCall()
{
  nr = 0;
  inCall=false;

  _SSerial->flush();
  sendCommand(F("AT+CHUP"), true);
  unsigned long temp = millis();
  delay(1);
  // while (millis() - temp < 100)
  // {  }
  sendCommand("", true);
  _SSerial->flush();
  freezeIncomingCalls = false;
  zeroPressed=false;

  // if(retryOn)
  // {
  //   retryOn=false;
  // } 
  // else if(!retryOn)
  // {
    #ifdef ENABLE_M2M
    if((currentStatus == 'N' || currentStatus == 'R') && currentCallStatus == 'O' && !m2mEvent) //if outgoing call
    #else
    if((currentStatus == 'N' || currentStatus == 'R') && currentCallStatus == 'O') //if outgoing call
    #endif
    {
      if(retries==0 && !callAccepted && (eeprom1->RESPONSE)=='T' && (eeprom1->numbersCount)>1)
      {
        retries=1;
        eventStaged=true;
        tempEventStageTime=millis();
        stagedEventType=actionType;
      }
    }

    #ifdef ENABLE_M2M
    if(currentStatus == 'I' && currentCallStatus == 'O' && m2mEvent && eeprom1->M2M)
    {
      m2mEventCalls++;
      if(m2mAck)
      {
        motor1->setM2MEventState(m2mEventNo,ME_CLEARED);
      }
      else
      {
        if(m2mEventCalls<2)
        {
          tempEventStageTime=millis();
          stagedEventType=m2mEventNo;
          // tempM2MEventStageTime=millis();
          // stagedM2MEventNo=m2mEventNo;
          m2mEventStaged=true;
        }
        else
        {
          motor1->setM2MEventState(m2mEventNo,ME_NOTAVAILABLE);
        }
      }
      m2mEvent = false;
    }

    keyPressed=false;
    m2mAck=false;
    #endif

  // callDialled = false;
  // callAlerted = false;
  // attemptsToCall=0;

  // eeprom1->inCall(false);

  callAccepted = false;
  // responseToAction = false;
  currentStatus = 'N';
  currentCallStatus = 'N';

  isRegisteredNumber=false;
  obtainEventTimer = millis();
  obtainNewEvent = false;
  //  starPresent=false;
#ifndef disable_debug
  _NSerial->println("CallEnd");
#endif
}

inline void SIM::setObtainEvent()
{
  if (!obtainNewEvent  && millis() - obtainEventTimer > 1000)
    obtainNewEvent = true;
}

void SIM::acceptCall()
{
  isRegisteredNumber=false;   //clear flag for next call, in case any error occures and endCall() is not called for ending the call
  callAccepted = true;
  _SSerial->flush();
  sendCommand(F("ATA"), true);
  _SSerial->flush();
  currentStatus = 'I';
  currentCallStatus = 'I';
  playSound('M');
}

void SIM::sendSMS(String msg = "", bool predefMsg = false, byte isM2M)
{
  inCall=true;
  _SSerial->flush();
  String responseString=msg;
  if (!predefMsg)
  {
    switch (actionType)
    {
      case 'S':
        responseString = STR_MOTOR;
        responseString.concat(STR_ON);
        break;
      case 'O':
      case 'U':
      case 'C':
      case 'F':
        responseString = STR_MOTOR;
        responseString.concat(STR_OFF);
        break;
      default:
        return;
    }
  }
  // _NSerial->println(responseString);
// #ifndef disable_debug
  // _NSerial->println("SMS");
// #endif
  String command =  "AT+CMGS=\"+91";
  

  if(isM2M==SEND_TO_M2M_MASTER)
  {
    command.concat(eeprom1->getM2MNumber());
  }
  #ifdef ENABLE_M2M
  else if(isM2M==SEND_TO_M2M_REMOTE)
  {
    command.concat(eeprom1->getM2MRemoteNumber());
  }
  #endif
  else 
  {
    if (isMsgFromAdmin)
      command.concat(F(adminNumber));
    else
      command.concat(eeprom1->getActiveNumber());
  }

  command.concat("\"");

  _SSerial->flush();
  sendCommand(command, true);
  _SSerial->flush();
  // unsigned long int temp = millis();
  // while (millis() - temp < 1000)
  // {}
  delay(10);
  sendCommand(responseString, true);
  _SSerial->flush();
  delay(10);
  // temp = millis();
  // while (millis() - temp < 1000)
  // {}
  _SSerial->flush();
  sendCommand((char)26, true);
  _SSerial->flush();
  // temp = millis();
  // while (millis() - temp < 1000)
  // {}
  delay(10);
  isMsgFromAdmin = false;
  inCall=false;
}

void SIM::sendSMS(char *msg, bool predefMsg = false, byte isM2M)
{
  inCall=true;
  _SSerial->flush();

  char respMsg[25];
  if (!predefMsg)
  {
    switch (actionType)
    {
      case 'S':
        strcpy_P(respMsg,PSTR(STR_MOTOR));
        strcat_P(respMsg,PSTR(STR_ON));
        break;
      case 'O':
      case 'U':
      case 'C':
      case 'F':
        strcpy_P(respMsg,PSTR(STR_MOTOR));
        strcat_P(respMsg,PSTR(STR_OFF));
        break;
      default:
        return;
    }
  }
  char cmd[30];
  char num[11];
  strcpy_P(cmd,PSTR("AT+CMGS=\"+91"));
  // String command =  "AT+CMGS=\"+91";
  if(isM2M==SEND_TO_M2M_MASTER)
  {
    eeprom1->getM2MNumber(num);
  }
  #ifdef ENABLE_M2M
  else if(isM2M==SEND_TO_M2M_REMOTE)
  {
    eeprom1->getM2MRemoteNumber(num);
  }
  #endif
  else 
  {
      // command.concat(adminNumber);

    if (isMsgFromAdmin)
      strcpy_P(num,PSTR(adminNumber));
    else
      eeprom1->getActiveNumber(num);
      // command.concat(eeprom1->getActiveNumber());
  }

  strcat(cmd,num);
  strcat_P(cmd,"\"");
  // command.concat("\"");
  _SSerial->flush();
  sendCommand(cmd, true);
  // sendCommand(command, true);
  _SSerial->flush();
  // unsigned long int temp = millis();
  // while (millis() - temp < 1000)
  // {}
  delay(10);
  if(!predefMsg)
    sendCommand(respMsg, true);
  else
    sendCommand(msg, true);

  _SSerial->flush();
  delay(10);
  // temp = millis();
  // while (millis() - temp < 1000)
  // {}
  _SSerial->flush();
  sendCommand((char)26, true);
  _SSerial->flush();
  // temp = millis();
  // while (millis() - temp < 1000)
  // {}
  delay(10);
  isMsgFromAdmin = false;
  inCall=false;
}

void SIM::delay(byte time)
{
  unsigned long temp = millis();
  while (millis() - temp < time*100)
  {}
}

void SIM::operateDTMF(String s)
{
  char str=s.charAt(0);

  #ifdef ENABLE_M2M
  if(m2mEvent)
  {
    if(str == 'A')
    {
      m2mAck=true;
      endCall();
    }
  }
  else
  #endif
  {
    if (str == '1') //Motor On
    {
      currentOperation = 'S';
      subDTMF();
      motor1->startMotor(true);
    }
    else if (str == '2') //Motor Off
    {
      currentOperation = 'O';
      subDTMF();
      motor1->stopMotor(true);
    }
    else if (str == '3') //Status
    {
      currentOperation = 'T';
      subDTMF();
      motor1->statusOnCall();
    }
  #ifdef ENABLE_WATER
    else if (str == '4') //underground status
    {
      currentOperation = 'W';
      subDTMF();
      motor1->waterStatusOnCall();
    }
  #ifdef ENABLE_GP
    else if (str == '5') //overHead Status
    {
      currentOperation = 'V';
      subDTMF();
      motor1->overHeadWaterStatusOnCall();
    }
  #endif
  #endif
  #ifdef ENABLE_CURRENT
    else if(str == '0')
    {
      if(zeroPressed)
      {
        motor1->autoSetCurrent();   //to enable or disable current detection
        subDTMF();
        zeroPressed=false;
      }
        zeroPressed=true;
    }
    else if (str == '7') //Speak Current Ampere On Call
    {
      subDTMF();
      motor1->speakAmpere();
      // eeprom1->saveAutoStartSettings(true);  //set AutoStart to True in EEPROM
      // motor1->resetAutoStart(true);
        // responseToAction = true;
        // playSound('8');     // playFile AutoStart is On
    }
    // else if(str=='7')   // increase overload per by 10
    // {
    //     eeprom1->setOverloadPer(eeprom1->OVERLOADPER+10);
    // }
    // else if(str=='8')   // decrease overload per by 10
    // {
    //     eeprom1->setOverloadPer(eeprom1->OVERLOADPER-10);
    // }
  #endif
    else if(str=='D')
    {
      currentOperation = '1';   //m2m 1
      eeprom1->saveAutoStartSettings(true);  //set AutoStart to True in EEPROM
      motor1->resetAutoStart(true);
      motor1->startMotor();
      sendDTMFTone(0xFF);
    }
    else if(str=='C')
    {
      currentOperation = '2';   //m2m 2
      eeprom1->saveAutoStartSettings(false);  //set AutoStart to false in EEPROM
      motor1->stopMotor(false,false,true);
      sendDTMFTone(0xFF);
    }
    else if (str == '8') //Set AUTOTIMER ON
    {
      subDTMF();
      eeprom1->saveAutoStartSettings(true);  //set AutoStart to True in EEPROM
      motor1->resetAutoStart(true);
        // responseToAction = true;
        playSound('8');     // playFile AutoStart is On
    }
    else if (str == '9') //Set AUTOTIMER OFF
    {
      subDTMF();
      eeprom1->saveAutoStartSettings(false);  //set AUtoStart to False in EEPROM
      motor1->resetAutoStart(true);
        // responseToAction = true;
        playSound('9'); //playFile autoStart is turned oFF
    }
  }
}

inline void SIM::subDTMF()
{
  //    // starPresent=true;
  stopSound();
  callCutWait = millis();
}

void SIM::operateRing()
{
  nr++;
  if (nr <= 4)
  {
    if (nr == 1)
    {
      sendCommand(F("AT+DDET=1\r\n"));
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
      if(nr>1)
      {
          if(!isRegisteredNumber)           //if (nr > 1 && !checkNumber(str))
            isRegisteredNumber = checkNumber(str);  //   endCall();
      }
    }
  }
  else if (nr >= 5)
  {
    if(!isRegisteredNumber)
    {
      endCall();
    }
  }
  
  if(isRegisteredNumber)
  {
    callCutWait = millis();
    acceptCall();      
  }
}

inline bool SIM::playSoundElligible()
{
  return (bplaySound && ((millis() - soundWait) > (soundWaitTime * 100)));
}


// inline void SIM::triggerPlaySound()
// {
//   _SSerial->flush();
//   sendCommand(F("AT+CREC=4,\""));
//   sendCommand(F("AT+CREC=4,\""));
//   sendCommand("C:\\User\\FTP\\");
//   sendCommand(playFile);
//   sendCommand(F(".amr\",0,100,"));
//   if(maxPlayingFiles>1)
//     sendCommand(F("0\r"), true);
//   else
//     sendCommand(F("1\r"), true);

//   // sendCommand(F(".amr\",0,100,1\r"), true);
//   _SSerial->flush();
//   bplaySound = false;
// }

inline void SIM::triggerPlaySound()
{
  _SSerial->flush();
  sendCommand_P(PSTR ("AT+CREC=4,\""));
  sendCommand_P(PSTR ("C:\\User\\FTP\\"));
  sendCommand(playFile);
  sendCommand_P(PSTR(".amr\",0,100,"));
  if(maxPlayingFiles>1)
    sendCommand_P(PSTR("0\r"), true);
  else
    sendCommand_P(PSTR("1\r"), true);
  _SSerial->flush();
  bplaySound = false;
}

void SIM::playSoundAgain(String str)
{
  if (isSoundStop(str))
  {
    if(maxPlayingFiles>1 && currentPlayingFileIndex<maxPlayingFiles-1)
    {
      playSound(playFilesList[++currentPlayingFileIndex]);
    }
    else
    {
      playSound('M');
    }
// //       if (starPresent)
//       {
//           if(playFile==actionType)
//             playFile='N';
//           else
//             playFile=actionType;
//       }
//       playSound(playFile);
  }
}

// void SIM::playRepeatedFiles(String &fileList)
// {
//   _SSerial->flush();
//   stopSound();
//   if(fileList.length()<5)
//   {
//     currentPlayingFileIndex=0;
//     maxPlayingFiles=fileList.length();
//     while(currentPlayingFileIndex<maxPlayingFiles)
//     {
//       playFilesList[currentPlayingFileIndex]=fileList.charAt(currentPlayingFileIndex++);
//     }
//     currentPlayingFileIndex=0;
//     soundWait = millis();
//     playFile = playFilesList[currentPlayingFileIndex];
//     bplaySound = true;
//   }
// }

void SIM::playRepeatedFiles(char *fileList)
{
  _SSerial->flush();
  stopSound();
  if(strlen(fileList)<5)
  {
    currentPlayingFileIndex=0;
    maxPlayingFiles=strlen(fileList);
    strcpy(playFilesList,fileList);
    // while(currentPlayingFileIndex<maxPlayingFiles)
    // {
    //   playFilesList[currentPlayingFileIndex]=fileList[currentPlayingFileIndex++];
    // }
    // currentPlayingFileIndex=0;
    soundWait = millis();
    playFile = playFilesList[currentPlayingFileIndex];
    bplaySound = true;
  }

  // if(fileList.length()<5)
  // {
  //   currentPlayingFileIndex=0;
  //   maxPlayingFiles=fileList.length();
  //   while(currentPlayingFileIndex<maxPlayingFiles)
  //   {
  //     playFilesList[currentPlayingFileIndex]=fileList.charAt(currentPlayingFileIndex++);
  //   }
  //   currentPlayingFileIndex=0;
  //   soundWait = millis();
  //   playFile = playFilesList[currentPlayingFileIndex];
  //   bplaySound = true;
  // }
}

void SIM::playSound(char actionType, bool newAction)
{
  _SSerial->flush();
  stopSound();
  soundWait = millis();
  bplaySound = true;
  if (newAction)
  {
    maxPlayingFiles=1;
    currentPlayingFileIndex=0;
    playFilesList[currentPlayingFileIndex]=actionType;
    this->actionType = actionType;
  }
  playFile = actionType;
}

inline void SIM::stopSound()
{
  _SSerial->flush();
  sendCommand(F("AT+CREC=5\r"), true);
  _SSerial->flush();
}

inline bool SIM::callTimerExpire()
{
  return ((millis() - callCutWait) >= (callCutWaitTime * 100));
}

inline void SIM::makeResponseAction()
{
  #ifndef ENABLE_M2M
  if(eeprom1->RESPONSE!='N')
  #else
  if(eeprom1->RESPONSE!='N' || m2mEvent)
  #endif
  {
      freezeIncomingCalls = true;
      acceptCommands();
      makeCall();
  }
  // if (eeprom1->RESPONSE == 'A' || eeprom1->RESPONSE == 'C' || eeprom1->RESPONSE == 'T' || (m2mEvent && eeprom1->RESPONSE=='N'))
}

bool SIM::registerEvent(char eventType)
{
  if(eeprom1->numbersCount==0 || eeprom1->RESPONSE=='N')
  {
    return true;
  }

  if (!initialized)
  {
#ifndef disable_debug
    _NSerial->println("NO SIM");
#endif
    return true;
  }

  if(!eventStaged && actionType==eventType)
    return true;

  #ifdef ENABLE_M2M
  if (currentStatus == 'N' && currentCallStatus == 'N' && obtainNewEvent && !eventStaged && !m2mEventStaged)
  #else
  if (currentStatus == 'N' && currentCallStatus == 'N' && obtainNewEvent && !eventStaged)
  #endif
  {    
#ifndef disable_debug
    _NSerial->print("E:");
    _NSerial->print(eventType);
#endif

    #ifndef ENABLE_M2M
    #ifndef ENABLE_CURRENT
    if(eeprom1->NOCALL && checkNoCallTime())
    {
        return true; 
    }
    #endif
    #endif

    retries=0;
    if(eeprom1->EVENTSTAGE>0x00)
    {
      tempEventStageTime=millis();
      stagedEventType=eventType;
      eventStaged=true;
    }
    else
    {
      actionType = eventType;
      makeResponseAction();
    }
    return true;
  }
  else
    return false;
}

inline bool SIM::rejectCommandsElligible()
{
  return (commandsAccepted && millis() - tempAcceptCommandTime >= (acceptCommandsTime * 100));
}

inline void SIM::checkNetwork(String str)
{
  if (str == F("+CPIN: NOT READY\r"))
  {
    endCall();
    startSIMAfterUpdate();
  }
}

// void SIM::networkCounterMeasures()
// {
//   // _SSerial->flush();
//   // sendBlockingATCommand(F("AT+CFUN=1,1\r\n"));
//   // _SSerial->flush();
//   // unsigned long t = millis();
//   // delay(1);
//   // t=millis();
//   // while (millis() - t < 100)
//   // {}
// }

void SIM::setMotorMGRResponse(char response)
{  
  if(currentStatus!='I')    // not in Call than return.
  {
    return;
  }

  // responseToAction = true;
  { 
    playSound(response);
  }
  // if (currentOperation == 'S') //start Motor
  // {
  //   if (response == 'L')
  //     playSound('N');  //cannot start motor
  //   else if (response == 'O')
  //     playSound('1');  //motor is already on
  //   else if (response == 'D')
  //     playSound('S');  //motor has started
  //   else
  //     playSound(response);  //other response than specified, handled by class SIM.
  //   // endCall();
  // }
  // else if (currentOperation == 'O') //switch off motor
  // {
  //   if (response == 'L')
  //     playSound('P');    //cannot stop motor
  //   else if (response == 'O')
  //     playSound('2');  //motor is already off
  //   else if (response == 'D')
  //     playSound('O');  //motor has stopped
  //   else
  //     playSound(response);  //other response than specified, handled by class SIM.
  // }
  // else if (currentOperation == 'T')
  // {
  //   if (response == 'L') //motor off, no light
  //     playSound('L');
  //   else if (response == 'A') //motor off, light on
  //     playSound('A');
  //   // else if (response == 'B') //motor off, light on
  //     // playSound('B');
  //   else if (response == 'O') //motor off, light on
  //     playSound('3');
  //   else if (response == 'D')
  //     playSound('1');  //motor is on
  //   else
  //     playSound(response);  //other response than specified, handled by class SIM.
  // }
  // else      //for all other currentOperation i.e. underground status, overhead status etc.
  // { 
  //   playSound(response);
  // }
}

inline bool SIM::isCallReady(String str)
{
  return (str == F("Call Ready\r"));
}

bool SIM::checkSleepElligible()
{
  #ifdef ENABLE_M2M
  return(!commandsAccepted  && checkNotInCall() && !m2mEventStaged && !eventStaged);
  #else
  return(!commandsAccepted  && checkNotInCall() && !eventStaged);
  #endif
}

bool SIM::checkNotInCall()
{
 return ( !sendCUSDResponse     &&  currentStatus=='N'  
  &&  currentCallStatus=='N'  &&  obtainNewEvent
  &&  !freezeIncomingCalls && !busy());
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

bool SIM::busy()
{
  return (inCall || inInterrupt);
}

inline bool SIM::checkEventGone()
{
  return (millis() - tempInterruptTime > 2000);
}

String SIM::makeStatusMsg(byte battery, byte network)
{
      byte t3 = motor1->checkLineSensors();
      bool t5 = motor1->motorState();
      String resp = F("AUTOON");

      if (!eeprom1->AUTOSTART)
        resp = F("AUTOOFF");

      resp.concat("\nAC:");
      resp.concat((t3==AC_3PH ? F(" ON\n") : (t3==AC_2PH ? F(" 2 PHASE\n") : F(" OFF\n"))));
      resp.concat(F("Motor:"));
      resp.concat((t5 ? F(" ON\n") : F(" OFF\n")));
      if(battery!=0xFF)
      {
        resp.concat(F("\nBat%:"));
        resp.concat(battery);
      }
      if(network!=0xFF)
      {
        resp.concat(F("\nNetwork:"));
        resp.concat(network);
      }
      return resp;
}

inline void SIM::checkRespSMS(char t1)
{
  if (!callAccepted && eeprom1->RESPONSE=='A')
  {
#ifndef disable_debug
        _NSerial->print("DIAL");
        _NSerial->println("OFF");
#endif
          actionType = t1;
          sendSMS();
  }
}

void SIM::operateOnStagedEvent()
{
  #ifdef ENABLE_M2M
    byte temp1=1;
    // if(m2mEventStaged)
    // {
    //   temp1=1;
    // } 
    if(eventStaged)
    {
      temp1=eeprom1->EVENTSTAGE;
    }
  #else
    byte temp1=eeprom1->EVENTSTAGE;
  #endif 


  if(obtainNewEvent && (retries==1 || (millis()-tempEventStageTime>((unsigned long)temp1*60000L))))
  {
      // freezeIncomingCalls = true;
      // acceptCommands();
      #ifdef ENABLE_M2M

        if(eventStaged)
        {
          actionType=stagedEventType;
          eventStaged=false;
        }
        else if(m2mEventStaged)
        {
          m2mEventNo=stagedEventType;
          m2mEvent=true;
          m2mEventStaged=false;
        }
      #else
          actionType=stagedEventType;
          eventStaged=false;
      #endif
      makeResponseAction();
  }
}

void SIM::sendDTMFTone(byte eventNo)
{
  String str=F("AT+VTS=\"");

  if(eventNo==0xFF)
  {
    str.concat('A');
  }
  #ifdef ENABLE_M2M
  else if(eventNo==0)
  {
    str.concat('D');
  }
  else if(eventNo==1)
  {
    str.concat('C');
  }
  #endif
  str.concat(F("\",2"));

   _SSerial->flush();
  sendCommand(str, true);
  _SSerial->flush();
}


unsigned short int SIM::getBatVolt()
{
  unsigned short int retVal=0;
  String str = F("AT+CBC");
  // +CBC: 0,76,4010
        // batPer = (str.substring(0,str.lastIndexOf(","))).toInt();

  if(getBlockingResponse(str,&SIM::isCBC))
  {
    byte temp2 = str.lastIndexOf(",");
    retVal = (str.substring(temp2+1,temp2+4)).toInt();
  }
  return retVal;
}

#ifndef ENABLE_M2M
#ifndef ENABLE_CURRENT
// void SIM::setSystemTime(String time)
// {
//     globalHours=(time.substring(0,2)).toInt();
//     globalMinutes=(time.substring(2,4)).toInt();
//     globalSeconds=0;
// }


void SIM::getSystemTime(byte &Hours, byte &Minutes)
{
  String time = F("AT+CCLK?");
  if(getBlockingResponse(time,&SIM::isCCLK))
  {
    // +CCLK: "17/10/17,15:12:50+22"
    Hours=(time.substring(0,2)).toInt();
    Minutes=(time.substring(3,5)).toInt();
    // globalSeconds=0;
  }
}
#endif
#endif

void SIM::update()
{
  // #ifndef ENABLE_M2M
    // if(triggerTimeUpdate)
      // updateTime();
  // #endif

  if(inInterrupt && checkEventGone())
    inInterrupt=false;

  if (rejectCommandsElligible() && !motor1->ACPowerState())
  {
    rejectCommands();
  }

  if (currentStatus == 'N' && currentCallStatus == 'N')
  {
    setObtainEvent();
    
    #ifdef ENABLE_M2M
    if(eventStaged || m2mEventStaged)
    {
      operateOnStagedEvent();
    }
    #else
    if(eventStaged)
    {
      operateOnStagedEvent();
    }
    #endif
  }
  else if (currentStatus == 'I' || currentStatus == 'R')
  {
    if (callTimerExpire())
    {
      char t1 = actionType;
      endCall();
      checkRespSMS(t1);
    }

    #ifdef ENABLE_M2M
    if(m2mEvent && callAccepted)
    {
        if(!keyPressed &&  millis() - callCutWait > 2000)
        {
          keyPressed=true;
          sendDTMFTone(m2mEventNo);
        }
    }
    #endif

    if (playSoundElligible())
      triggerPlaySound();
  }

  while (_SSerial->available() > 0)
  {
    String str = readString();

    if(isCUSD(str) && sendCUSDResponse)    //bal
    {
       sendSMS(str,true);
       sendCUSDResponse=false;
    }

    if (isNewMsg(str))
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
        inCall=true;
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
      // if (callState(str) == 'D')
      // {
        // #ifndef disable_debug
        // _NSerial->print("DIAL");
        // _NSerial->println("ON");
        // #endif
        // callDialled=true;
      // }
      if (callState(str) == 'R')
      {
        // #ifndef disable_debug
        // _NSerial->print("ALRT");
        // _NSerial->println("ON");
        // #endif
        // callAlerted = true;
        callCutWait = millis();
        currentStatus = 'R';
        currentCallStatus = 'O';
      }
      else if (isCut(str) || callState(str) == 'E') //
      {
        char t1 = actionType;
        endCall();
        checkRespSMS(t1);
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
        #ifdef ENABLE_M2M
          if (!m2mEvent)
            playSound(actionType);
        #else
          playSound(actionType);
        #endif
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
        playSoundAgain(str);
      }
    }
  }
}
