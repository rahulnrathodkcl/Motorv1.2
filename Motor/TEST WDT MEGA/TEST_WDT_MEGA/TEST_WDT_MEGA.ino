#include <avr/wdt.h>


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

void loop()
{
  delay(500);
  Serial.println(millis());

  if(Serial.available())
  {
    String str=Serial.readStringUntil('\n');
    if(str=="RESET\r")
    {
      do                          
      {
          wdt_enable(WDTO_15MS);  
          for(;;)                 
          {                       
          }                       
      } while(0);

    }
  }
}
