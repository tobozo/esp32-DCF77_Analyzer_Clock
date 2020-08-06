#ifndef _DCD77_H_
#define _DCD77_H_

#include "Config.h"
#ifdef DCF77_DO_WEATHER
  #include "DCF77_Weather.h" // Weather data support
#endif

// !! Pins 22 through 53 are only to be used for LED's
#define LED_SUNDAY         22 // output - LED - Sunday
#define LED_MONDAY         23 // output - LED - Monday
#define LED_TUESDAY        24 // output - LED - Tuesday
#define LED_WEDNESDAY      25 // output - LED - Wednesday
#define LED_THURSDAY       26 // output - LED - Thursday
#define LED_FRIDAY         27 // output - LED - Friday
#define LED_SATURDAY       28 // output - LED - Saturday
#define LED_CEST           29 // output - LED - Summertime CEST
#define LED_CET            30 // output - LED - Wintertime CET
#define LED_LEAPYEAR       31 // output - LED - Leap year
#define LED_RTCERROR       32 // output - LED - problem reading RTC data (empty battery/connection)
#define LED_RTCSYNC        33 // output - LED - On when RTC is succesfully synced with the DCF time
#define LED_TEMP           34 // output - LED - temperature is displayed
#define LED_OPTION1        35 // output - LED - optional 1 data is displayed
#define LED_OPTION2        36 // output - LED - optional 2 data is displayed
#define LED_ERRORPT        37 // output - LED - DCF Period Time error
#define LED_ERRORPW        38 // output - LED - DCF Period Width error
#define LED_BUFFERFULL     39 // output - LED - Buffer full indicator, next the data will be analized
#define LED_MINUTEMARKER   40 // output - LED - End of DCF data stream detected before buffer is filled, data is corrupt
#define LED_BUFFEROVERFLOW 41 // output - LED - More data received in one minute than expected due to bad signal
#define LED_DCFSTATUS      42 // output - LED - On when we have good DCF data
#define LED_POWERSAVE      43 // output - LED - Power save mode is activated, some displays are off
#define LED_PARITY1PASS    44 // output - LED - Parity 1 bit is OK
#define LED_PARITY1FAIL    45 // output - LED - Parity 1 bit FAILED
#define LED_PARITY2PASS    46 // output - LED - Parity 2 bit is OK
#define LED_PARITY2FAIL    47 // output - LED - Parity 2 bit FAILED
#define LED_PARITY3PASS    48 // output - LED - Parity 3 bit is OK
#define LED_PARITY3FAIL    49 // output - LED - Parity 3 bit FAILED
#define LED_PIRMOTION      50 // output - LED - On when PIR is detecting motion
#define LED_WEEKNUMBER     51 // output - dual digit

//----------------------------------------------------------------------------------------------------------
// User settings, variable and array definitions
//----------------------------------------------------------------------------------------------------------

// Choose if you want a test of all LED's and Displays after a startup
// '1' = Yes, '0' = No
#define PERFORM_LED_TEST 1
// Delay between each 7 segment display in ms
#define LedTest_DELAY_DISPLAYS 300
// Delay between each LED in the LED ring and other LED's in ms
#define LedTest_DELAY_LED_RING 10

// define power saving display OFF and ON time
// values are in 'Hour' format
// ONLY the displays are shut off at power saving time, the clock remains active.
// To disable this feature, make both values zero
//#define POWERSAVINGOFFTIME 0  // displays are activated
//#define POWERSAVINGONTIME  0 // displays are shutt off

// User option to reset temperature min/max memory at midnight
// '1' = Reset at midnight, '0' = Only manual reset
//#define TEMPRESET_MIDNIGHT 1

// definition of Maxim 7219 display number wiring sequence
// first Maxim 7219 in wiring 'daisychain' must be '0', next '1' etc.
// COMMON CATHODE DISPLAYS
#define LedRingInner          0
#define LedRingOuter          1
#define DisplayBufferBitError 2
#define DisplayPeriodPulse    3


// beed tones
static uint16_t willBuzzNote;
static int willBuzzDuration;

// Pulse flanks
//static unsigned long flankTime    = 0;
static unsigned long leadingEdge  = 0;
static unsigned long trailingEdge = 0;
unsigned long previousLeadingEdge = 0;

// used in <Int0handler>
volatile unsigned int DCFSignalState = 0; // interrupt variables ALWAYS need volatile qualifier!!

// used in <loop>
static int previousSecond      = 0;
static int previousSignalState = 0;

// DCF Buffers and indicators
static int DCFbitBuffer[59]; // here, the received DCFbits are stored
static int DCFbitFinalBuffer[59]; // here, the received DCFbits are stored
const int bitValue[] = {1, 2, 4, 8, 10, 20, 40, 80}; // these are the decimal values of the received DCFbits

// only after start on a new minute, display received bits on inner LED ring
boolean MinuteMarkerFlag = false;
static int bufferPosition       = 0;
static int lastBufferPosition   = 0;
static int previousMinute       = 0;
static int previousHour         = 0;

// variables to check if DCF bits are vald
static bool dcfValidSignal  = false;
static bool lastDcfValidSignal  = false;

int dcfP1counter     = 0;
int dcfP2counter     = 0;
int dcfP3counter     = 0;
int dcfParityCheckP1 = 0;
int dcfParityCheckP2 = 0;
int dcfParityCheckP3 = 0;

// dcf variables to store decoded DCF time in
int dcfMinute  = 0;
int dcfHour    = 0;
int dcfDay     = 0;
int dcfWeekDay = 0;
int dcfMonth   = 0;
int dcfYear    = 0;
int dcfDST     = 0;
int leapYear   = 0;

// variables used to store weeknumber and daynumer values
int dayNumber;
int weekNumber;

// error counter variable
int errorCounter        = 0;
boolean errorCondition  = false;

// miscelleanous variables
boolean daytimeChange   = true;
boolean dayTime         = false;
int dcf77SoundSwitch    = 0;

// ringLeds coords caching
typedef struct {
  float x;
  float y;
} coords;
// fake led array
coords ringLedCoords[4][60];

// will be loaded from UI.h
extern void LedDisplay( int addr, String leftOrRight, int value );
extern void error( int errorLed );
extern void scheduleBuzz( uint16_t note, int duration );
extern void LedErrorStatus( uint8_t lednum, int status );
extern void displayBufferPosition( int dcfBit );
extern void setRingLed( uint8_t ringNum, uint8_t ledNum, bool enable, bool clear=true );
extern void LedParityStatus( uint8_t paritynum, int status );
extern void LedDCFStatus( int status );
extern void clearRing( uint8_t ringNum );


void copyBuffer();
void decodeBufferContents( void );
void finalizeBufferTask( void *param=NULL );
void processDcfBit( int dcfBit );
static int bitDecode( int* buffer, int bitStart, int bitEnd );
int calculateLeapYear( int year );
int dayWeekNumber( int y, int m, int d, int w );


//================================================================================================================
//
// Function name : processDcfBit
// called from   : <scanSignal>
//
// Purpose       : Evaluates the signal as it is received. Decides whether we received a "1" or a "0"
//                 and perform checks to see if the pulse timing is within limits
// Parameters    : none
// Return value  : none
//
//================================================================================================================
/*
       pulse                 pulse
       width                 width
       |- -|               |--   --|           |----- END OF MINUTE marker:2000ms -----|
        ___                 _______             ___                                     ___                 _______
       | 0 |               |   1   |           | 0 |                                   | 0 |               |   1   |
       |   |               |       |           |   |                                   |   |               |       |
       |   |               |       |           |   |                                   |   |               |       |
 ______|   |_______________|       |___________|   |___________________________________|   |_______________|       |__ _ _ _
       ^   ^               ^       ^           ^   ^               ^                   ^   ^               ^       ^
       1000 2100           2000    2200        3000 3100         NO PULSE              5000 5100           6000    6200         << example millis() value
                                                                 = end of Minute indication
       ^                   ^                   ^                                       ^                   ^
       DCFbit# 56          DCFbit# 57          DCFbit# 58                               DCFbit# 0           DCFbit# 1  etc...   << DCF bit received

       ^                   ^        ^
       previous            leading  trailing
       leading edge        edge     edge

       ^   ^
       flanktime (rising or falling)

*/

void scanSignal() {
  //--------------------------------------------------------------------
  // Check for Rising-Edge signal and perform checks
  //--------------------------------------------------------------------
  if ( DCFSignalState == 1 ) {
    // store Rising-Edge Time to check later if the time between two pulses is valid
    leadingEdge = millis();
    // not much to do now so exit.
    return;
  }
  //--------------------------------------------------------------------
  // Check for Falling-Edge signal and perform checks
  //--------------------------------------------------------------------
  if ( DCFSignalState == 0 ) {
    // store Trailing-Edge Time to check later if the Pulse Width is valid
    trailingEdge = millis();
    // display period width time on "L"eft side of the 8 digit Maxim 72xx LED display
    LedDisplay( DisplayPeriodPulse, "L", (leadingEdge - previousLeadingEdge) );
    // display pulse width time on the "R"ight side of the 8 digit Maxim 72xx LED display
    LedDisplay( DisplayPeriodPulse, "R", (trailingEdge - leadingEdge) );
    //--------------------------------------------------------------------------------
    // Check PERIOD TIME
    //--------------------------------------------------------------------------------
    // If this flank UP is detected quickly after previous flank UP this is an incorrect
    // Period Time (should be 1000ms -or 2000ms after second 58-) that we shall reject
    if ( (leadingEdge - previousLeadingEdge) < 900 ) {
      // rPW - ERROR: Periode Time (rising flank to rising flank) time is too short -> REJECTED
      error( LED_ERRORPW );
      errorCondition = true;
    }
    //--------------------------------------------------------------------------------
    // CHECK PULSE TIME
    //--------------------------------------------------------------------------------
    // If the detected pulse is too short it will be an incorrect pulse that we shall reject
    // should be 100 and 200 ms ideally
    if ( ( ( trailingEdge - leadingEdge ) < 70 ) || ( ( trailingEdge - leadingEdge ) > 230 ) ) {
      //rPT - ERROR: Pulse Width too short or too long -> REJECTED
      error( LED_ERRORPT );
      errorCondition = true;
    }
    // if we had an error return and start over
    if ( errorCondition == true ) {
      errorCondition = false;
      // although we have an error, store current rising edge time to compare at the next Rising-Edge.
      previousLeadingEdge = leadingEdge;
      #ifdef SPEAKER_PIN
      if ( dcf77SoundSwitch == 1) scheduleBuzz( BEEPNOTE_ERROR, 60 );
      #endif
      return;
    }
    //--------------------------------------------------------------------
    // no errors found so now we can continue
    //--------------------------------------------------------------------
    // first we turn any error Led's OFF
    LedErrorStatus( LED_ERRORPW, LOW );
    LedErrorStatus( LED_ERRORPT, LOW );
    LedErrorStatus( LED_BUFFERFULL, LOW );      // previous BF
    LedErrorStatus( LED_BUFFEROVERFLOW, LOW );  // previous EoB
    LedErrorStatus( LED_MINUTEMARKER, LOW );    // previous EoM
    // END OF MINUTE check, looking for a gap of approx. 2000ms
    if ( leadingEdge - previousLeadingEdge > 1900 && leadingEdge - previousLeadingEdge < 2100 ) {
      // end of minute detected:
      copyBuffer();
      // Reset DCFbitBuffer array, positions 0-58 (=59 bits)
      for ( int i = 0; i < 59; i++ ) {
        // Reset DCFbitBuffer array, positions 0-58 (=59 bits)
        DCFbitBuffer[i] = 0;
      }
      bufferPosition   = 0;
      xTaskCreatePinnedToCore( finalizeBufferTask,   "finalizeBufferTask",   4096, NULL, 8, NULL, 0 );
    }
    // refresh previousLeadingEdge time with the new leading edge time
    previousLeadingEdge = leadingEdge;
    //--------------------------------------------------------------------------------
    // process DCF bits
    //--------------------------------------------------------------------------------
    // distinguish between long and short pulses
    if ( trailingEdge - leadingEdge < 170 ) { // call processDcfBit function and sent it the value '0'
      processDcfBit( 0 );
      // if switch is HIGH, the DCF pulses are audible
      #ifdef SPEAKER_PIN
        if ( dcf77SoundSwitch == 1) scheduleBuzz( BEEPNOTE_BIT_HIGH, 20 );
      #endif
    } else { // call processDcfBit function and sent it the value '1'
      processDcfBit( 1 );
      // if switch is HIGH, the DCF pulses are audible
      #ifdef SPEAKER_PIN
        if ( dcf77SoundSwitch == 1) scheduleBuzz( BEEPNOTE_BIT_LOW, 20 );
      #endif
    }
  }
}


//================================================================================================================
//
// Function name : processDcfBit
// called from   : <scanSignal>
//
// Purpose       : after reception of one good DCF bit, do some checks and save it in the DCFbitBuffer array
// Parameters    : none
// Return value  : none
//
//================================================================================================================

void processDcfBit( int dcfBit ) {
  // display bufferPosition and current bit value
  displayBufferPosition( dcfBit );

  //--------------------------------------------------------------------
  // display incoming DCF bits on inner LED ring
  //--------------------------------------------------------------------
  // only if we have valid DCF data or after an Minute Mark (EoM) signal
  // activate the inner LED ring and diplay incoming data
  if ( dcfValidSignal == true || MinuteMarkerFlag == true ) {
    // display received bits on inner LED ring
    setRingLed( LedRingInner, bufferPosition, dcfBit, false );
  }

  //--------------------------------------------------------------------
  //   // Fill DCFbitBuffer array with DCFbit
  //--------------------------------------------------------------------
  DCFbitBuffer[bufferPosition] = dcfBit;

  //--------------------------------------------------------------------
  // Parity check
  //--------------------------------------------------------------------
  // DURING reception of the DCF bits, calculate and display the results of the DCF parity check.
  //
  // There is a Parity bit for the minutes, the hours and for the date.
  // DCF77 works with EVEN parity, this works as follows:
  // The hours for example have 6 bits plus a paritybit. The bits with value 1 are add up including the paritybit,
  // the result must be an even number. If there is a bit wrong received, a 0 is as 1, or a 1 is as 0 received,
  // then the result is uneven.  source: http://www.picbasic.nl/frameload_uk.htm?http://www.picbasic.nl/info_dcf77_uk.htm

  if ( bufferPosition == 0 ) {
    // reset the parity LED's
    LedParityStatus( 1, -1 );
    LedParityStatus( 2, -1 );
    LedParityStatus( 3, -1 );
    // reset variables
    dcfP1counter = 0;
    dcfP2counter = 0;
    dcfP3counter = 0;
    dcfParityCheckP1 = 0;
    dcfParityCheckP2 = 0;
    dcfParityCheckP3 = 0;
  }

  // ----------------------------------------
  // First parity check: minute bits
  // ----------------------------------------
  if ( bufferPosition == 28 ) {
    for ( int i = 21; i <= 27; i++ ) {
      // count the number of bits with the value '1'
      dcfP1counter += DCFbitBuffer[i];
    }
    // perform P1 parity check. Parity is OK if the sum is an EVEN value
    if ( (DCFbitBuffer[28] + dcfP1counter) % 2 == 0 ) {
      // Parity1 PASS LED ON
      LedParityStatus( 1, 0 );
      // Parity P1 PASS
      dcfParityCheckP1 = 1;
    } else {
      // Parity1 FAIL LED ON
      LedParityStatus( 1, 1 );
      // we have no valid data!
      dcfValidSignal = false;
      // Turn DCF OK LED OFF
      LedDCFStatus( false );
      log_e( "Parity fail on minute bits" );
    }
  }

  // ----------------------------------------
  // Second parity check: hour bits
  // ----------------------------------------
  if ( bufferPosition == 35 ) {
    for ( int i = 29; i <= 34; i++ ) {
      dcfP2counter += DCFbitBuffer[i];
    }
    // perform P2 parity check. Parity is OK if the sum is an EVEN value
    if ( ( DCFbitBuffer[35] + dcfP2counter ) % 2 == 0 ) {
      // Parity2 PASS LED ON
      LedParityStatus( 2, 0 );
      // Parity P2 PASS
      dcfParityCheckP2 = 1;
    } else {
      // Parity2 FAIL LED ON
      LedParityStatus( 2, 1 );
      // we have no valid data!
      dcfValidSignal = false;
      // Turn DCF OK LED OFF
      LedDCFStatus( false );
      log_e( "Parity fail on second bits" );
    }
  }

  // ----------------------------------------
  // Third parity check: date bits
  // ----------------------------------------
  if ( bufferPosition == 58 ) {
    for ( int i = 36; i <= 57; i++ ) {
      dcfP3counter += DCFbitBuffer[i];
    }
    // perform P3 parity check. Parity is OK if the sum is an EVEN value
    ( DCFbitBuffer[58] + dcfP3counter ) % 2 == 0 ? dcfParityCheckP3 = 1 : dcfParityCheckP3 = 0;
    // Turn Parity2 'PASS' or 'FAIL' LED ON
    if ( dcfParityCheckP3 == 1 ) {
      // Parity2 PASS LED ON
      LedParityStatus( 3, 0 );
      // Parity P3 PASS
      dcfParityCheckP3 = 1;
    } else {
      // Parity2 FAIL LED ON
      LedParityStatus( 3, 1 );
      // we have no valid data!
      dcfValidSignal = false;
      // Turn DCF OK LED OFF
      LedDCFStatus( false );
      log_e( "Parity fail on date bits" );
    }
    // ----------------------------------------
    // finally, check all Parity bits
    // ----------------------------------------
    dcfParityCheckP1 + dcfParityCheckP2 + dcfParityCheckP3 == 3 ? dcfValidSignal = true : dcfValidSignal = false;
  }

  //--------------------------------------------------------------------
  // before continuing with the next bit, increment counter
  //--------------------------------------------------------------------
  bufferPosition++;

  //--------------------------------------------------------------------
  // check if we have not received too many pulses?
  //--------------------------------------------------------------------
  if ( bufferPosition > 59 ) {
    // Buffer Overflow ERROR - we have received more pulses before reaching
    // the 2 second 'gap' signalling the end of the minute.
    //This error may be due to a noisy signal giving addition peaks/dcfBits
    // So clear both DCFbit displays and start again.
    // Reset buffer counter
    bufferPosition = 0;
    // clear inner LED ring
    clearRing( LedRingInner );
    // turn Buffer Overflow Error LED ON
    error( LED_BUFFEROVERFLOW );
    return;
  }
  //--------------------------------------------------------------------
  // everything OK so we wait for next incoming DCFbit
  //--------------------------------------------------------------------
}

//================================================================================================================
//
// Function name : finalizeBuffer
// called from   : <scanSignal>
//
// Purpose       : Process the succesfully received DCF data of one minute
// Parameters    : none
// Return value  : none
//
//================================================================================================================


static bool finalizing = false;

void finalizeBuffer( void ) {
  if( finalizing ) {
    return;
  }
  finalizing = true;
  __attribute__((unused)) unsigned long start = millis();
  //--------------------------------------------------------------------
  // We are here because of the detected 2 second 'gap'.
  // Now check if it correspondends with the buffer counter
  // 'bufferPosition' which should be value 59
  //--------------------------------------------------------------------
  if ( lastBufferPosition == 59 && lastDcfValidSignal == true ) {
    // process buffer and extract data sync the time with the RTC
    decodeBufferContents();
    // set Arduino time and after that set RTC time
    setTime( dcfHour, dcfMinute, 0, dcfDay, dcfMonth, dcfYear );
    #ifdef USE_RTC
      takeMuxSemaphore();
      RTC.set(now());
      giveMuxSemaphore();
      log_w( "RTC and internal clock adjusted to 20%02d:%02d:%02d %02d:%02d:00", dcfYear, dcfMonth, dcfDay, dcfHour, dcfMinute );
    #endif
    // bufferPosition == 59 so turn Buffer Full LED ON
    LedErrorStatus( LED_BUFFERFULL, HIGH );
    // Turn DCF OK LED ON
    LedDCFStatus( true );
    // copy 'contents' of inner LED ring to the outer LED ring (current time information)
    for ( uint8_t r = 0; r < 59; r++ ) {
      setRingLed( LedRingOuter, r, DCFbitFinalBuffer[r] == 1, false );
      // Reset inner LED ring (incoming time information)
      setRingLed( LedRingInner, r, false );
      // Reset array, positions 0-58 (=59 bits)
      //DCFbitBuffer[r] = 0;
    }
    // activate Synced LED
    LedErrorStatus( LED_RTCSYNC, HIGH );
    // Reset running buffer
    //bufferPosition   = 0;
    // reset flag
    MinuteMarkerFlag = false;
  } else { // if (bufferPosition == 59)
    //--------------------------------------------------------------------
    // The buffer is not yet filled although the 2 second 'gap' was detected.
    // Can be result of a noisy signal, starting in middle of receiving data etc.
    // Turn 'Minute Mark' LED ON
    //--------------------------------------------------------------------
    LedErrorStatus( LED_MINUTEMARKER, HIGH );
    log_w( "Minute Mark" );
    // Clear displays
    for (int i = 0; i < 59; i++) {
      // Reset inner LED ring (incoming time information)
      setRingLed( LedRingInner, i, false );
    }
    // set flag so we can display incoming pulsed on the inner LED ring.
    MinuteMarkerFlag = true;
  }
  //unsigned long duration = millis() - start;
  log_i( " FinalizeBuffer() duration %d ms", (int)(millis() - start) );
  finalizing = false;
}



void copyBuffer() {
  lastBufferPosition = bufferPosition;
  lastDcfValidSignal = dcfValidSignal;
  for( uint8_t i=0;i<59;i++ ) {
    DCFbitFinalBuffer[i] = DCFbitBuffer[i]; // here, the received DCFbits are stored copyBuffer()
  }
}



//================================================================================================================
//
// Function name : decodeBufferContents
// called from   : <finalizeBuffer>
//
// Purpose       : Evaluates the information stored in the buffer.
//                 This is where the DCF77 signal is decoded to time and date information
// Parameters    : none
// Return value  : none
//
//================================================================================================================

void decodeBufferContents( void ) {
  // Buffer is full and ready to be decoded
  dcfMinute  = bitDecode( DCFbitFinalBuffer, 21, 27 );
  dcfHour    = bitDecode( DCFbitFinalBuffer, 29, 34 );
  dcfDay     = bitDecode( DCFbitFinalBuffer, 36, 41 );
  dcfWeekDay = bitDecode( DCFbitFinalBuffer, 42, 44 );
  dcfMonth   = bitDecode( DCFbitFinalBuffer, 45, 49 );
  dcfYear    = bitDecode( DCFbitFinalBuffer, 50, 57 );
  //call function to calculate day of year and weeknumber
  dayWeekNumber( dcfYear, dcfMonth, dcfDay, dcfWeekDay );
  // Get value of Summertime DCFbit. '1' = Summertime, '0' = wintertime
  dcfDST     = bitDecode( DCFbitFinalBuffer, 17, 17 );
  // determine Leap Year
  leapYear   = calculateLeapYear( dcfYear );
  #ifdef DCF77_DO_WEATHER
    // store weather info (needs three records in a row to decode)
    if ( addToWeatherInfo( DCFbitFinalBuffer ) ) {
      mWeatherArea = getArea();
      mWeatherSection = getSection();
      uint8_t aInfo[3];
      if (GetWeatherInfo( aInfo ) ) { // Decrypt+decompress
        //log_d( "Ciphered Weather Data: %s", mDcf.weatherData );
        meteodata = decToBinStr( aInfo );
        //showWeather();
        if( AllCitiesForecast != nullptr ) {
          int forecastMinute, forecastHour;
          if( dcfMinute == 0 ) {
            if( dcfHour == 0 ) {
              forecastHour = 23;
            } else {
              forecastHour = dcfHour -1;
            }
            forecastMinute = 59;
          } else {
            forecastHour = dcfHour;
            forecastMinute = dcfMinute-1;
          }
          int forecastID = (((forecastHour) * 60) + (forecastMinute))  / 3; // zero indexed
          setForecast( forecastID, forecastHour, forecastMinute, aInfo );
        }
        weatherReady = true;
      } else {
        log_w("weather data couldn't be decrypted/uncompressed");
      }
    } else {
      log_w("weather data not complete yet");
    }
  #endif
}

//================================================================================================================
//
// bitDecode
//
// called from <processBuffer>
//================================================================================================================
static int bitDecode( int* buffer, int bitStart, int bitEnd )  {
  // reset 'bitValue-array' counter
  int i = 0;
  int value = 0;
  // process bitrange bitStart > bitEnd
  while ( bitStart <= bitEnd ) {
    // check if DCFbit in buffer is '1', discard when '0'
    if ( buffer[bitStart] == 1 ) {
      // DCFbit in buffer == 1 so append its corresponding value to the variable 'value'
      value = value + bitValue[i];
    }
    // increment 'bitValue-array' counter
    i++;
    // increment bit-range counter
    bitStart++;
  }
  return value;
}




//================================================================================================================
//
// Function name : dayWeekNumber
// called from   : <decodeBufferContents>
//
// Purpose       : calculate the WEEK number according to ISO standard, see comments in the ARCHIVE below
// Parameters    : dcfYear, dcfMonth, dcfDay, dcfWeekDay
// Return value  : weekNumber
//
//================================================================================================================
//Code from: http://forum.arduino.cc/index.php/topic,44476.0.html

int dayWeekNumber( int y, int m, int d, int w ) {
  // Number of days at the beginning of the month in a normal (not leap) year.
  int days[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
  // Start to calculate the number of days of the first two months
  if ( m == 1 || m == 2 ) {
    // for any type of year we calculate the number of days for January or february
    dayNumber = days[(m - 1)] + d;
  } else if ( (y % 4 == 0 && y % 100 != 0) ||  y % 400 == 0 ) {
    // now calculate for the other months
    // first, check for a leap year
     // we have a leap year, so calculate in the same way but adding one day
    dayNumber = days[(m - 1)] + d + 1;
  } else {
    //no leap year, calculate in the normal way, such as January or February
    dayNumber = days[(m - 1)] + d;
  }
  // Now start to calculate Week number
  if (w == 0) {
    //if it is sunday (time library returns 0)
    weekNumber = (dayNumber - 7 + 10) / 7;
  } else {
    // for the other days of week
    weekNumber = (dayNumber - w + 10) / 7;
  }
  // finished! return with the week number as an INT value
  return weekNumber;
}



//================================================================================================================
//
// Function name : calculateLeapYear
// called from   : <decodeBufferContents>
//
// Purpose       : determine if a given year is a leap year
// Parameters    : year - the year to test
// Return value  : '1' if the year is a leap year, '0' otherwise
//
//================================================================================================================

int calculateLeapYear( int year ) {
  if ( (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0) ) {
    return 1;
  } else {
    return 0;
  }
}


#endif //  _DCD77_H_
