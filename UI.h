#ifndef _UI_H_
#define _UI_H_

#include "Config.h"
#ifdef USE_RTC
#include "DS1307RTC.h" // a custom DS1307 library that returns time as a time_t
DS1307RTC RTC;
#else
#include <Time.h>
#endif

#define freeheap heap_caps_get_free_size( MALLOC_CAP_INTERNAL )

static xSemaphoreHandle mux = NULL; // this is needed to prevent rendering collisions
// between scrollpanel and heap graph
#define takeMuxSemaphore() if( mux ) { xSemaphoreTake( mux, portMAX_DELAY ); log_v( "Took Semaphore" ); }
#define giveMuxSemaphore() if( mux ) { xSemaphoreGive( mux ); log_v( "Gave Semaphore" ); }

#ifdef SPEAKER_PIN
SPEAKER Speaker;
#endif
#ifdef BUTTON_A_PIN
Button BtnA = Button( BUTTON_A_PIN, true, DEBOUNCE_MS );
#endif
#ifdef BUTTON_B_PIN
Button BtnB = Button( BUTTON_B_PIN, true, DEBOUNCE_MS );
#endif
#ifdef BUTTON_C_PIN
Button BtnC = Button( BUTTON_C_PIN, true, DEBOUNCE_MS );
#endif

#if defined( ARDUINO_M5Stack_Core_ESP32 ) || defined( ARDUINO_M5STACK_FIRE )
#include "UI_320x240.h"
#else
#include "UI_160x128.h"
#endif
#include <JPEGDecoder.h>
#include "DCF77.h"

TFT_eSprite sprite = TFT_eSprite( &M5.Lcd );

#ifdef DCF77_DO_WEATHER
TFT_eSprite ScrollSprite = TFT_eSprite( &M5.Lcd );
#endif
TFT_eSprite MainSprite = TFT_eSprite( &M5.Lcd );
TFT_eSprite MaskSprite = TFT_eSprite( &M5.Lcd );
TFT_eSprite LogoSprite = TFT_eSprite( &M5.Lcd );

static uint16_t TFT_HALFWIDTH;//    M5.Lcd.width()/2
static uint16_t TFT_HALFHEIGHT;//   M5.Lcd.height()/2
static uint16_t RingLabelsFontHeight = 0;
static uint16_t RingLabelsFontWidth  = 0;
static uint16_t ErrorsCountFontHeight = 0;
static uint16_t ErrorsCountFontWidth  = 0;
static uint16_t tmpFontHeight = 0;
static uint16_t tmpFontWidth  = 0;
static uint16_t LedWeekStatusFontWidth = 0;
static uint16_t LedWeekStatusFontHeight = 0;
static uint16_t LedErrorStatusFontWidth = 0;
static uint16_t LedErrorStatusFontHeight = 0;
static uint16_t LedParityStatusFontWidth = 0;
static uint16_t LedParityStatusFontHeight = 0;
static uint16_t LedDCFStatusFontWidth = 0;
static uint16_t LedDCFStatusFontHeight = 0;

static uint16_t weekDayNamesWidth = 0;
static uint16_t CETWidth = 0;
static uint16_t CETXPos = 0;
static uint16_t CESTWidth = 0;
static uint16_t CESTXpos = 0;
static uint16_t LEAPWidth = 0;
static uint16_t LEAPXPos = 0;

int16_t  x, y;
uint16_t w, h;

float MinuteSteps = ( TWO_PI / 60 ) / 5;

float r1 = float_r1; // inner ring
float r2 = float_r2; // outer ring
float r3 = float_r3; // inner 10's mark
float r4 = float_r4; // inner 10's label


#ifdef DCF77_DO_WEATHER
static String scrollText = "blah bleh blih bloh bluh blyh blah bleh blih bloh bluh blyh ";
static uint16_t scrollWidth;
static uint16_t scrollHeight;
static int scrollPos = 0;
static int scrollFontSize = int_scrollFontSize;
static bool isScrolling = false;
static uint16_t scrollFontColor;
static uint16_t scrollFontColorDisabled = ST7735_DARKGRAY;
static uint16_t scrollFontColorEnabled = ST7735_GRAY;
#endif


#include "Tasks.h"


void checkButtons(void) {
#ifdef BUTTON_A_PIN
  BtnA.read();
  vTaskDelay(1);
  if ( BtnA.wasPressed() ) {
    Serial.println( "Will restart" );
    ESP.restart();
    while (1) {
      ;
    }
  }
#endif
#ifdef BUTTON_B_PIN
  BtnB.read();
  vTaskDelay(1);
  if ( BtnB.wasPressed() ) { // toggle sound
    dcf77SoundSwitch = 1 - dcf77SoundSwitch;
    Serial.printf( "Toggling sound %s\n", dcf77SoundSwitch == 1 ? "On" : "Off" );
  }
#endif
#ifdef BUTTON_C_PIN
  BtnC.read();
  vTaskDelay(1);
  if ( BtnC.wasPressed() ) {
    Serial.println( "Button C was pressed but no action is assigned" );
  }
#endif
}


void tft_setFont( const GFXfont *myFont, TFT_eSprite &spr ) {
  M5.Lcd.setFreeFont( myFont );
  spr.setFreeFont( myFont );
}


void tft_setFont( uint8_t myFont, TFT_eSprite &spr ) {
  M5.Lcd.setTextFont( myFont );
  spr.setTextFont( myFont );
}


void tft_setFont( const GFXfont *myFont ) {
  tft_setFont( myFont, sprite );
}


void tft_setFont( uint8_t myFont ) {
  tft_setFont( myFont, sprite );
}


static void tft_getTextBounds( const char *string, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) {
  *w = M5.Lcd.textWidth( string );
  *h = M5.Lcd.fontHeight( M5.Lcd.textfont );
}


static void tft_getTextBounds( const __FlashStringHelper *s, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) {
  *w = M5.Lcd.textWidth( s );
  *h = M5.Lcd.fontHeight( M5.Lcd.textfont );
}


static void tft_getTextBounds( const String &str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) {
  *w = M5.Lcd.textWidth( str );
  *h = M5.Lcd.fontHeight( M5.Lcd.textfont );
}


#ifdef DCF77_DO_WEATHER
void setupScroll() {
  tft_setFont( scrollFontSize, ScrollSprite );
  tft_getTextBounds( scrollText, (int16_t)0, (int16_t)0,  &x, &y, &scrollWidth, &scrollHeight );

  takeMuxSemaphore();
  ScrollSprite.createSprite( M5.Lcd.width(), scrollHeight );
  //ScrollSprite.fillSprite( ST7735_BLACKISH );
  ScrollSprite.setTextColor( scrollFontColor, ST7735_BLACKISH );
  giveMuxSemaphore();

  takeMuxSemaphore();
  MaskSprite.createSprite( M5.Lcd.width(), scrollHeight );
  MaskSprite.fillSprite( TFT_BLACK );
  MaskSprite.setSwapBytes( true );
  giveMuxSemaphore();
}


void initScroll( String text/*="                             Meteo is loading" */ ) {
  scrollText = text;
  scrollPos = 0;
  scrollFontSize = int_scrollFontSizeSmall;
  scrollFontColor = scrollFontColorDisabled;
  takeMuxSemaphore();
  MainSprite.createSprite( M5.Lcd.width(), M5.Lcd.height() );
  MainSprite.fillSprite( TFT_BLACK );
  MainSprite.setSwapBytes( true );
  giveMuxSemaphore();
  setupScroll();
}


void updateScroll( String text ) {
  scrollText = text;
  scrollPos = 0;
  scrollFontSize = int_scrollFontSize;
  scrollFontColor = scrollFontColorEnabled;
  MaskSprite.deleteSprite();
  ScrollSprite.deleteSprite();
  setupScroll();
}


void handleScroll() {
  if ( scrollWidth <= 0 ) {
    return;
  }

  takeMuxSemaphore();
  tft_setFont( scrollFontSize, ScrollSprite );
  ScrollSprite.drawString( scrollText, scrollPos, 0 );
  ScrollSprite.drawString( scrollText, scrollWidth + scrollPos, 0 );
  giveMuxSemaphore();

  takeMuxSemaphore();
  MaskSprite.pushImage( 0, -M5.Lcd.height() / 2 + scrollHeight / 2, M5.Lcd.width(), M5.Lcd.height(), (const uint16_t*)MainSprite.frameBuffer(1) );
  //MaskSprite.pushImage( ( TFT_HALFWIDTH - 32 / 2 ), ( TFT_HALFHEIGHT - 32 / 2 ), 32, 32, (const uint16_t*)LogoSprite.frameBuffer(1) );
  MaskSprite.pushRotated( &ScrollSprite, 0, TFT_BLACK );
  giveMuxSemaphore();

  takeMuxSemaphore();
  MainSprite.pushImage( 0, M5.Lcd.height() / 2 - scrollHeight / 2, M5.Lcd.width(), scrollHeight, (const uint16_t*)ScrollSprite.frameBuffer(1) );
  giveMuxSemaphore();
  scrollPos -= scrollSpeed;
  if ( scrollPos <= -scrollWidth ) scrollPos = 0;
}
#endif


uint32_t sprite_jpegRender( TFT_eSprite &spr, int xpos, int ypos ) {
  uint16_t *pImg;
  uint16_t mcu_w = JpegDec.MCUWidth;
  uint16_t mcu_h = JpegDec.MCUHeight;
  uint32_t max_x = JpegDec.width;
  uint32_t max_y = JpegDec.height;
  // Jpeg images are draw as a set of image block (tiles) called Minimum Coding Units (MCUs)
  // Typically these MCUs are 16x16 pixel blocks
  // Determine the width and height of the right and bottom edge image blocks
  uint32_t min_w = min( mcu_w, max_x % mcu_w );
  uint32_t min_h = min( mcu_h, max_y % mcu_h );
  // save the current image block size
  uint32_t win_w = mcu_w;
  uint32_t win_h = mcu_h;
  // record the current time so we can measure how long it takes to draw an image
  uint32_t drawTime = millis();
  // save the coordinate of the right and bottom edges to assist image cropping
  // to the screen size
  max_x += xpos;
  max_y += ypos;
  // Fetch data from the file, decode and display
  while ( JpegDec.read() ) {    // While there is more data in the file
    pImg = JpegDec.pImage ;   // Decode a MCU (Minimum Coding Unit, typically a 8x8 or 16x16 pixel block)
    // Calculate coordinates of top left corner of current MCU
    int mcu_x = JpegDec.MCUx * mcu_w + xpos;
    int mcu_y = JpegDec.MCUy * mcu_h + ypos;
    // check if the image block size needs to be changed for the right edge
    if ( mcu_x + mcu_w <= max_x ) win_w = mcu_w;
    else win_w = min_w;
    // check if the image block size needs to be changed for the bottom edge
    if ( mcu_y + mcu_h <= max_y ) win_h = mcu_h;
    else win_h = min_h;
    // copy pixels into a contiguous block
    if ( win_w != mcu_w ) {
      uint16_t *cImg;
      int p = 0;
      cImg = pImg + win_w;
      for ( int h = 1; h < win_h; h++ ) {
        p += mcu_w;
        for ( int w = 0; w < win_w; w++ ) {
          *cImg = *( pImg + w + p );
          cImg++;
        }
      }
    }
    // calculate how many pixels must be drawn
    uint32_t mcu_pixels = win_w * win_h;
    // draw image MCU block only if it will fit on the screen
    if ( ( mcu_x + win_w ) <= spr.width() && ( mcu_y + win_h ) <= spr.height() )
      spr.pushImage( mcu_x, mcu_y, win_w, win_h, pImg );
    else if ( (mcu_y + win_h) >= spr.height() )
      JpegDec.abort(); // Image has run off bottom of screen so abort decoding
  }
  return millis() - drawTime;
}


void sprite_drawJpg( TFT_eSprite &spr, int16_t x, int16_t y, const uint8_t * jpg_data, size_t jpg_len, uint16_t maxWidth, uint16_t maxHeight ) {
  boolean decoded = JpegDec.decodeArray( jpg_data, jpg_len );
  if ( decoded ) {
    sprite_jpegRender( spr, x, y );
  } else {
    Serial.println( "Jpeg file format not supported!" );
  }
}


void displayBufferPosition( int dcfBit ) {
  takeMuxSemaphore();
  tft_setFont( BufferFont );
  tft_getTextBounds( "0", (int16_t)0, (int16_t)0,  &x, &y, &w, &h );
  sprite.setTextColor( ST7735_RED, ST7735_DARKGRAY );
  sprite.drawString( dcfBit ? "1" : "0", M5.Lcd.width() - w, h + 1 );

  char bufferPosStr[3];
  sprintf( bufferPosStr, "%02d", bufferPosition );
  tft_getTextBounds( "000", (int16_t)0, (int16_t)0,  &x, &y, &w, &h );
  sprite.setTextColor( ST7735_GREEN, ST7735_DARKGRAY );
  sprite.drawString( bufferPosStr, M5.Lcd.width() - (w + 1), h + 1 );
  giveMuxSemaphore();

}


void LedDisplay( int addr, String leftOrRight, int value ) {
  int shift;
  ( leftOrRight == "L" ? shift = 4 : shift = 0 );
  //Now print the number digit by digit
  //preceding zero's are removed with tenary operator by 'printing' a space character.
  uint16_t xpos = 0;
  takeMuxSemaphore();
  switch ( addr ) {
    case DisplayPeriodPulse:
      char pulseStr[10];
      switch ( shift ) {
        case 4: // Right
          tft_setFont( LedDisplayFont );
          tft_getTextBounds( "0000000", (int16_t)0, (int16_t)0,  &x, &y, &w, &h );
          sprite.setTextColor( ST7735_ORANGE, ST7735_DARKGRAY );
          sprintf( pulseStr, "%4d", value );
          xpos = M5.Lcd.width() - (w + 1);
          break;
        case 0: // Left
          tft_setFont( LedDisplayFont );
          tft_getTextBounds( "000", (int16_t)0, (int16_t)0,  &x, &y, &w, &h );
          sprite.setTextColor( ST7735_YELLOW, ST7735_DARKGRAY );
          sprintf( pulseStr, "%3d", value );
          xpos = M5.Lcd.width() - w;
          break;
      }
      sprite.drawString( pulseStr, xpos, h / 2 - 4 );
      sprite.fillCircle( TFT_HALFWIDTH - 1, TFT_HALFHEIGHT - 1, 1, second() % 2 == 0 ? ST7735_RED : ST7735_WHITE );
      //M5.Lcd.fillCircle( 80, 55, 1, second() % 2 == 0 ? ST7735_WHITE : ST7735_RED );
      break;
    case DisplayBufferBitError:
      char errorStr[12];
      if (value > 99) value = 99;
      sprintf( errorStr, "E:%02d", value );
      tft_setFont( ErrorsCountFont );
      tft_getTextBounds( errorStr, (int16_t)0, (int16_t)0,  &x, &y, &w, &h );
      sprite.fillRect( M5.Lcd.width() - (w + 3), ErrorsCountFontHeight * 2 + 2, w + 2, h + 1, ST7735_DARKGRAY );
      sprite.setTextColor( ST7735_RED, ST7735_DARKGRAY );
      sprite.drawString( errorStr, M5.Lcd.width() - (w + 2), ErrorsCountFontHeight * 2 + 2 + 2  );
      break;
    default:
      log_e( "Ignored addr: %d, value: %d", addr, value );
  }
  giveMuxSemaphore();
}


void LedDCFStatus( int status ) {
  uint16_t color = ST7735_GRAY;
  switch ( status ) {
    case 0:
      color = ST7735_RED;
      break;
    case 1:
      color = ST7735_GREEN;
      break;
    case -1:
    default:
      //
      break;
  }
  takeMuxSemaphore();
  sprite.setTextColor( color );
  tft_setFont( LedDCFStatusFont );
  sprite.drawString( "DCF77", 0, 6 );
  giveMuxSemaphore();
}


void LedParityStatus( byte paritynum, int status ) {
  int xpos = 0;
  int ypos = ( M5.Lcd.height() -  2 * LedParityStatusFontHeight ) - 1; // TODO: normalize this
  String out = " ";
  uint16_t color;

  switch ( paritynum ) {
    case 1:
      out = "M";
      break;
    case 2:
      out = "S";
      xpos = 12;
      break;
    case 3:
      out = "D";
      xpos = 24;
      break;
    default:
      log_w( "invalid paritynum: %d", paritynum );
      return;
  }
  switch (status) {
    case -1:
      color = ST7735_LIGHTGRAY;
      break;
    case 0:
      color = ST7735_GREEN;
      break;
    case 1:
      color = ST7735_RED;
      break;
    default:
      log_w( "invalid status for paritynum %d", paritynum );
      return;
  }
  takeMuxSemaphore();
  tft_setFont( LedParityStatusFont );
  sprite.setTextColor( color );
  //M5.Lcd.setCursor(  xpos, ypos );
  //M5.Lcd.print( out );
  sprite.drawString( out, xpos, ypos );
  //M5.Lcd.setTextFont( 1 );
  giveMuxSemaphore();
}


void LedErrorStatus( byte lednum, int status ) {
  int xpos = 0;
  int ypos = 12;
  String out = "   ";
  uint16_t color = 0;

  takeMuxSemaphore();
  tft_setFont( LedErrorStatusFont );
  tft_getTextBounds( "0", (int16_t)0, (int16_t)0,  &x, &y, &tmpFontWidth, &tmpFontHeight );
  //Serial.printf("Font height for error led status is: %d", tmpFontHeight );
  ypos = tmpFontHeight * 2; //
  giveMuxSemaphore();

  switch ( status ) {
    case LOW:
      //M5.Lcd.fillCircle(xpos, ypos, 2, ST7735_BLACK);
      //M5.Lcd.drawCircle(xpos, ypos, 2, ST7735_GRAY);
      color = ST7735_GRAY;
      break;
    case HIGH:
      //M5.Lcd.fillCircle(xpos, ypos, 2, ST7735_RED);
      color = ST7735_RED;
      break;
    default:
      log_w( "Unhandled status: %d", status );
      return;
  }

  switch ( lednum ) {
    case 40: // LED_MINUTEMARKER output - LED - End of DCF data stream detected before buffer is filled, data is corrupt
      ypos += 0;
      out = "MMK";
      color = ( status == HIGH ) ? ST7735_GREEN : ST7735_GRAY;
      break;
    case 32: // LED_RTCERROR
      ypos += tmpFontHeight;//6;
      out = "RTC";
      color = ( status == HIGH ) ? ST7735_RED : ST7735_GRAY;
      break;
    case 33: // LED_RTCSYNC
      ypos += tmpFontHeight;//6;
      out = "RTC";
      color = ( status == HIGH ) ? ST7735_GREEN : ST7735_GRAY;
      break;
    case 39: // LED_BUFFERFULL output - LED - Buffer full indicator, next the data will be analized
      ypos += tmpFontHeight * 2; //12;
      out = "BFU";
      break;
    case 41: // LED_BUFFEROVERFLOW output - LED - More data received in one minute than expected due to bad signal
      ypos += tmpFontHeight * 3; //18;
      out = "BOV";
      break;
    case 38: // LED_ERRORPW output - LED - DCF Period Width error
      ypos += tmpFontHeight * 4; //24;
      out = "PW";
      break;
    case 37: // LED_ERRORPT output - LED - DCF Period Time error
      ypos += tmpFontHeight * 5; //30;
      out = "PT";
      break;

    default:
      log_w( "Unhandled lednum : %d", lednum );
      return;
  }
  takeMuxSemaphore();
  //tft_setFont( LedErrorStatusFont );
  sprite.setTextColor( color );
  //M5.Lcd.setCursor(  xpos, ypos );
  //M5.Lcd.print( out );
  //M5.Lcd.setTextFont( 1 );
  sprite.drawString( out, xpos, ypos );
  giveMuxSemaphore();
}


void LedWeekStatus( int weekDay, int status ) {
  int xpos = M5.Lcd.width() - ( weekDayNamesWidth + 2 ) + 1;
  int ypos = WeekDayLedYpos;//( M5.Lcd.height() - (LedWeekStatusFontHeight * 2) ) - 1;
  int vpos = 0;
  int fontwidth = 6;
  byte fontAscent = 5;
  switch ( weekDay ) {
    case 22: // LED_SUNDAY    // output - LED - Sunday
      xpos += 0;
      break;
    case 23: // LED_MONDAY    // output - LED - Monday
      vpos = 1;
      xpos += LedWeekStatusFontWidth;
      break;
    case 24: // LED_TUESDAY   // output - LED - Tuesday
      vpos = 2;
      xpos += LedWeekStatusFontWidth * 2;
      break;
    case 25: // LED_WEDNESDAY // output - LED - Wednesday
      vpos = 3;
      xpos += LedWeekStatusFontWidth * 3;
      break;
    case 26: // LED_THURSDAY  // output - LED - Thursday
      vpos = 4;
      xpos += LedWeekStatusFontWidth * 4;
      break;
    case 27: // LED_FRIDAY    // output - LED - Friday
      vpos = 5;
      xpos += LedWeekStatusFontWidth * 5;
      break;
    case 28: // LED_SATURDAY  // output - LED - Saturday
      vpos = 6;
      xpos += LedWeekStatusFontWidth * 6;
      break;
    case 29: // LED_CEST      // output - LED - Summertime CEST
      // CET/CEST status holders
      takeMuxSemaphore();
      tft_setFont( LedWeekStatusFont );
      sprite.fillRect( CETXPos, CETCESTYPos - LedWeekStatusFontHeight, CETWidth + 2, LedWeekStatusFontHeight, ST7735_WHITE );
      sprite.setTextColor( ST7735_LIGHTGRAY );
      sprite.drawString( "CET", CETXPos + 1, (CETCESTYPos - LedWeekStatusFontHeight) + 1 );
      sprite.fillRect( CESTXpos, CETCESTYPos - LedWeekStatusFontHeight, CESTWidth + 2, LedWeekStatusFontHeight, status ? ST7735_LIGHTGRAY : ST7735_WHITE );
      sprite.setTextColor( status ? ST7735_DARKGRAY : ST7735_LIGHTGRAY );
      sprite.drawString( "CEST", CESTXpos + 1, (CETCESTYPos - LedWeekStatusFontHeight) + 1 );
      giveMuxSemaphore();
      return;
      break;
    case 30: // LED_CET       // output - LED - Wintertime CET
      // CET/CEST status holders
      takeMuxSemaphore();
      tft_setFont( LedWeekStatusFont );
      sprite.fillRect( CETXPos, CETCESTYPos - LedWeekStatusFontHeight, CETWidth + 2, LedWeekStatusFontHeight, status ? ST7735_LIGHTGRAY : ST7735_WHITE );
      sprite.setTextColor( status ? ST7735_DARKGRAY : ST7735_LIGHTGRAY );
      sprite.drawString( "CET", CETXPos + 1, (CETCESTYPos - LedWeekStatusFontHeight) + 1 );
      sprite.fillRect( CESTXpos, CETCESTYPos - LedWeekStatusFontHeight, CESTWidth + 2, LedWeekStatusFontHeight, ST7735_WHITE );
      sprite.setTextColor( ST7735_LIGHTGRAY );
      sprite.drawString( "CEST", CESTXpos + 1, (CETCESTYPos - LedWeekStatusFontHeight) + 1 );
      giveMuxSemaphore();
      return;
      break;
    case 31: // LED_LEAPYEAR  // output - LED - Leap year
      // LEAP year status holders
      takeMuxSemaphore();
      tft_setFont( LedWeekStatusFont );
      sprite.fillRect( LEAPXPos, LeapYearYpos - LedWeekStatusFontHeight, LEAPWidth + 2, LedWeekStatusFontHeight, status ? ST7735_LIGHTGRAY : ST7735_WHITE );
      sprite.setTextColor( status ? ST7735_DARKGRAY : ST7735_LIGHTGRAY );
      sprite.drawString( "LEAP", LEAPXPos + 1, (LeapYearYpos - LedWeekStatusFontHeight) + 1 );
      giveMuxSemaphore();
      return;
      break;
    case 51: // LED_WEEKNUMBER
      char weekNumStr[5];
      sprintf( weekNumStr, "W:%02d", weekNumber );
      tft_getTextBounds( weekNumStr, (int16_t)0, (int16_t)0,  &x, &y, &tmpFontWidth, &LedWeekStatusFontHeight );
      // Week number
      takeMuxSemaphore();
      tft_setFont( LedWeekStatusFont );
      sprite.fillRect( M5.Lcd.width() - (tmpFontWidth + 2), WeekNumberYpos, tmpFontWidth + 2, LedWeekStatusFontHeight, ST7735_LIGHTGRAY ); 
      sprite.setTextColor( weekNumber > 0 ? ST7735_DARKGRAY : ST7735_GRAY );
      sprite.drawString( String( weekNumStr ), M5.Lcd.width() - (tmpFontWidth + 1), WeekNumberYpos +1 );
      giveMuxSemaphore();
      return;
      //weekNumber

      break;
    default:
      log_w("Invalid weekDay: %d", weekDay);
      return;
  }
  takeMuxSemaphore();
  switch (status) {
    case LOW:
      sprite.fillCircle( xpos, ypos, 1, ST7735_DARKGRAY );
      sprite.drawCircle( xpos, ypos, 1, ST7735_GRAY );
      break;
    case HIGH:
      sprite.fillCircle( xpos, ypos, 1, ST7735_ORANGE );
      break;
  }

  tft_setFont( LedWeekStatusFont );
  sprite.setCursor( WeekDayNamesXpos, WeekDayNamesYpos );

  char weekStr[] = "SMTWTFS";
  for ( byte pos = 0; pos < sizeof(weekStr); pos++ ) {
    if ( vpos != 0 && vpos == pos ) {
      sprite.setTextColor( status >= 0 ? ST7735_DARKGRAY : ST7735_GRAY );
    } else {
      sprite.setTextColor( ST7735_GRAY );
    }
    sprite.print( weekStr[pos] );
  }
  giveMuxSemaphore();
}


void setRingLed( byte ringNum, byte ledNum, bool enable, uint16_t color ) {
  float i = ( float( ledNum - 60 / 2 ) / 60) * TWO_PI;
  float x1, y1;
  byte r = 0;
  switch ( ringNum ) {
    case 0: // inner
      x1 = ringLedCoords[0][ledNum].x;
      y1 = ringLedCoords[0][ledNum].y;
      r = int_ringLedRadius;
      break;
    case 1: // outer
      x1 = ringLedCoords[1][ledNum].x;
      y1 = ringLedCoords[1][ledNum].y;
      r = int_ringLedRadius;
      break;
    default:
      log_w( "Invalid ringNum: %d", ringNum );
      return;
  }
  takeMuxSemaphore();
  if ( enable ) {
    switch ( ledNum ) {
      case 20: // start
      case 28: // p1
      case 35: // p2
      case 58: // p3
        color = ST7735_GREEN;
        break;
      default:
        color = ST7735_RED;
    }
    sprite.fillCircle( x1, y1, r, color );
    if (r > 1) {
      sprite.fillTriangle( x1, y1 - 1, x1 + 1, y1 - 1, x1 + 1, y1, ST7735_UGRAY );
    }
  } else {
    sprite.fillCircle( x1, y1, r, ST7735_DARKGRAY );
    sprite.drawCircle( x1, y1, r, ST7735_LIGHTGRAY );
  }
  //sprite.pushSprite( 0, 0 );
  giveMuxSemaphore();
}


void setRingCoords() {
  for ( uint8_t ledNum = 0; ledNum < 60; ledNum++ ) {
    float i = ( float( ledNum - 60 / 2 ) / 60 ) * TWO_PI;
    float x1, y1, x2, y2, x3, y3, x4, y4;
    x1 = ( -sin(i) * r1 ) + TFT_HALFWIDTH;
    y1 = ( cos(i)  * r1 ) + TFT_HALFHEIGHT;
    x2 = ( -sin(i) * r2 ) + TFT_HALFWIDTH;
    y2 = ( cos(i)  * r2 ) + TFT_HALFHEIGHT;
    x3 = ( -sin(i) * r3 ) + TFT_HALFWIDTH;
    y3 = ( cos(i)  * r3 ) + TFT_HALFHEIGHT;
    x4 = ( -sin(i) * r4 ) + TFT_HALFWIDTH;
    y4 = ( cos(i)  * r4 ) + TFT_HALFHEIGHT;
    // cache coords
    ringLedCoords[0][ledNum] = {x1, y1};
    ringLedCoords[1][ledNum] = {x2, y2};
    ringLedCoords[2][ledNum] = {x3, y3};
    ringLedCoords[3][ledNum] = {x4, y4};
  }
}


void drawRing() {
  takeMuxSemaphore();
  tft_setFont( RingLabelsFont );
  sprite.setTextColor( ST7735_GRAY );
  for ( uint8_t ledNum = 0; ledNum < 60; ledNum++ ) {
    float i = ( float( ledNum - 60 / 2 ) / 60 ) * TWO_PI;
    float x1, y1, x2, y2, x3, y3, x4, y4;
    x1 = ringLedCoords[0][ledNum].x;
    y1 = ringLedCoords[0][ledNum].y;
    x2 = ringLedCoords[1][ledNum].x;
    y2 = ringLedCoords[1][ledNum].y;
    x3 = ringLedCoords[2][ledNum].x;
    y3 = ringLedCoords[2][ledNum].y;
    x4 = ringLedCoords[3][ledNum].x;
    y4 = ringLedCoords[3][ledNum].y;
    uint16_t color1;
    if ( ledNum > 0 && ledNum < 15 ) { // meteo data
      color1 = M5.Lcd.color565( 0x44, 0x22, 0x44);
    } else if ( ledNum > 14 && ledNum < 21 ) { // antenna / leap hour / CEST / CET / leap sec . start
      color1 = M5.Lcd.color565( 0x22, 0x44, 0x44 );
    } else if ( ledNum > 20 && ledNum < 29 ) { // Minute
      color1 = M5.Lcd.color565( 0x66, 0x22, 0x00 );
    } else if ( ledNum > 28 && ledNum < 36 ) { // Hour
      color1 = M5.Lcd.color565( 0x44, 0x22, 0x44);
    } else if ( ledNum > 35 && ledNum < 42 ) { // Day
      color1 = M5.Lcd.color565( 0x22, 0x44, 0x44 );
    } else if ( ledNum > 41 && ledNum < 45 ) { // Day Of Week
      color1 = M5.Lcd.color565( 0x66, 0x22, 0x00 );
    } else if ( ledNum > 44 && ledNum < 50 ) { // Month
      color1 = M5.Lcd.color565( 0x44, 0x22, 0x44);
    } else if ( ledNum > 49 && ledNum < 58 ) { // Year
      color1 = M5.Lcd.color565( 0x22, 0x44, 0x44 );
    } else {                                  // Minute Marker
      color1 = M5.Lcd.color565( 0x22, 0x66, 0x22 );
    }
    if ( ledNum % 10 == 0 ) {
      sprite.drawLine( x2, y2, x3, y3, ST7735_LIGHTGRAY );
      tft_getTextBounds( String( ledNum ), (int16_t)0, (int16_t)0,  &x, &y, &w, &h );
      sprite.drawString( String(ledNum), x4 - w / 2, y4 - h / 2 );
    }

    for ( float l = ledNum - 0.5; l <= ledNum + 0.5; l += MinuteSteps ) {
      i = ( float( l - 60 / 2) / 60) * TWO_PI;
      x1 = ( -sin(i) * r1 ) + TFT_HALFWIDTH;
      y1 = ( cos(i)  * r1 ) + TFT_HALFHEIGHT;
      x2 = ( -sin(i) * r2 ) + TFT_HALFWIDTH;
      y2 = ( cos(i)  * r2 ) + TFT_HALFHEIGHT;
      sprite.drawLine( x1, y1, x2, y2, color1 );
    }
  }
  giveMuxSemaphore();
}


void enableRing( byte ringNum, uint16_t color ) {
  for ( uint8_t ledNum = 0; ledNum < 60; ledNum++ ) {
    setRingLed( ringNum, ledNum, true, color );
  }
}

/* probably unused but kept for semantics */
void clearRing( byte ringNum ) {
  for ( uint8_t ledNum = 0; ledNum < 60; ledNum++ ) {
    setRingLed( ringNum, ledNum, false );
  }
}


void error( int errorLed ) {
  // no valid data
  dcfValidSignal = false;
  // turn 'dcfValidSignal = false on' and clear Led's/displays because of error condition
  LedErrorStatus( errorLed, HIGH );
  LedErrorStatus( LED_RTCSYNC, LOW );
  LedDCFStatus( false );
  // clearRing( LedRingOuter );
  // increase errorCounter and display errorCount
  errorCounter++;
  LedDisplay( DisplayBufferBitError, "R", errorCounter );
}


#ifdef SPEAKER_PIN
void marioIntro() {
  //Mario main theme short intro
  uint16_t NOTE_C7 = 2093;
  uint16_t NOTE_E7 = 2637;
  uint16_t NOTE_G7 = 3136;
  int melody[] = {
    NOTE_E7, NOTE_E7, 0, NOTE_E7,
    0, NOTE_C7, NOTE_E7, 0,
    NOTE_G7, 0, 0,  0
  };
  int tempo = 12;
  int size = sizeof( melody ) / sizeof( int );
  for ( int thisNote = 0; thisNote < size; thisNote++ ) {
    int noteDuration = 1000 / tempo;
    Speaker.tone( melody[thisNote], noteDuration );
    int pauseBetweenNotes = noteDuration * 1.30;
    delay( pauseBetweenNotes );
    Speaker.tone( 0, noteDuration );
  }
}
#endif


void scheduleBuzz( uint16_t note, int duration ) {
#ifdef SPEAKER_PIN
  willBuzzNote = note;
  willBuzzDuration = duration;
#endif
}


void initPins() {
  // initialize PIN connections
  pinMode( DCF77PIN,           INPUT );
#ifdef DCF77_pdnPort
  pinMode( DCF77_pdnPort, OUTPUT );
  digitalWrite( DCF77_pdnPort, LOW );  // LOW=turn on radio
#endif
#ifdef DCF77_gainPort
  pinMode( DCF77_gainPort, OUTPUT );
  digitalWrite( DCF77_gainPort, HIGH );  // HIGH=turn on autogain
#endif

#ifdef BUTTON_A_PIN
  pinMode( BUTTON_A_PIN, INPUT_PULLUP );
#endif
#ifdef BUTTON_B_PIN
  pinMode( BUTTON_B_PIN, INPUT_PULLUP );
#endif
#ifdef BUTTON_C_PIN
  pinMode( BUTTON_C_PIN, INPUT_PULLUP );
#endif
}


void initSpeaker() {
#ifdef SPEAKER_PIN
  // TONE
  Speaker.begin();
  Speaker.setVolume( 2 );
  marioIntro();
#endif
}


void initRTC() {
#ifdef USE_RTC
  // Initialize RTC and set as SyncProvider.
  // Later RTC will be synced with DCF time
  RTC.begin( DS1307_SDA, DS1307_SCL );
  //delay(10);
  setSyncProvider( RTC.get );   // the function to get the time from the RTC
  //delay(500);
  // check if RTC has set the system time
  if ( timeStatus() != timeSet ) { // Unable to sync with the RTC - activate RTCError LED
    LedErrorStatus(LED_RTCERROR, HIGH);
    log_e( "RTC Time is not set! RTC.isrunning: %x", RTC.isrunning() );
    // use for test purposes and/or setting the RTC time manually
    //setTime(11, 15, 40, 13, 8, 19);
    //RTC.set(now());
  } else {
    // RTC has set the system time - dim RTCError LED
    LedErrorStatus( LED_RTCERROR, LOW );
    log_d( "hh:mm:ss: %02d:%02d:%02d", hour(), minute(), second() );
  }
#endif
}


static void InitUI() {
  M5.begin();
  M5.Lcd.clear();
  M5.Lcd.setRotation( displayRotation );
  M5.Lcd.fillScreen( ST7735_BLACKISH );

  //delay( 2000 );

  //delay( 1000 );
  //Serial.println(" DCF77 Clock starting ");
  //Serial.println();
  //Serial.println();
  //delay( 1000 );


  TFT_HALFWIDTH = M5.Lcd.width() / 2;
  TFT_HALFHEIGHT = M5.Lcd.height() / 2;

  LogoSprite.createSprite( 32, 32 );
  LogoSprite.fillSprite( ST7735_BLACKISH );
  LogoSprite.setSwapBytes( true );
  sprite_drawJpg( LogoSprite, 0, 0, atomic_32x32_jpeg, atomic_32x32_jpeg_len, 32, 32 );

  //TFT_eSprite sprite = TFT_eSprite(&M5.Lcd);
  sprite.createSprite( M5.Lcd.width(), M5.Lcd.height() );
  sprite.fillSprite( TFT_BLACK );
  sprite.pushImage( ( TFT_HALFWIDTH - 32 / 2 ), ( TFT_HALFHEIGHT - 32 / 2 ), 32, 32, (const uint16_t*)LogoSprite.frameBuffer(1) );

  tft_setFont( LedWeekStatusFont );
  tft_getTextBounds( "0", (int16_t)0, (int16_t)0,  &x, &y, &LedWeekStatusFontWidth, &LedWeekStatusFontHeight );

  tft_setFont( LedErrorStatusFont );
  tft_getTextBounds( "0", (int16_t)0, (int16_t)0,  &x, &y, &LedErrorStatusFontWidth, &LedErrorStatusFontHeight );

  tft_setFont( LedParityStatusFont );
  tft_getTextBounds( "0", (int16_t)0, (int16_t)0,  &x, &y, &LedParityStatusFontWidth, &LedParityStatusFontHeight );

  tft_setFont( LedDCFStatusFont );
  tft_getTextBounds( "0", (int16_t)0, (int16_t)0,  &x, &y, &LedDCFStatusFontWidth, &LedDCFStatusFontHeight );

  tft_setFont( ErrorsCountFont );
  tft_getTextBounds( "0", (int16_t)0, (int16_t)0,  &x, &y, &ErrorsCountFontWidth, &ErrorsCountFontHeight );

  tft_setFont( RingLabelsFont );
  tft_getTextBounds( "0", (int16_t)0, (int16_t)0,  &x, &y, &RingLabelsFontWidth, &RingLabelsFontHeight );

  tft_setFont( LedWeekStatusFont );
  tft_getTextBounds( "LEAP", (int16_t)0, (int16_t)0,  &x, &y, &LEAPWidth, &h );
  tft_getTextBounds( "CET", (int16_t)0, (int16_t)0,  &x, &y, &CETWidth, &h );
  tft_getTextBounds( "CEST", (int16_t)0, (int16_t)0,  &x, &y, &CESTWidth, &h );
  CESTXpos = M5.Lcd.width() - ( CESTWidth + 3 );
  CETXPos  = M5.Lcd.width() - ( CETWidth + 2 + 2 + CESTWidth + 2 );
  LEAPXPos = M5.Lcd.width() - ( LEAPWidth + 3 );

  tft_setFont( LedParityStatusFont );
  tft_getTextBounds( "01100101 P", (int16_t)0, (int16_t)0,  &x, &y, &tmpFontWidth, &tmpFontHeight );
  sprite.fillRect( 0,                                     M5.Lcd.height() - (LedParityStatusFontHeight + 2), tmpFontWidth - LedParityStatusFontWidth, LedParityStatusFontHeight, ST7735_LIGHTGRAY );
  sprite.fillRect( tmpFontWidth - LedParityStatusFontWidth, M5.Lcd.height() - (LedParityStatusFontHeight + 2), LedParityStatusFontWidth + 2,            LedParityStatusFontHeight, ST7735_ORANGE);
  sprite.setTextColor( ST7735_DARKGRAY );
  sprite.drawString( "01100101 P", 1, M5.Lcd.height() - (LedParityStatusFontHeight + 1)  );

  tft_setFont( LedWeekStatusFont );
  tft_getTextBounds( "SMTWTFS", (int16_t)0, (int16_t)0,  &x, &y, &weekDayNamesWidth, &h );
  sprite.fillRect( M5.Lcd.width() - (weekDayNamesWidth + 3),    M5.Lcd.height() - (LedWeekStatusFontHeight + 1), weekDayNamesWidth + 3,      LedWeekStatusFontHeight + 2, ST7735_LIGHTGRAY );
  sprite.fillRect( M5.Lcd.width() - (weekDayNamesWidth + 3),    M5.Lcd.height() - (LedWeekStatusFontHeight + 1), LedWeekStatusFontWidth + 2, LedWeekStatusFontHeight + 2, ST7735_ORANGE );
  sprite.fillRect( M5.Lcd.width() - (LedWeekStatusFontWidth + 2), M5.Lcd.height() - (LedWeekStatusFontHeight + 1), LedWeekStatusFontWidth + 2, LedWeekStatusFontHeight + 2, ST7735_ORANGE );
  sprite.setTextColor( ST7735_DARKGRAY );
  sprite.drawString( "SMTWTFS", WeekDayNamesXpos, WeekDayNamesYpos );

  LedParityStatus( 1, -1 ); // H
  LedParityStatus( 2, -1 ); // M
  LedParityStatus( 3, -1 ); // S
  LedDCFStatus( -1 );

  LedWeekStatus( LED_CEST, LOW );
  LedWeekStatus( LED_CET, LOW );

  sprite.pushSprite( 0, 0, TFT_BLACK );
}


void displayData( void ) {
  // display Day of Week on LED's
  // first, clear all the 'Day' Led's before displaying new value
  LedWeekStatus( LED_SUNDAY, LOW );
  LedWeekStatus( LED_MONDAY, LOW );
  LedWeekStatus( LED_TUESDAY, LOW );
  LedWeekStatus( LED_WEDNESDAY, LOW );
  LedWeekStatus( LED_THURSDAY, LOW );
  LedWeekStatus( LED_FRIDAY, LOW );
  LedWeekStatus( LED_SATURDAY, LOW );
  // switch on the Weekday LED
  switch ( dcfWeekDay ) {
    case 1:
      LedWeekStatus( LED_MONDAY, HIGH );
      break;
    case 2:
      LedWeekStatus( LED_TUESDAY, HIGH );
      break;
    case 3:
      LedWeekStatus( LED_WEDNESDAY, HIGH );
      break;
    case 4:
      LedWeekStatus( LED_THURSDAY, HIGH );
      break;
    case 5:
      LedWeekStatus( LED_FRIDAY, HIGH );
      break;
    case 6:
      LedWeekStatus( LED_SATURDAY, HIGH );
      break;
    case 7:
      LedWeekStatus( LED_SUNDAY, HIGH );
      break;
  }
  // display Summer- or Wintertime LED
  if ( dcfDST == 1 ) {
    LedWeekStatus( LED_CET, LOW );
    LedWeekStatus( LED_CEST, HIGH );
  } else {
    LedWeekStatus( LED_CEST, LOW );
    LedWeekStatus( LED_CET, HIGH );
  }
  // display Leap Year LED
  if ( leapYear == 1 ) {
    LedWeekStatus( LED_LEAPYEAR, HIGH );
  } else {
    LedWeekStatus( LED_LEAPYEAR, LOW );
  }
  LedWeekStatus( LED_WEEKNUMBER, weekNumber );
  Serial.printf( "dcfWeekDay: %d, weekNumber: %d, dcfDay: %d, dcfMonth: %d, dcfYear: %d, dcfDST: %d, leapYear: %d\n", dcfWeekDay, weekNumber, dcfDay, dcfMonth, dcfYear, dcfDST, leapYear );
}


void displayRtcTime() {
  byte fontAscent = 5;
  char timeStr[9];
  sprintf( timeStr, "%02d:%02d:%02d", hour(), minute(), second() );
  takeMuxSemaphore();
  tft_setFont( myRTCTimeFont );
  tft_getTextBounds( timeStr, (int16_t)0, (int16_t)0,  &x, &y, &w, &h );
  byte xpos = TFT_HALFWIDTH - w / 2;
  byte ypos = RTCTimeYPos;
  sprite.setTextColor( ST7735_UGRAY, ST7735_DARKGRAY );
  sprite.drawString( timeStr, xpos + 1, ypos );
  giveMuxSemaphore();
  log_i( "hh:mm:ss : %02d:%02d:%02d", hour(), minute(), second() );
}


void displayRtcDate() {
  char dateStr[11];
  sprintf( dateStr, "%04d-%02d-%02d", year(), month(), day() );
  takeMuxSemaphore();
  tft_setFont( myRTCDateFont );
  tft_getTextBounds( dateStr, (int16_t)0, (int16_t)0,  &x, &y, &w, &h );
  byte xpos = TFT_HALFWIDTH - w / 2;
  byte ypos = RTCDateYPos;
  sprite.setTextColor( ST7735_UGRAY, ST7735_DARKGRAY );
  //M5.Lcd.print( dateStr );
  sprite.drawString( dateStr, xpos, ypos );
  //tft_setFont( 1 );
  giveMuxSemaphore();
}


void LedTest() {
  // The displays are lit up sequentially because of the current draw.
  // When all is lit up at the same time, you would need a bigger power supply.

  // Outer LED ring
  for ( int i = 0; i < 60; i++ )  { // LED's ON
    setRingLed( LedRingOuter, i, true );
    sprite.pushSprite( 0, 0, TFT_BLACK );
  }
  for ( int i = 59; i >= 0; i-- ) { // LED's OFF
    setRingLed( LedRingOuter, i, false );
    sprite.pushSprite( 0, 0, TFT_BLACK );
  }
  // Inner LED ring
  for ( int i = 0; i < 60; i++ ) { // LED's ON
    setRingLed( LedRingInner, i, true );
    sprite.pushSprite( 0, 0, TFT_BLACK );
  }
  for ( int i = 59; i >= 0; i-- ) { // LED's OFF
    setRingLed( LedRingInner, i, false );
    sprite.pushSprite( 0, 0, TFT_BLACK );
  }
  for ( int i = 22; i <= 53; i++ ) { // LED's ON
    LedWeekStatus( i, HIGH );
    LedErrorStatus( i, HIGH );
    LedParityStatus( (i % 3) + 1, HIGH );
    sprite.pushSprite( 0, 0, TFT_BLACK );
  }
  // wait before turning the LED's off
  delay( LedTest_DELAY_DISPLAYS );
  for ( int i = 53; i >= 22; i-- ) { // LED's OFF
    LedWeekStatus( i, LOW );
    LedErrorStatus( i, LOW );
    LedParityStatus( (i % 3) + 1, LOW );
    sprite.pushSprite( 0, 0, TFT_BLACK );
  }
  LedWeekStatus( LED_WEEKNUMBER, -1 );
  sprite.pushSprite( 0, 0, TFT_BLACK );
}


void initialize(void) {
  InitUI();
  initPins();
  leadingEdge           = 0;
  trailingEdge          = 0;
  previousLeadingEdge   = 0;
  bufferPosition        = 0;
  // Reset DCFbitBuffer array, positions 0-58 (=59 bits)
  for ( int i = 0; i < 59; i++ ) {
    DCFbitBuffer[i] = 0;
    DCFbitFinalBuffer[i] = 0;
  }

  setRingCoords(); // cache ring coords
  drawRing();
  initSpeaker();
  initRTC();
  // check if a LED test is needed
  if ( PERFORM_LED_TEST == 1 ) {
    // do a LED test
    LedTest();
  }
  // activate errorCounter display after LED test
  LedDisplay( DisplayBufferBitError, "R", 0 );
  // spawn tasks
  initTasks();
}


#endif // _UI_H_
