#include <Arduino.h>
#include "S_EEPROM.h"

// #ifndef disable_debug
String S_EEPROM::getNumbers()
{
  String str="";
  for(byte i=0;i<numbersCount;i++)
  {
    str=str+read_StringEE(mobileNumberAddress + (i*11),10);
    str=str+"\n";    
  }
  return str;
}
// #endif

S_EEPROM::S_EEPROM()
{
    numbersCount = 0;
    PROGSIZE=0;
}

byte S_EEPROM::checkExists(String &number)
{
  if (numbersCount > 0)
  {
 
      // if (alterNumber == number)
      if(isPrimaryNumber(number))
        return 0;

      // if(alterNumberSetting && number == read_StringEE(alterNumberAddress, 10))
        // return 0;

    // if (primaryNumber == number)
      for (byte i=1;i<numbersCount;i++)      // not to check for primaryNumber as it is checked in isPrimaryNUmber() above, so i=1
      {
          if(read_StringEE(mobileNumberAddress + (i*11),10)==number)
            return i;
      }
    // if(read_StringEE(mobileNumberAddress, 10)==number)
    //   return 0;

    //   secondary[temp - 1] = read_StringEE(mobileNumberAddress + (11 * temp), 10);

    // for (byte i = 0; i < numbersCount - 1; i++)
    // {
    //   if (secondary[i] == number)
    //     return i + 1;
    // }
  }
  return 0xFF;
}

bool S_EEPROM::addNumber(String &number)
{
  if (numbersCount == MAXNUMBERS)
    return false;
  else
  {
    if (checkExists(number) == 0xFF)
    {
      write_StringEE(mobileNumberAddress + (11 * numbersCount), number);
      EEPROM.put(numbersCountAddress,++numbersCount);

      // if (numbersCount == 0)
      //   primaryNumber = number;
      // else if (numbersCount < 5)
      //   secondary[numbersCount - 1] = number;

      // updateNumberChanges();
      return true;
    }
  }
  return false;
}

// bool S_EEPROM::saveBalNumber(String &number)
// {
//   if(number.startsWith("*") && number.endsWith("#") && number.length()>=5 && number.length()<=9)
//   {
//     EEPROM.put(balNumberPresentAddress, true);
//     write_StringEE(balNumberAddress, number);
//     return true;
//   }
//   return false;
// }

// bool S_EEPROM::getBalNumber(String &number)
// {
//   bool b= EEPROM.get(balNumberPresentAddress);
//   if(b!=false)
//     number = read_StringEE(balNumberAddress, 9);
//   return b;
// }

bool S_EEPROM::isAlterNumber(String number)
{
  if(numbersCount>0)
  {
    if(alterNumberPresent && number == read_StringEE(alterNumberAddress, 10))
      return true;
  }
  return false;
}

bool S_EEPROM::isPrimaryNumber(String number)
{
  if (numbersCount > 0)
  {
      if(read_StringEE(mobileNumberAddress,10)==number)
        return true;
        return isAlterNumber(number);
    // if (primaryNumber == str)
      // return true;
    // if (alterNumberSetting && alterNumber == str)
      // return true;
  }
  return false;
}

String S_EEPROM::getActiveNumber()
{
  if (numbersCount > 0)
  {
    return(read_StringEE((!alterNumberSetting ? mobileNumberAddress : alterNumberAddress),10));
  }
  else
    return (adminNumber); //="AT+CMGS=\"+917698439201\"";
}

bool S_EEPROM::addAlternateNumber(String &number)
{
  if (numbersCount > 0)
  {
    // alterNumber = number;
    alterNumberPresent = true;
    EEPROM.put(alterNumberPresentAddress, alterNumberPresent);
    write_StringEE(alterNumberAddress, number);
    return true;
  }
  return false;
}

bool S_EEPROM::removeNumber(String &number)
{
  if (numbersCount < 2)
    return false;
  else
  {
    byte loc = checkExists(number);    
    if (loc != 0xFF && loc != 0x00)
    {

      // secondary[loc - 1] = "";
      for (byte i = loc; i < (numbersCount-1); i++)
      {
        write_StringEE(mobileNumberAddress + (i*11), read_StringEE(mobileNumberAddress + ((i+1)*11),10));
        // secondary[i - 1] = secondary[i];
      }
      EEPROM.put(numbersCountAddress,--numbersCount);
      // updateNumberChanges();
      return true;
    }
  }
  return false;
}

// void S_EEPROM::updateNumberChanges()
// {
  // EEPROM.put(numbersCountAddress, numbersCount);
  // if (numbersCount > 0)
  // {
    // write_StringEE(mobileNumberAddress, primaryNumber);
    // for (byte temp = 1; temp < numbersCount; temp++)
    // {
      // write_StringEE(mobileNumberAddress + (11 * temp), secondary[temp - 1]);
    // }
  // }
// }

void S_EEPROM::loadNumberSettings()
{
  EEPROM.get(numbersCountAddress, numbersCount);
  if(numbersCount==0xFF || numbersCount > MAXNUMBERS)
  {
    numbersCount=0;
    EEPROM.put(numbersCountAddress,numbersCount);
  }

  EEPROM.get(alterNumberPresentAddress, alterNumberPresent);    //whether alterNUmber is present or not
  if (alterNumberPresent == 0xFF)
  {
    alterNumberPresent=false;
    EEPROM.put(alterNumberPresentAddress, alterNumberPresent);
  }

  EEPROM.get(alterNumberSettingAddress, alterNumberSetting);    //whether alterNUmber is On or not
  if (alterNumberSetting == 0xFF)
    saveAlterNumberSetting(false);
}
  // if (numbersCount > 0 && numbersCount != 0xFF)
  // {
    // primaryNumber = read_StringEE(mobileNumberAddress, 11);
    // for (byte temp = 1; temp < numbersCount; temp++)
    // {
      // secondary[temp - 1] = read_StringEE(mobileNumberAddress + (11 * temp), 10);
    // }
  // }
  // else
  // {
    // clearLoadedNumbers();
  // }

// void S_EEPROM::clearLoadedNumbers()
// {
  // numbersCount = 0;
  // primaryNumber = "";
  // for (byte temp = 0; temp < 4; temp++)
  // {
    // secondary[temp] = "";
  // }
// }

void S_EEPROM::saveAlterNumberSetting(bool temp)
{
  alterNumberSetting = temp;
  EEPROM.put(alterNumberSettingAddress, alterNumberSetting);
}

void S_EEPROM::saveAutoStartSettings(bool temp)
{
  AUTOSTART = (byte)temp;
  EEPROM.put(autoStartAddress, AUTOSTART);
}

void S_EEPROM::saveProgramSize(unsigned long int temp)
{
  programSizeSet=true;
  PROGSIZE = temp;
  EEPROM.put(prgSizeAddress, PROGSIZE);
}

void S_EEPROM::saveAutoStartTimeSettings(unsigned short int temp)
{
  AUTOSTARTTIME = temp;
  EEPROM.put(autoStartTimeAddress, AUTOSTARTTIME);
}

void S_EEPROM::saveDNDSettings(bool temp)
{
  DND = (byte)temp;
  EEPROM.put(dndAddress, DND);
}

void S_EEPROM::saveBypassSettings(bool temp)
{
  BYPASS = (byte)temp;
  EEPROM.put(bypassAddress, BYPASS);
}

void S_EEPROM::saveResponseSettings(char temp)
{
  RESPONSE = temp;
  EEPROM.put(responseAddress, RESPONSE);
}

void S_EEPROM::updateFirmware(bool temp,bool verify)
{
  EEPROM.put(prgUpdateRequestAddress, (byte)temp); 
  EEPROM.put(prgUpdateTryAddress, 0x00);
  EEPROM.put(VerifyStatusAddress, (byte)!verify);
}

// void S_EEPROM::saveTempSettings(unsigned short int temp)
// {
  // EEPROM.put(highTempAddress, temp);
  // EEPROM.get(highTempAddress, HIGHTEMP);
// }

// void S_EEPROM::loadTempSettings()
// {
  // EEPROM.get(highTempAddress, HIGHTEMP);
  // if (HIGHTEMP == 0xFFFF)
    // saveTempSettings(50);
// }

void S_EEPROM::loadAutoStartSettings()
{
  EEPROM.get(autoStartAddress, AUTOSTART);
  if (AUTOSTART == 0xFF)
    saveAutoStartSettings(false);
}

#ifdef ENABLE_WATER

void S_EEPROM::loadPreventOverFlowSettings()
{
  EEPROM.get(preventOverFlowAddress, PREVENTOVERFLOW);
  if (PREVENTOVERFLOW == 0xFF)
    savePreventOverFlowSettings(false);
}

void S_EEPROM::savePreventOverFlowSettings(bool temp)
{
  PREVENTOVERFLOW = (byte)temp;
  EEPROM.put(preventOverFlowAddress, PREVENTOVERFLOW);
}

#endif

void S_EEPROM::loadAutoStartTimeSettings()
{
  EEPROM.get(autoStartTimeAddress, AUTOSTARTTIME);
  if (AUTOSTARTTIME == 0xFFFF)
    saveAutoStartTimeSettings(50);
}

unsigned long int S_EEPROM::getProgramSize()
{
  return PROGSIZE;
}

byte S_EEPROM::getUpdateStatus()
{
  byte b;
  EEPROM.get(prgUpdateStatusAddress,b);
  return b;
}

void S_EEPROM::discardUpdateStatus()
{
  EEPROM.put(prgUpdateStatusAddress, 0);
}

void S_EEPROM::loadDNDSettings()
{
  EEPROM.get(dndAddress, DND);
  if (DND == 0xFF)
    saveDNDSettings(false);
}

void S_EEPROM::loadBypassSettings()
{
  EEPROM.get(bypassAddress, BYPASS);
  if (BYPASS == 0xFF)
    saveBypassSettings(false);
}


void S_EEPROM::loadResponseSettings()
{
  EEPROM.get(responseAddress, RESPONSE);
  if ((byte)RESPONSE == 0xFF)
    saveResponseSettings('C');
}

// void S_EEPROM::loadAlterNumberSettings()
// {
 // 
// }

// void S_EEPROM::loadAlterNumber()
// {
//   // if (alterNumberPresent == (byte)true)
//   // {
//     // alterNumber = read_StringEE(alterNumberAddress, 11);
//   // }
// }

void S_EEPROM::loadCCID()
{
  EEPROM.get(simCCIDPresentAddress, simCCIDPresent);
   if (simCCIDPresent == 0xFF)
   {
     simCCIDPresent=false;
     EEPROM.put(simCCIDPresentAddress, simCCIDPresent);
   }
}

bool S_EEPROM::getCCID(String &ccid)
{
  if (!simCCIDPresent)
    return false;
  else
  {
    byte length;
    EEPROM.get(simCCIDLengthAddress,length);
    ccid = read_StringEE(simCCIDAddress,length);
    return true;
  }
}

void S_EEPROM::setCCID(String &ccid)
{
  simCCIDPresent=true;
  EEPROM.put(simCCIDPresentAddress, simCCIDPresent);
  byte length = ccid.length();
  EEPROM.put(simCCIDLengthAddress,length);
  write_StringEE(simCCIDAddress, ccid);
}

String S_EEPROM::getDeviceId()
{
  String str= F("ID:");
  unsigned long int temp;
  EEPROM.get(deviceIdAddress,temp);
  str = str + temp;
  return str;
}

void S_EEPROM::loadStarDeltaTimer()
{
  EEPROM.get(starDeltaTimerAddress,starDeltaTimerTime);
  if(starDeltaTimerTime==0xFFFF)
    saveStarDeltaTimer(2);
}

void S_EEPROM::loadEventStageSettings()
{
  EEPROM.get(eventStageAddress,EVENTSTAGE);
  if(EVENTSTAGE==0xFF)
    saveEventStageSettings(0);
}

void S_EEPROM::saveStarDeltaTimer(unsigned short int data)
{
  starDeltaTimerTime=data;
  EEPROM.put(starDeltaTimerAddress,starDeltaTimerTime);
}

void S_EEPROM::saveEventStageSettings(byte data)
{
  EVENTSTAGE=data;
  EEPROM.put(eventStageAddress,EVENTSTAGE);
}

void S_EEPROM::loadAllData()
{
  // loadTempSettings();

  loadEventStageSettings();
  loadBypassSettings();

  loadAutoStartSettings();
  loadAutoStartTimeSettings();
  loadDNDSettings();
  loadResponseSettings();
  loadNumberSettings();
  loadCCID();
  loadStarDeltaTimer();
  #ifdef ENABLE_WATER
    loadPreventOverFlowSettings();
  #endif
  // loadNumbers();
  // loadAlterNumber();
}

void S_EEPROM::clearNumbers(bool admin = false)
{
  unsigned short int i;
  if(admin) numbersCount=0;
  else numbersCount=1;  

  EEPROM.put(numbersCountAddress, numbersCount);

  saveAlterNumberSetting(false);
  alterNumberPresent = false;
  EEPROM.put(alterNumberPresentAddress, alterNumberPresent);
}

bool S_EEPROM::write_StringEE(unsigned short int Addr, String input)
{
  char cbuff[input.length() + 1]; //Finds length of string to make a buffer
  input.toCharArray(cbuff, input.length() + 1); //Converts String into character array
  return eeprom_write_string(Addr, cbuff); //Saves String
}

String S_EEPROM::read_StringEE(unsigned short int Addr, byte length)
{
  char cbuff[length + 1];
  eeprom_read_string(Addr, cbuff, length + 1);

  String stemp(cbuff);
  return stemp;
}

bool S_EEPROM::eeprom_read_string(unsigned short int addr, char* buffer, byte bufSize) {
  byte ch; // byte read from eeprom
  byte bytesRead; // number of bytes read so far

  if (!eeprom_is_addr_ok(addr)) { // check start address
    return false;
  }

  if (bufSize == 0) { // how can we store bytes in an empty buffer ?
    return false;
  }

  // is there is room for the string terminator only, no reason to go further
  if (bufSize == 1) {
    buffer[0] = 0;
    return true;
  }

  bytesRead = 0; // initialize byte counter
  ch = EEPROM.read(addr + bytesRead); // read next byte from eeprom
  buffer[bytesRead] = ch; // store it into the user buffer
  bytesRead++; // increment byte counter

  // stop conditions:
  // - the character just read is the string terminator one (0x00)
  // - we have filled the user buffer
  // - we have reached the last eeprom address
  while ( (ch != 0x00) && (bytesRead < bufSize) && ((addr + bytesRead) <= EEPROM_MAX_ADDR) ) {
    // if no stop condition is met, read the next byte from eeprom
    ch = EEPROM.read(addr + bytesRead);
    buffer[bytesRead] = ch; // store it into the user buffer
    bytesRead++; // increment byte counter
  }

  // make sure the user buffer has a string terminator, (0x00) as its last byte
  if ((ch != 0x00) && (bytesRead >= 1)) {
    buffer[bytesRead - 1] = 0;
  }

  return true;
}

bool S_EEPROM::eeprom_write_string(unsigned short int addr, const char* str)
{
  byte numBytes; // actual number of bytes to be written
  numBytes = strlen(str) + 1;
  return eeprom_write_bytes(addr, (const byte*)str, numBytes);
}

bool S_EEPROM::eeprom_is_addr_ok(unsigned short int addr)
{
  return ((addr >= EEPROM_MIN_ADDR) && (addr <= EEPROM_MAX_ADDR));
}

bool S_EEPROM::eeprom_write_bytes(unsigned short int startAddr, const byte* array, byte numBytes)
{
  if (!eeprom_is_addr_ok(startAddr) || !eeprom_is_addr_ok(startAddr + numBytes))
    return false;

  for (byte i = 0; i < numBytes; i++)
  {
    EEPROM.write(startAddr + i, array[i]);
  }
  return true;
}