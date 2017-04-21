#include <avr/wdt.h>
#define WATCHDOG_OFF    (0)
#define WATCHDOG_16MS   (_BV(WDE))
#define WATCHDOG_32MS   (_BV(WDP0) | _BV(WDE))
#define WATCHDOG_64MS   (_BV(WDP1) | _BV(WDE))
#define WATCHDOG_125MS  (_BV(WDP1) | _BV(WDP0) | _BV(WDE))
#define WATCHDOG_250MS  (_BV(WDP2) | _BV(WDE))
#define WATCHDOG_500MS  (_BV(WDP2) | _BV(WDP0) | _BV(WDE))
#define WATCHDOG_1S     (_BV(WDP2) | _BV(WDP1) | _BV(WDE))
#define WATCHDOG_2S     (_BV(WDP2) | _BV(WDP1) | _BV(WDP0) | _BV(WDE))
#ifndef __AVR_ATmega8__
#define WATCHDOG_4S     (_BV(WDP3) | _BV(WDE))
#define WATCHDOG_8S     (_BV(WDP3) | _BV(WDP0) | _BV(WDE))
#endif


void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3")));

// Function Implementation
void wdt_init(void)
{
    MCUSR = 0;
    wdt_disable();

    return;
}

void setup()
{
  Serial.begin(19200);
}


void watchdogReset() {
  __asm__ __volatile__ (
    "wdr\n"
  );
}

void watchdogConfig(uint8_t x) {
  WDTCSR = _BV(WDCE) | _BV(WDE);
  WDTCSR = x;
}

void loop()
{
  delay(500);
  Serial.println(millis());

  if(Serial.available())
  {
    String str=Serial.readStringUntil('\n');
    if(str=="WDT\r")
    {
      noInterrupts();
      do                          
      {
        noInterrupts();
          wdt_enable(WDTO_15MS);  
          for(;;)                 
          {                       
          }                       
      } while(0);

    }
    else
    {
    if(str=="WDTE\r")
    { 
      noInterrupts();
      watchdogReset();
      watchdogConfig(WATCHDOG_16MS);
          for(;;)
          {                       
          }                       
    }
      
    }
  }
}
