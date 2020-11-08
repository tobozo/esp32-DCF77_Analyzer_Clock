#ifndef DS1307RTC_h
#define DS1307RTC_h

#include <Wire.h>
#include <Time.h>
#include <TimeLib.h>

/*
 * DS1307RTC.h - library for DS1307 RTC, the shittiest RTC library ever

  Copyright (c) Michael Margolis 2009
  This library is intended to be uses with Arduino Time.h library functions

  The library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  30 Dec 2009 - Initial release
  5 Sep 2011 updated for Arduino 1.0
  15 Aug 2018 removed constructor and implemented begin()
  7 Nov 2020 moved cpp code to header file after compilation hell
 */

class DS1307_RTC {
  public:
    void begin( uint8_t sdaPin, uint8_t sclPin );
    uint8_t isrunning( void );
    time_t get();
    void set( time_t t );
    bool read( tmElements_t &tm );
    bool write( const tmElements_t& dt );
    bool chipPresent() { return exists; }
  private:
    bool exists = false;
    uint8_t dec2bcd( uint8_t num );
    uint8_t bcd2dec( uint8_t num );
};


#define DS1307_CTRL_ID 0x68

void DS1307_RTC::begin( uint8_t sdaPin, uint8_t sclPin )
{
  if( sdaPin==0 || sclPin==0) {
    Wire.begin();
    log_d( "RTC begin SDA:%d, SCL:%d", sdaPin, sclPin );
  } else {
    Wire.begin( sdaPin, sclPin );
    log_d( "RTC begin SDA:%d, SCL:%d", sdaPin, sclPin );
  }
}


uint8_t DS1307_RTC::isrunning( void )
{
  Wire.beginTransmission( DS1307_CTRL_ID );
  Wire.write( (int)0 );
  Wire.endTransmission();
  Wire.requestFrom( DS1307_CTRL_ID, 1 );
  uint8_t ss = Wire.read();
  return !( ss>>7 );
}

// PUBLIC FUNCTIONS
time_t DS1307_RTC::get()   // Aquire data from buffer and convert to time_t
{
  tmElements_t tm;
  if ( read(tm) == false ) {
    log_w( "RTC.get is sending back empty time!!!" );
    return 0;
  }
  return( makeTime(tm) );
}

void DS1307_RTC::set( time_t t )
{
  tmElements_t tm;
  breakTime( t, tm );
  tm.Second |= 0x80;  // stop the clock
  write( tm );
  tm.Second &= 0x7f;  // start the clock
  write( tm );
}

// Aquire data from the RTC chip in BCD format
bool DS1307_RTC::read( tmElements_t &tm )
{
  uint8_t sec;
  Wire.beginTransmission( DS1307_CTRL_ID );
  Wire.write( (int)0 );
  if ( Wire.endTransmission() != 0 ) {
    log_e( "unexpected end of transmission" );
    exists = false;
    return false;
  }
  exists = true;
  Wire.requestFrom( DS1307_CTRL_ID, 7 );
  sec = Wire.read();
  tm.Second = bcd2dec( sec & 0x7f );
  tm.Minute = bcd2dec( Wire.read() );
  tm.Hour   = bcd2dec( Wire.read() & 0x3f );  // mask assumes 24hr clock
  tm.Wday   = bcd2dec( Wire.read() );
  tm.Day    = bcd2dec( Wire.read() );
  tm.Month  = bcd2dec( Wire.read() );
  tm.Year   = y2kYearToTm( bcd2dec( Wire.read() ) ); // 2000 to 1970 offset
  if ( sec & 0x80 ) {
    log_e( "clock is halted" );
    return false;
  }
  return true;
}


bool DS1307_RTC::write( const tmElements_t& dt )
{
  //dumpTime("RTC Will adjust from tmElements_t ", dt );
  Wire.beginTransmission( DS1307_CTRL_ID );
  Wire.write( (int)0 );
  Wire.write( dec2bcd(dt.Second) );
  Wire.write( dec2bcd(dt.Minute) );
  Wire.write( dec2bcd(dt.Hour) );
  Wire.write( dec2bcd(0) );
  Wire.write( dec2bcd(dt.Day) );
  Wire.write( dec2bcd(dt.Month) );
  Wire.write( dec2bcd(tmYearToY2k(dt.Year)) ); // 2000 to 1970 offset
  Wire.write( (int)0 );
  if ( Wire.endTransmission() != 0 ) {
    log_e( "unexpected end of transmission" );
    exists = false;
    return false;
  }
  exists = true;
  return true;
}

// PRIVATE FUNCTIONS

// Convert Decimal to Binary Coded Decimal (BCD)
uint8_t DS1307_RTC::dec2bcd( uint8_t num )
{
  return ( (num/10 * 16) + (num % 10) );
}

// Convert Binary Coded Decimal (BCD) to Decimal
uint8_t DS1307_RTC::bcd2dec(uint8_t num)
{
  return ( (num/16 * 10) + (num % 16) );
}

#endif
