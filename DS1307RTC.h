#ifndef DS1307RTC_h
#define DS1307RTC_h

#include "Config.h"
#include <Time.h>

#ifndef DS1307_CTRL_ID
#define DS1307_CTRL_ID 0x68
#endif

// library interface description
class DS1307RTC {
  // user-accessible "public" interface
  public:
    DS1307RTC();
    void begin( uint8_t sdaPin, uint8_t sclPin );
    uint8_t isrunning( void );
    static time_t get();
    static bool set( time_t t );
    static bool read( tmElements_t &tm );
    static bool write( const tmElements_t& dt );
    static bool chipPresent() { return exists; }

  private:
    static bool exists;
    static uint8_t dec2bcd( uint8_t num );
    static uint8_t bcd2dec( uint8_t num );
};

#endif 
