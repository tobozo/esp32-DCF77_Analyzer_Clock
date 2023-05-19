#ifndef _UI_H_
#define _UI_H_

#include "./Config.h"

#ifdef USE_RTC
  #include "modules/RTC/DS1307RTC.h" // a custom DS1307 library that returns time as a time_t
  DS1307_RTC RTC;
  static time_t RTCGet()
  {
    return RTC.get();
  }
#else
  //#include <Time.h>
  #include <TimeLib.h>
#endif

void M5Begin()
{
  #ifndef LGFX_ONLY
    if( SERIAL_SPEED != 115200 ) {
      Serial.begin( SERIAL_SPEED );
      M5.begin(true, SD_ENABLE, false);
    } else {
      M5.begin();
    }
  #else
    Serial.begin( SERIAL_SPEED );
    Serial.printf("Custom ESP32 started at %d bauds!", SERIAL_SPEED);
    setDisplayPins();
    tft.setPanel(&panel);
    tft.init();
  #endif
}

#include "modules/SpriteSheet/SpriteSheet.h"

#ifdef DCF77_DO_WEATHER
  #include <Preferences.h>
  Preferences prefs;
  static int maxEntriesPerPage = 10;
  static int entriesIndex = -1;
  static bool pickedCity = false;
  static unsigned long pickerTimeout = 5000;
  static unsigned long beforePicking = millis();
  String preferredCountry = "";
  String preferredCity    = "";
#endif

#ifdef USE_SPEAKER
  SPEAKER Speaker;
#endif

#define freeheap heap_caps_get_free_size( MALLOC_CAP_INTERNAL )

static xSemaphoreHandle mux = NULL; // this is needed to prevent rendering collisions
// between scrollpanel and heap graph
#define takeMuxSemaphore() if( mux ) { xSemaphoreTake( mux, portMAX_DELAY ); log_v( "Took Semaphore" ); }
#define giveMuxSemaphore() if( mux ) { xSemaphoreGive( mux ); log_v( "Gave Semaphore" ); }


// this is just lgfx::TextStyle with a constructor
struct TextStyle
{
  std::uint32_t fore_rgb888 = 0xFFFFFFU;
  std::uint32_t back_rgb888 = 0;
  float size_x = 1;
  float size_y = 1;
  textdatum_t datum = textdatum_t::top_left;
  bool utf8 = true;
  bool cp437 = false;
  // constructor
  TextStyle( std::uint32_t _fore_rgb888, std::uint32_t _back_rgb888, float _size_x, float _size_y, textdatum_t _datum, bool _utf8, bool _cp437 ) :
    fore_rgb888( _fore_rgb888 ), back_rgb888( _back_rgb888 ), size_x( _size_x ), size_y( _size_y ), datum( _datum ), utf8( _utf8 ), cp437( _cp437 ) { }
  void setFgColor( std::uint32_t color ) { fore_rgb888 = color;  }
  void setBgColor( std::uint32_t color ) { back_rgb888 = color;  }
  void setSizeX( float size ) { size_x = size; }
  void setSizeY( float size ) { size_y = size; }
  void setTextDatum( textdatum_t newdatum ) { datum = newdatum; }
};

struct FontStyle
{
  const lgfx::IFont* font;
  TextStyle *style;
  // constructor
  FontStyle( const lgfx::IFont *_font, TextStyle *_style ) : font( _font ), style( _style ) { }
  // style setter
  void setTextStyle( TextStyle *_style ) {
    style = _style;
  }
};


struct BoxStyle
{
  const std::uint32_t fillColor;
  FontStyle* style;
  BoxStyle( const std::uint32_t  f, FontStyle *s ) : fillColor(f), style(s) { }
};


// helper for setFontStyle
void setTextStyle( DCFDisplay *_tft, TextStyle *style )
{
  lgfx::TextStyle myStyle; // why no constructor ??
  myStyle.fore_rgb888 = style->fore_rgb888;
  myStyle.back_rgb888 = style->back_rgb888;
  myStyle.size_x      = style->size_x;
  myStyle.size_y      = style->size_y;
  myStyle.datum       = style->datum;
  myStyle.utf8        = style->utf8;
  myStyle.cp437       = style->cp437;
  _tft->setTextStyle( myStyle );
}

void setFontStyle( DCFDisplay *_tft, FontStyle *myFontStyle )
{
  _tft->setFont( myFontStyle->font );
  setTextStyle( _tft, myFontStyle->style );
}


// helper for setFontStyle
void setTextStyle( LGFX_Sprite *sprite, TextStyle *style )
{
  lgfx::TextStyle myStyle; // why no constructor ??
  myStyle.fore_rgb888 = style->fore_rgb888;
  myStyle.back_rgb888 = style->back_rgb888;
  myStyle.size_x      = style->size_x;
  myStyle.size_y      = style->size_y;
  myStyle.datum       = style->datum;
  myStyle.utf8        = style->utf8;
  myStyle.cp437       = style->cp437;
  sprite->setTextStyle( myStyle );
}

void setFontStyle( LGFX_Sprite *sprite, FontStyle *myFontStyle )
{
  sprite->setFont( myFontStyle->font );
  setTextStyle( sprite, myFontStyle->style );
}


#if defined UI_320x240
  #include "themes/UI_320x240.h"
#elif defined UI_160x128
  #include "themes/UI_160x128.h"
#else
  #error "Please define either UI_320x240 or UI_160x128"
#endif


#include "modules/DCF77/DCF77.h"


static LGFX_Sprite sprite = LGFX_Sprite( &tft );

#ifdef DCF77_DO_WEATHER
  static LGFX_Sprite ScrollSprite = LGFX_Sprite( &tft );
#endif
static LGFX_Sprite MainSprite = LGFX_Sprite( &tft );
static LGFX_Sprite MaskSprite = LGFX_Sprite( &tft );
static LGFX_Sprite LogoSprite = LGFX_Sprite( &sprite );

static uint16_t TFT_HALFWIDTH;//    tft.width()/2
static uint16_t TFT_HALFHEIGHT;//   tft.height()/2
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
static uint16_t LedDisplayFontHeight = 0;
//static uint16_t BufferFontHeight = 0;
static uint16_t RTCDateFontHeight = 0;

//static uint16_t timeColor;

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

enum UIModes
{
  DCF_CLOCK, // default
  #ifdef DCF77_DO_WEATHER
  DCF_SETUP,
  #endif
  #ifdef USE_BUTTONS
  COOK_TIMER, // bonus ^^
  #endif
};

UIModes UIMode = DCF_CLOCK;
//UIModes UIMode = COOK_TIMER;


#ifdef DCF77_DO_WEATHER
  static String scrollText = "blah bleh blih bloh bluh blyh blah bleh blih bloh bluh blyh ";
  static uint16_t scrollWidth;
  static uint16_t scrollHeight;
  static int scrollPos = 0;
  //static int scrollFontSize = int_scrollFontSize;
  //static bool isScrolling = false;
  //static uint16_t scrollFontColor;
  //static uint16_t scrollFontColorDisabled = TFT_DARKGRAY;
  //static uint16_t scrollFontColorEnabled = TFT_GRAY;
#endif

//void sprite_drawJpg( LGFX_Sprite *spr, int16_t x, int16_t y, const uint8_t * jpg_data, size_t jpg_len, uint16_t maxWidth, uint16_t maxHeight );


void tft_drawSpriteSheet( SpriteSheetIcon icon, uint16_t x=0, uint16_t y=0 )
{
  uint16_t offsetx = (icon%16)*32;
  uint16_t offsety = (icon/16)*32;
  tft.drawJpg( sprite_jpg, sprite_jpg_len, x, y, 32, 32, offsetx, offsety );
}

void sprite_drawSpriteSheet( LGFX_Sprite &sprite, SpriteSheetIcon icon, uint16_t x=0, uint16_t y=0 )
{
  uint16_t offsetx = (icon%16)*32;
  uint16_t offsety = (icon/16)*32;
  //log_e("sprite.drawSpriteSheet( %d, %d, %d, %d, %d, %d );", x, y, 32, 32, offsetx, offsety );
  sprite.fillRect( x, y, 32, 32, TFT_BLACK );
  sprite.drawJpg( sprite_jpg, sprite_jpg_len, x, y, 32, 32, offsetx, offsety );
  //log_e("done");
}

static void getTextBounds( LGFX_Sprite *sprite, const char *string, uint16_t *w, uint16_t *h )
{
  *w = sprite->textWidth( string );
  *h = sprite->fontHeight();
}

#ifdef USE_BUTTONS
  #include "modules/CookTimer/CookTimer.h"
#endif

#include "Tasks.h"


#if defined USE_BUTTONS

  #if defined( BUTTON_A_PIN ) &&  defined( BUTTON_B_PIN ) &&  defined( BUTTON_C_PIN )

  void checkButtons()
  {
    //static byte longPushCounter = 0;
    M5.update();
    if ( longPushCounter >= 30 /*|| M5.BtnC.pressedFor( 1000 )*/ ) {
      longPushCounter = 0;
      // long press = switch UI mode
      switch( UIMode ) {
        #ifdef DCF77_DO_WEATHER
          case DCF_SETUP:
            // TODO: find something to do with long press when in setup mode :-)
          break;
        #endif
        case DCF_CLOCK:
          UIMode = COOK_TIMER;
          cookTimerSetup();
        break;
        case COOK_TIMER:
          UIMode = DCF_CLOCK;
          cookTimerStop();
        break;
      }
      // wait for button release
      while( M5.BtnC.isPressed() ) M5.update();
    }
    switch( UIMode ) {
      #ifdef DCF77_DO_WEATHER
        case DCF_SETUP:
          if ( M5.BtnA.wasPressed() ) { entriesIndex--; pickerTimeout = 600000; beforePicking = millis(); log_w("entriesIndex : %d", entriesIndex ); }
          if ( M5.BtnB.wasPressed() ) { entriesIndex++; pickerTimeout = 600000; beforePicking = millis(); log_w("entriesIndex : %d", entriesIndex ); }
          if ( M5.BtnC.wasPressed() ) pickedCity = true;
        break;
      #endif
      case DCF_CLOCK:
        if( M5.BtnC.isPressed() ) longPushCounter++;
        else longPushCounter = 0;
        #ifdef DCF77_DO_WEATHER
          if ( M5.BtnA.wasPressed() ) dumpWeather();
        #endif
        if ( M5.BtnB.wasPressed() ) dcf77SoundSwitch = 1 - dcf77SoundSwitch;
      break;
      case COOK_TIMER:
        if( M5.BtnC.isPressed() ) { longPushCounter++; CookTimerButtonPos = BUTTON_CLICK; }
        else if( M5.BtnB.isPressed() ) { longPushCounter = 0; CookTimerButtonPos = BUTTON_RIGHT; }
        else if( M5.BtnA.isPressed() ) { longPushCounter = 0; CookTimerButtonPos = BUTTON_LEFT; }
        else { longPushCounter = 0; CookTimerButtonPos = BUTTON_NEUTRAL; }
      break;
    }
    //delay(50);
  }

  #else
    // TODO: implement touch or other input methods
  #endif


#else

  void checkSerial()
  {
    if( Serial.available() ) {
      String respStr = Serial.readStringUntil('\n');
      int resp = respStr.toInt();
      if( resp < 0 || resp > citiesLength ) {
        Serial.println("Invalid entry, please provide the City ID, not the name");
        pickerTimeout = 600000;
        beforePicking = millis();
      } else {
        Serial.printf("Selecting city #%d ( %s / %s", resp, cities[resp].country.name, cities[resp].name );
        entriesIndex = resp;
        pickedCity = true;
      }
    }
  }

#endif


#ifdef DCF77_DO_WEATHER

  static void updateIcons( SpriteSheetIcon icon1, SpriteSheetIcon icon2, SpriteSheetIcon icon3 )
  {
    needrendering = 0;
    uint16_t posx = ( TFT_HALFWIDTH - 32 / 2 );
    uint16_t posy = ( TFT_HALFHEIGHT - 32 / 2 );

    if( icon1 != nullicon && icon1 != lasticon1 ) {
      lasticon1 = icon1;
      needrendering++;
      log_w("Icon1 will be rendererd with #%d", icon1 );
      drawIconTask( icon1, posx-32, posy );
      vTaskDelay(200);
    }
    if( icon2 != nullicon && icon2 != lasticon2 ) {
      lasticon2 = icon2;
      needrendering++;
      log_w("Icon2 will be rendererd with #%d", icon2 );
      drawIconTask( icon2, posx, posy );
      vTaskDelay(200);
    }
    if( icon3 != nullicon && icon3 != lasticon3 ) {
      lasticon3 = icon3;
      needrendering++;
      log_w("Icon3 will be rendererd with #%d", icon3 );
      drawIconTask( icon3, posx+32, posy );
      vTaskDelay(200);
    }
    if( needrendering > 0 ) {
      while( needrendering > 1 ) {
        takeMuxSemaphore();
        sprite.pushSprite( 0, 0 );
        giveMuxSemaphore();
        vTaskDelay( 10 );
      }
    }
  }


  void setupScroll()
  {

    takeMuxSemaphore();
    ScrollSprite.createSprite( tft.width(), scrollHeight );
    //ScrollSprite.fillSprite( TFT_BLACKISH );
    //ScrollSprite.setTextColor( scrollFontColor, TFT_BLACKISH );
    scrollFontStyle->setTextStyle( scrollStyleDisabled );
    setFontStyle( &ScrollSprite, scrollFontStyle );
    giveMuxSemaphore();

    takeMuxSemaphore();
    MaskSprite.createSprite( tft.width(), scrollHeight );
    MaskSprite.fillSprite( TFT_BLACK );
    MaskSprite.setSwapBytes( true );
    giveMuxSemaphore();
  }

  void unsetupScroll()
  {
    MaskSprite.deleteSprite();
    ScrollSprite.deleteSprite();
  }


  void initScroll( String text/*="                             Meteo is loading" */ )
  {
    scrollText = text;
    scrollPos = 0;
    //scrollFontSize = int_scrollFontSizeSmall;
    //scrollFontColor = scrollFontColorDisabled;
    scrollFontStyle->setTextStyle( scrollStyleDisabled );
    /*
    takeMuxSemaphore();
    uint16_t* mainSpritePtr = (uint16_t*)MainSprite.createSprite( tft.width(), tft.height() );
    if( mainSpritePtr == NULL ) {
      log_e("Unable to create mainSpritePtr");
    } else {
      log_n("Successfully created mainSpritePtr");
    }
    MainSprite.fillSprite( TFT_BLACK );
    MainSprite.setSwapBytes( true );
    giveMuxSemaphore();*/

    setFontStyle( &ScrollSprite, scrollFontStyle );
    //tft_setFont( scrollFontSize, ScrollSprite );
    getTextBounds( &sprite, scrollText.c_str(), &scrollWidth, &scrollHeight );

    setupScroll();
    unsetupScroll();
  }


  __attribute__((unused))
  void updateScroll( String text )
  {
    scrollText = text;
    scrollPos = 0;
    //scrollFontSize = int_scrollFontSize;
    //scrollFontColor = scrollFontColorEnabled;
    scrollFontStyle->setTextStyle( scrollStyleEnabled );
    //MaskSprite.deleteSprite();
    //ScrollSprite.deleteSprite();
    //setupScroll();
  }


  void handleScroll()
  {
    if ( scrollWidth <= 0 ) {
      Serial.println("Skipping zero width scroll");
      return;
    }
    setupScroll();

    takeMuxSemaphore();
    /*
    tft_setFont( scrollFontSize, ScrollSprite );
    ScrollSprite.drawString( scrollText, scrollPos, 0 );
    ScrollSprite.drawString( scrollText, scrollWidth + scrollPos, 0 );*/
    //scrollFontStyle->setTextStyle( );
    //setFontStyle( &ScrollSprite, scrollFontStyle );
    //ScrollSprite.fillSprite( TFT_GREEN );
    ScrollSprite.drawString( scrollText, scrollPos, 0 );
    giveMuxSemaphore();

    takeMuxSemaphore();
    //MaskSprite.pushImage( 0, -tft.height() / 2 + scrollHeight / 2, tft.width(), tft.height(), (const uint16_t*)sprite.frameBuffer(1)/*, TFT_BLACK*/ );
    //MaskSprite.pushImage( ( TFT_HALFWIDTH - 32 / 2 ), ( TFT_HALFHEIGHT - 32 / 2 ), 32, 32, (const uint16_t*)LogoSprite.frameBuffer(1) );
    //MaskSprite.pushImage( ( TFT_HALFWIDTH - 32 / 2 ), ( TFT_HALFHEIGHT - 32 / 2 ), 32, 32, (const uint16_t*)LogoSprite.frameBuffer(1) );
    MaskSprite.pushRotated( &ScrollSprite, 0/*, TFT_BLACK*/ );
    //MaskSprite.pushRotatedHP( &ScrollSprite, 0, TFT_BLACK );

    //ScrollSprite.pushImage( 0, 0, MaskSprite.width(), MaskSprite.height(), (const uint16_t*)MaskSprite.frameBuffer(1) );

    giveMuxSemaphore();

    takeMuxSemaphore();
    //MainSprite.pushImage( 0, tft.height() / 2 - scrollHeight / 2, tft.width(), scrollHeight, (const uint16_t*)ScrollSprite.frameBuffer(1)/*, TFT_BLACK*/);
    giveMuxSemaphore();
    scrollPos -= scrollSpeed;
    if ( scrollPos <= -scrollWidth ) {
      scrollPos = 0;
    }

    unsetupScroll();
  }

#endif



void displayBufferPosition( int dcfBit )
{
  takeMuxSemaphore();

  setFontStyle( &sprite, LedDisplayFontStyle );
  w = sprite.textWidth( "0" );
  sprite.setTextColor( TFT_RED, TFT_BLACK );
  sprite.drawString( dcfBit ? "1" : "0", tft.width() - w, LedDisplayFontHeight + 2 );

  char bufferPosStr[3];
  sprintf( bufferPosStr, "%02d", bufferPosition );
  w = sprite.textWidth( "000" );
  sprite.setTextColor( TFT_GREEN, TFT_BLACK );
  sprite.drawString( bufferPosStr, tft.width() - (w + 10), LedDisplayFontHeight + 2 );

  giveMuxSemaphore();
}


void LedDisplay( int addr, String leftOrRight, int value )
{
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
          LedDisplayFontStyle->setTextStyle( LedDisplayStylePingtime );
          setFontStyle( &sprite, LedDisplayFontStyle );
          w = sprite.textWidth( "00000000" );
          sprintf( pulseStr, "%4d ", value );
          xpos = tft.width() - w;
        break;
        case 0: // Left
          LedDisplayFontStyle->setTextStyle( LedDisplayStyleLeftover );
          setFontStyle( &sprite, LedDisplayFontStyle );
          w = sprite.textWidth( "000" );
          sprintf( pulseStr, "%3d", value );
          xpos = tft.width() - w;
        break;
      }
      sprite.drawString( pulseStr, xpos, 0 );
      sprite.fillCircle( TFT_HALFWIDTH - 1, TFT_HALFHEIGHT - 1, 1, second() % 2 == 0 ? TFT_RED : TFT_WHITE );
      //tft.fillCircle( 80, 55, 1, second() % 2 == 0 ? TFT_WHITE : TFT_RED );
    break;
    case DisplayBufferBitError:
      {
        char errorStr[12];
        if (value > 99) value = 99;
        sprintf( errorStr, "E: %02d", value );
        ErrorsCountFontStyle->setTextStyle( (value==0) ? ErrorsCountStyleOK : ErrorsCountStyleKO );
        setFontStyle( &sprite, ErrorsCountFontStyle );
        w = sprite.textWidth( errorStr  );
        sprite.fillRect( tft.width() - (w + 3), ErrorsCountFontHeight * 2 + 2, w + 2, ErrorsCountFontHeight + 1, TFT_DARKGRAY );
        sprite.drawString( errorStr, tft.width() - w, ErrorsCountFontHeight * 2 + 3 );
      }
    break;
    default:
      log_e( "Ignored addr: %d, value: %d", addr, value );
  }
  giveMuxSemaphore();
}


void LedDCFStatus( int status )
{
  //uint16_t color = TFT_GRAY;
  switch ( status ) {
    case 0:
      LedDCFStatusFontStyle->setTextStyle( LedDCFStatusStyleKO );
      //color = TFT_RED;
      break;
    case 1:
      LedDCFStatusFontStyle->setTextStyle( LedDCFStatusStyleOK );
      //color = TFT_GREEN;
      break;
    case -1:
    default:
      //
      break;
  }
  takeMuxSemaphore();
  setFontStyle( &sprite, LedDCFStatusFontStyle );
  sprite.drawString( "DCF77", 0, 0 );
  giveMuxSemaphore();
}


void LedParityStatus( byte paritynum, int status )
{
  int xpos = 0;
  int ypos = ( tft.height() -  2 * LedParityStatusFontHeight ) - 2; // TODO: normalize this
  String out = " ";
  //uint16_t color;

  switch ( paritynum ) {
    case 1: out = "Min";  xpos = MHDXPos[0]; break;
    case 2: out = "Hr";   xpos = MHDXPos[1]; break;
    case 3: out = "Date"; xpos = MHDXPos[2]; break;
    default: log_d( "invalid paritynum: %d", paritynum ); return;
  }
  switch (status) {
    case -1:
      //color = TFT_LIGHTGRAY;
      LedParityStatusFontStyle->setTextStyle( LedParityStatusStyleNA );
      break;
    case 0:
      //color = TFT_GREEN;
      LedParityStatusFontStyle->setTextStyle( LedParityStatusStyleOK );
      break;
    case 1:
      //color = TFT_RED;
      LedParityStatusFontStyle->setTextStyle( LedParityStatusStyleKO );
      break;
    default:
      log_d( "invalid status for paritynum %d", paritynum );
      return;
  }
  takeMuxSemaphore();

  setFontStyle( &sprite, LedParityStatusFontStyle );
  sprite.drawString( out, xpos, ypos );
  giveMuxSemaphore();
}


void LedErrorStatus( byte lednum, int status )
{
  int xpos = 0;
  int ypos = 12;
  String out = "   ";

  takeMuxSemaphore();

  ypos = LedErrorStatusFontHeight; //

  switch ( status ) {
    case LOW:
      LedErrorStatusFontStyle->setTextStyle( LedErrorStatusStyleGray );
    break;
    case HIGH:
      LedErrorStatusFontStyle->setTextStyle( LedErrorStatusStyleRed );
    break;
    default:
      log_d( "Unhandled status: %d", status );
      return;
  }

  switch ( lednum ) {
    case 40: // LED_MINUTEMARKER output - LED - End of DCF data stream detected before buffer is filled, data is corrupt
      ypos += 0;
      out = "MMK";
      LedErrorStatusFontStyle->setTextStyle( ( status == HIGH ) ? LedErrorStatusStyleGreen : LedErrorStatusStyleGray );
    break;
    case 32: // LED_RTCERROR
      ypos += LedErrorStatusFontHeight;//6;
      out = "RTC";
      LedErrorStatusFontStyle->setTextStyle( ( status == HIGH ) ? LedErrorStatusStyleRed : LedErrorStatusStyleGray );
      myRTCTimeFontStyle->setTextStyle ( ( status == HIGH ) ? myRTCDateTimeTextStyleKO : myRTCDateTimeTextStyleNA );
    break;
    case 33: // LED_RTCSYNC
      ypos += LedErrorStatusFontHeight;//6;
      out = "RTC";
      LedErrorStatusFontStyle->setTextStyle( ( status == HIGH ) ? LedErrorStatusStyleGreen : LedErrorStatusStyleGray );
      myRTCTimeFontStyle->setTextStyle ( ( status == HIGH ) ? myRTCDateTimeTextStyleOK : myRTCDateTimeTextStyleNA );
    break;
    case 39: // LED_BUFFERFULL output - LED - Buffer full indicator, next the data will be analized
      ypos += LedErrorStatusFontHeight * 2; //12;
      out = "BFU";
    break;
    case 41: // LED_BUFFEROVERFLOW output - LED - More data received in one minute than expected due to bad signal
      ypos += LedErrorStatusFontHeight * 3; //18;
      out = "BOV";
    break;
    case 38: // LED_ERRORPW output - LED - DCF Period Width error
      ypos += LedErrorStatusFontHeight * 4; //24;
      out = "PW";
    break;
    case 37: // LED_ERRORPT output - LED - DCF Period Time error
      ypos += LedErrorStatusFontHeight * 5; //30;
      out = "PT";
    break;

    default:
      log_d( "Unhandled lednum : %d", lednum );
      return;
  }

  setFontStyle( &sprite, LedErrorStatusFontStyle );
  sprite.drawString( out, xpos, ypos );
  giveMuxSemaphore();
}


void LeapSecondWarning()
{
  takeMuxSemaphore();
  //TODO: manage coordinates and text styles from 320x240 and 160x128 profiles

  setFontStyle( &sprite, StatusFontStyle ); // centered / small font

  sprite.setTextColor(TFT_RED, TFT_BLACK);
  sprite.drawString("Leap Sec Inserted", tft.width()/2, 95 );

  // record date of last Leap Second on TFT
  sprite.setTextColor(TFT_GREEN, TFT_BLACK);
  sprite.drawString("Leap Sec", tft.width()/2, 170);

  char timeStr[16];
  sprintf( timeStr, TIME_FORMAT, hour(), minute(), bufferPosition +1 );
  sprite.drawString( timeStr, tft.width()/2, 180 );

  char dateStr[16];
  sprintf( dateStr, DATE_FORMAT, year(), month(), day() );
  sprite.drawString( timeStr, tft.width()/2, 190 );

  giveMuxSemaphore();

  // record Leap Second date and time on serial port
  Serial.printf( "Leap Second Inserted %02d:%02d:%02d 20%02d/%02d/%02d ]\n",
    dcfHour,
    dcfMinute,
    bufferPosition +1,
    dcfYear,
    dcfMonth,
    dcfDay
  );

}




static const char weekStr[] = "SMTWTFS";

void drawWeekDays( int daynum )
{

  int hpos = sprite.width();
  int vpos = sprite.height();
  int margin = 1;

  int boxWidth = LedWeekStatusFontWidth + (margin);
  int boxHeight = (LedWeekStatusFontHeight + (margin*2))-1;

  int boxCenterX = boxWidth/2 - margin;
  int boxCenterY = boxHeight/2 + margin*2;

  hpos -= (boxWidth*7); // right aligned
  vpos -= (boxHeight);

  setFontStyle( &sprite, LedWeekStatusFontStyle );

  for ( byte pos = 0; pos < 7; pos++ ) {
    if ( daynum > -1 && daynum == pos ) {
      sprite.fillCircle( hpos+(boxWidth/2), vpos-2, 1, TFT_ORANGE );
      sprite.fillRect( hpos, vpos, boxWidth, boxHeight, TFT_DARKRED );
      sprite.setTextColor( TFT_UGRAY );
    } else {
      sprite.fillCircle( hpos+(boxWidth/2), vpos-2, 1, TFT_DARKGRAY );
      sprite.drawCircle( hpos+(boxWidth/2), vpos-2, 1, TFT_GRAY );
      if( pos == 0 || pos == 6 ) {
        sprite.fillRect( hpos, vpos, boxWidth, boxHeight, TFT_ORANGE );
      } else {
        sprite.fillRect( hpos, vpos, boxWidth, boxHeight, TFT_LIGHTGRAY );
      }
      sprite.setTextColor( TFT_GRAY );
    }
    sprite.drawChar( weekStr[pos],  hpos+boxCenterX+LedWeekStatusMarginX, vpos+boxCenterY+LedWeekStatusMarginY );
    hpos += boxWidth;
  }

}


#ifdef DCF77_DO_WEATHER





#endif



void LedWeekStatus( int weekDay, int status )
{
  int xpos = tft.width() - ( weekDayNamesWidth + 1 );
  //int ypos = ( tft.height() - (LedWeekStatusFontHeight) ) - 3;
  int weekDayBlockWidth = LedWeekStatusFontWidth + 2;
  int wpos = -1;

  switch ( weekDay ) {
    case 22: // LED_SUNDAY    // output - LED - Sunday
      wpos = 0;
      xpos += 0;
    break;
    case 23: // LED_MONDAY    // output - LED - Monday
      wpos = 1;
      xpos += weekDayBlockWidth;
    break;
    case 24: // LED_TUESDAY   // output - LED - Tuesday
      wpos = 2;
      xpos += weekDayBlockWidth * 2;
    break;
    case 25: // LED_WEDNESDAY // output - LED - Wednesday
      wpos = 3;
      xpos += weekDayBlockWidth * 3;
    break;
    case 26: // LED_THURSDAY  // output - LED - Thursday
      wpos = 4;
      xpos += weekDayBlockWidth * 4;
    break;
    case 27: // LED_FRIDAY    // output - LED - Friday
      wpos = 5;
      xpos += weekDayBlockWidth * 5;
    break;
    case 28: // LED_SATURDAY  // output - LED - Saturday
      wpos = 6;
      xpos += weekDayBlockWidth * 6;
    break;
    case 29: // LED_CEST      // output - LED - Summertime CEST
      takeMuxSemaphore();
      setFontStyle( &sprite, status ?  BoxSelected->style : BoxUnSelected->style );
      sprite.fillRect( CESTXpos, CETCESTYPos - LedWeekStatusFontHeight - 1, CESTWidth + 2, LedWeekStatusFontHeight+1, status ? BoxSelected->fillColor : BoxUnSelected->fillColor );
      sprite.drawString( "CEST", CESTXpos + 1, (CETCESTYPos - LedWeekStatusFontHeight) );
      giveMuxSemaphore();
      return;
    break;
    case 30: // LED_CET       // output - LED - Wintertime CET
      takeMuxSemaphore();
      setFontStyle( &sprite, status ?  BoxSelected->style : BoxUnSelected->style );
      sprite.fillRect( CETXPos, CETCESTYPos - LedWeekStatusFontHeight - 1, CETWidth + 2, LedWeekStatusFontHeight+1, status ? BoxSelected->fillColor : BoxUnSelected->fillColor );
      sprite.drawString( "CET", CETXPos + 1, (CETCESTYPos - LedWeekStatusFontHeight) );
      giveMuxSemaphore();
      return;
    break;
    case 31: // LED_LEAPYEAR  // output - LED - Leap year
      takeMuxSemaphore();
      setFontStyle( &sprite, status ?  BoxSelected->style : BoxUnSelected->style );
      sprite.fillRect( LEAPXPos, LeapYearYpos - LedWeekStatusFontHeight - 1, LEAPWidth + 2, LedWeekStatusFontHeight+1, status ? BoxSelected->fillColor : BoxUnSelected->fillColor );
      sprite.drawString( "LEAP", LEAPXPos + 1, (LeapYearYpos - LedWeekStatusFontHeight) );
      giveMuxSemaphore();
      return;
    break;
    case 51: // LED_WEEKNUMBER
      char weekNumStr[5];
      sprintf( weekNumStr, "W:%02d", weekNumber );
      takeMuxSemaphore();
      setFontStyle( &sprite, weekNumber > 0 ?  BoxSelected->style : BoxUnSelected->style );
      tmpFontWidth = sprite.textWidth( weekNumStr );
      sprite.fillRect( tft.width() - (tmpFontWidth + 2), WeekNumberYpos - 1, tmpFontWidth + 2, LedWeekStatusFontHeight+1, weekNumber > 0 ? BoxSelected->fillColor : BoxUnSelected->fillColor );
      sprite.drawString( String( weekNumStr ), tft.width() - (tmpFontWidth + 1), WeekNumberYpos );
      giveMuxSemaphore();
      return;
    break;
    default:
      log_d("Invalid weekDay: %d", weekDay);
      return;
  }

  takeMuxSemaphore();
  if( wpos > -1 ) {
    drawWeekDays( wpos );
  }
  giveMuxSemaphore();
}


void setRingLed( byte ringNum, byte ledNum, bool enable, bool clear )
{
  //float i = ( float( ledNum - 60 / 2 ) / 60) * TWO_PI;
  float x1, y1;
  uint16_t fillcolor, shinecolor;
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
      log_d( "Invalid ringNum: %d", ringNum );
      return;
  }
  takeMuxSemaphore();
  if ( enable ) {
    switch ( ledNum ) {
      case 20: // start
      case 28: // p1
      case 35: // p2
      case 58: // p3
        fillcolor = TFT_GREENISH;
        shinecolor = TFT_WHITE;
        break;
      default:
        fillcolor = TFT_RED;
        shinecolor = TFT_UGRAY;
    }
    sprite.fillCircle( x1, y1, r, fillcolor );
    if (r > 1) {
      sprite.fillTriangle( x1, y1 - 1, x1 + 1, y1 - 1, x1 + 1, y1, shinecolor );
    }
  } else {
    if( clear ) {
      sprite.fillCircle( x1, y1, r, TFT_DARKGRAY );
      sprite.drawCircle( x1, y1, r, TFT_LIGHTGRAY );
    } else {
      sprite.fillCircle( x1, y1, r, TFT_LIGHTGRAY );
      sprite.fillTriangle( x1, y1 - 1, x1 + 1, y1 - 1, x1 + 1, y1, TFT_WHITE );
    }
  }
  giveMuxSemaphore();
}


void setRingCoords()
{
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


void drawRing()
{
  takeMuxSemaphore();

  setFontStyle( &sprite, RingLabelsFontStyle );

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
      color1 = tft.color565( 0x44, 0x22, 0x44);
    } else if ( ledNum > 14 && ledNum < 21 ) { // antenna / leap hour / CEST / CET / leap sec . start
      color1 = tft.color565( 0x22, 0x44, 0x44 );
    } else if ( ledNum > 20 && ledNum < 29 ) { // Minute
      color1 = tft.color565( 0x66, 0x22, 0x00 );
    } else if ( ledNum > 28 && ledNum < 36 ) { // Hour
      color1 = tft.color565( 0x44, 0x22, 0x44);
    } else if ( ledNum > 35 && ledNum < 42 ) { // Day
      color1 = tft.color565( 0x22, 0x44, 0x44 );
    } else if ( ledNum > 41 && ledNum < 45 ) { // Day Of Week
      color1 = tft.color565( 0x66, 0x22, 0x00 );
    } else if ( ledNum > 44 && ledNum < 50 ) { // Month
      color1 = tft.color565( 0x44, 0x22, 0x44);
    } else if ( ledNum > 49 && ledNum < 58 ) { // Year
      color1 = tft.color565( 0x22, 0x44, 0x44 );
    } else {                                  // Minute Marker
      color1 = tft.color565( 0x22, 0x66, 0x22 );
    }
    if ( ledNum % 10 == 0 ) {
      sprite.drawLine( x2, y2, x3, y3, TFT_LIGHTGRAY );
      sprite.drawString( String(ledNum).c_str(), x4, y4);
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


/* probably unused but kept for semantics */
void clearRing( byte ringNum )
{
  for ( uint8_t ledNum = 0; ledNum < 60; ledNum++ ) {
    setRingLed( ringNum, ledNum, false );
  }
}


void error( int errorLed )
{
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


void displayRtcTime()
{
  //byte fontAscent = 5;
  char timeStr[16];
  sprintf( timeStr, TIME_FORMAT, hour(), minute(), second() );
  takeMuxSemaphore();
  setFontStyle( &sprite, myRTCTimeFontStyle );
  sprite.drawString( timeStr, TFT_HALFWIDTH, RTCTimeYPos );
  giveMuxSemaphore();
  log_i( "hh:mm:ss (%s) : %02d:%02d:%02d @ %d:%d [%d*%d]", timeStr, hour(), minute(), second(), x, y, w, h );
}


void displayRtcDate()
{
  char dateStr[16];
  sprintf( dateStr, DATE_FORMAT, year(), month(), day() );
  takeMuxSemaphore();
  setFontStyle( &sprite, myRTCDateFontStyle );
  sprite.drawString( dateStr, TFT_HALFWIDTH, RTCDateYPos );
  giveMuxSemaphore();

  #ifdef USE_BUTTONS
    uint16_t fontHeight = sprite.fontHeight();
    uint16_t textWidth = tft.textWidth( dateStr );
    timerBox.width  = textWidth  > timerBox.width  ? textWidth  : timerBox.width;
    timerBox.height = fontHeight > timerBox.height ? fontHeight : timerBox.height;
    timerBox.x      = TFT_HALFWIDTH - timerBox.width/2;
    timerBox.y      = RTCDateYPos - timerBox.height/2;
  #endif
}


#ifdef USE_SPEAKER
  void marioIntro()
  {
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
      //Speaker.tone( 0, noteDuration );
      Speaker.mute();
    }
    Speaker.mute();

  }
#endif


void scheduleBuzz( uint16_t note, int duration )
{
  #ifdef USE_SPEAKER
    willBuzzNote = note;
    willBuzzDuration = duration;
  #endif
}


#ifdef DCF77_DO_WEATHER


  static const char weatherBitsStatusStr[] = "123";

  static const String weatherBitsStr[3] = { "110", "101", "011" };

  void LedWeatherStatus( int bitnumber, int bitstatus, bool echo = true )
  {

    int bitposition = 0;

    setFontStyle( &sprite, LedWeekStatusFontStyle );

    switch( bitnumber ) {
      case 0: bitposition = 2; if( echo ) Serial.println( "Received Meteo Bits 29-42" ); break;
      case 1: bitposition = 0; if( echo ) Serial.println( "Received Meteo Bits 01-14" );  break;
      case 2: bitposition = 1; if( echo ) Serial.println( "Received Meteo Bits 15-28" ); break;
      default: log_e("Invalid meteo bit index !!"); return;
    }

    switch( bitstatus ) {
      case -1: sprite.setTextColor( TFT_RED,   TFT_DARKGRAY ); if( echo ) log_w("weather data couldn't be decrypted/uncompressed"); break;
      case  0: sprite.setTextColor( TFT_WHITE, TFT_DARKGRAY );/* grey */ break;
      case  1: sprite.setTextColor( TFT_GREEN, TFT_DARKGRAY );/* green */ break;
      default: log_e("Invalid meteo bit status !!");
    }

    int hpos = 0; // left aligned
    int vpos = sprite.height(); // bottom aligned
    int margin = 1;

    //sprite.fillRect( 0, tft.height() - (LedParityStatusFontHeight + 2), tmpFontWidth - LedParityStatusFontWidth, LedParityStatusFontHeight+1, TFT_DARKGRAY );
    for( int i=0;i<3;i++) {
      getTextBounds( &sprite, weatherBitsStr[i].c_str(), &tmpFontWidth, &tmpFontHeight );
      int boxCenterX = /*(tmpFontWidth/2)+*/margin;
      int boxCenterY = (tmpFontHeight/2)-margin;
      if( bitposition == 0 ) {
        // resetting, process all
        sprite.drawString( weatherBitsStr[i], hpos+boxCenterX, (vpos-tmpFontHeight)+boxCenterY );
        if( i==0 ) sprite.setTextColor( TFT_GRAY );
      } else {
        // updating, process one
        if( bitposition == i ) sprite.drawString( weatherBitsStr[i], hpos+boxCenterX, (vpos-tmpFontHeight)+boxCenterY );
      }
      hpos += tmpFontWidth;
    }

  }


  void printWatchedCity( bool detected = false )
  {
    takeMuxSemaphore();
    setFontStyle( &sprite, StatusFontStyle ); // centered / small font
    if( detected ) {
      sprite.setTextColor(TFT_GREEN, TFT_BLACK);
    } else {
      sprite.setTextColor(TFT_ORANGE, TFT_BLACK);
    }
    sprite.drawString( watchedCity, tft.width()/2, 165 ); // This uses the standard ADAFruit small font
    giveMuxSemaphore();
  }


  char countryCityHolder[64];
  //int citiesLength = 0;
  int halfPage = 0;

  void paginateCities()
  {

    if( entriesIndex < 0 ) {
      entriesIndex = citiesLength-1;
    }
    if( entriesIndex >= citiesLength ) {
      entriesIndex = 0;
    }

    int pageStart = entriesIndex - halfPage;
    int pageEnd   = entriesIndex + halfPage ;

    if( entriesIndex < halfPage ) {
      pageEnd+=halfPage;
    }

    if( pageEnd >= citiesLength ) {
      pageEnd = citiesLength-1;
    }
    if( pageStart < 0 ) {
      pageStart = 0;
    }

    log_d("Found %d cities, paginating from %d to %d (index=%d)", citiesLength, pageStart, pageEnd, entriesIndex );

    sprite.fillScreen( CountryCityFontStylePicoDisabled->style->back_rgb888 );

    uint8_t linenum = 0;

    for( int i=pageStart; i<=pageEnd; i++ ) {
      if( cities[i].name == nullptr ) continue;
      if( i == entriesIndex ) {
        setFontStyle( &sprite, CountryCityFontStylePicoEnabled );
      } else {
        setFontStyle( &sprite, CountryCityFontStylePicoDisabled );
      }
      sprintf( countryCityHolder, " %s : %s ", cities[i].country.name, cities[i].name );
      sprite.drawString( countryCityHolder, 0, linenum*RTCDateFontHeight );
      linenum++;
    }
    sprite.pushSprite( 0, 0/*, TFT_BLACK*/ );
  }


  void LoadCountryPref( const countryBycode* country )
  {
    watchedCountry = (countryBycode*)country;
  }
  void LoadCityPref( const char* city )
  {
    watchedCity = (char*)city;
  }


  void CountryCityWizard( unsigned long timeout )
  {
    #ifndef USE_BUTTONS
      // TODO: handle serial input instead of blaming the lack of HID
      log_n("Please find your preferred city and enter the number");
      //return;
    #endif
    maxEntriesPerPage = tft.height() / RTCDateFontHeight;
    halfPage = maxEntriesPerPage / 2;
    log_d("maxEntriesPerPage / citiesLength / halfPage : %d / %d / %d", maxEntriesPerPage, citiesLength, halfPage);
    for( int i=0; i<citiesLength; i++ ) {
      #ifndef USE_BUTTONS
        log_n("[%2d] %s / %s",  i, cities[i].country.name, cities[i].name );
      #else
        log_d("Index / Country / City: %d / %s / %s",  i, cities[i].country.name, cities[i].name );
      #endif
    }
    UIMode = DCF_SETUP;
    beforePicking = millis();
    //bool has_timed_out = false;
    pickerTimeout = timeout;
    while( !pickedCity ) {
      #ifndef USE_BUTTONS
        checkSerial();
      #else
        checkButtons();
        paginateCities();
      #endif
      if( beforePicking + pickerTimeout < millis() ) {
        //has_timed_out = true;
        break;
      }
    }
    if(  preferredCountry != String( cities[entriesIndex].country.name )
      || preferredCity    != String( cities[entriesIndex].name ) ) {
      // save only if choice changed
      log_w("Prefs have changed, clearing/saving!");

      /*
      prefs.begin("DCF77WData", false);
      prefs.clear();
      prefs.end();
      */
      //esp_err_t result = nvs_flash_erase();

      prefs.begin("DCF77Prefs", false);
      prefs.clear();
      prefs.putString( "country", cities[entriesIndex].country.name );
      prefs.putString( "city", cities[entriesIndex].name );
      prefs.end();
    }
    LoadCountryPref( &cities[entriesIndex].country );
    LoadCityPref( cities[entriesIndex].name );
    sprite.fillScreen( TFT_BLACK );
    sprite.pushSprite( 0, 0 );
    UIMode = DCF_CLOCK;
  }


  void InitPrefs()
  {

    #if defined FORCED_WATCHED_CITY && defined FORCED_WATCHED_COUNTRY
      preferredCountry = String( FORCED_WATCHED_COUNTRY );
      preferredCity    = String( FORCED_WATCHED_CITY );
    #else
      prefs.begin("DCF77Prefs", true);
      preferredCountry = prefs.getString("country");
      preferredCity    = prefs.getString("city");
      prefs.end();
    #endif
    //citiesLength = ( sizeof(cities) / sizeof(cityByCountry) ) -1;


    if( preferredCountry != "" ) {
      log_w("Found preferredCountry '%s' in NVS, will compare with local list", preferredCountry.c_str() );
      const countryBycode* country = findCountryByName( preferredCountry.c_str() );
      const char* prefcity = preferredCity.c_str();
      if( country != nullptr ) {
        LoadCountryPref( country );
        if( preferredCity != "" ) {
          log_w("Found preferredCity '%s' in NVS, will compare with local list", prefcity );
          int cityID = findCityIDByName( prefcity );
          if( cityID > -1 ) {
            LoadCityPref( prefcity );
            for( int i=0; i<citiesLength; i++ ) {
              if( strcmp( prefcity, cities[i].name )==0 && strcmp( country->name, cities[i].country.name )==0 ) {
                entriesIndex = i;
                #if defined FORCED_WATCHED_CITY && defined FORCED_WATCHED_COUNTRY
                  CountryCityWizard( 100 );
                #else
                  CountryCityWizard( 5000 );
                #endif

                log_w("Free heap before weather data memory allocation: %d", ESP.getFreeHeap() );
                initWeatherForecastCache();
                initMyCityForecastsCache();
                log_w("Free heap after weather data memory allocation: %d", ESP.getFreeHeap() );
                return;
              }
            }
          } // else invalid city or end of array
        } // else empty city name returned from prefs
      } // else invalid country or end of array
    } // else empty country name returned from prefs
    CountryCityWizard( 600000 );

    log_w("Free heap before weather data memory allocation: %d", ESP.getFreeHeap() );
    initWeatherForecastCache();
    initMyCityForecastsCache();
    log_w("Free heap after weather data memory allocation: %d", ESP.getFreeHeap() );

  }
#endif


void InitPins()
{
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
}


void initSpeaker()
{
  #ifdef USE_SPEAKER
    // TONE
    Speaker.begin();
    Speaker.setVolume( 2 );
    marioIntro();
  #endif
}


#ifdef USE_RTC
void initRTC()
{
    Wire.begin( DS1307_SDA, DS1307_SCL );
    #ifdef _CHIMERA_CORE_
      M5.I2C.scan();
    #endif
    // Initialize RTC and set as SyncProvider.
    // Later RTC will be synced with DCF time
    RTC.begin( DS1307_SDA, DS1307_SCL );
    //delay(10);
    setSyncProvider( RTCGet );   // the function to get the time from the RTC
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
}
void setRTC( unsigned long nowtime)
{
  takeMuxSemaphore();
  RTC.set( nowtime );
  giveMuxSemaphore();
  log_w( "RTC and internal clock adjusted to 20%02d:%02d:%02d %02d:%02d:00", dcfYear, dcfMonth, dcfDay, dcfHour, dcfMinute );
}
#endif


static void InitUI()
{
  tft.clear();
  tft.setRotation( displayRotation );

  tft.fillScreen( TFT_BLACKISH );

  //morphSprite();

  tft.fillScreen( TFT_BLACKISH );

  #if defined USE_PSRAM
    // only do this if your DCF77 is far away, this messes up with signal decoding
    if( psramInit() ) {
      sprite.setColorDepth( 16 );
      sprite.setPsram( true );
    }
  #endif

  uint16_t* spritePtr = (uint16_t*)sprite.createSprite( tft.width(), tft.height() );
  if( spritePtr == NULL ) {
    log_e("Unable to create 16bits spritePtr, will go with 8bits");
    sprite.setColorDepth( 8 );
    spritePtr = (uint16_t*)sprite.createSprite( tft.width(), tft.height() );
  } else {
    log_d("Successfully created spritePtr");
  }
  sprite.fillSprite( TFT_BLACK );
  uint16_t* logoSpritePtr = (uint16_t*)LogoSprite.createSprite( 32, 32 );
  if( logoSpritePtr == NULL ) {
    log_e("Unable to create logoSpritePtr");
  } else {
    log_d("Successfully created logoSpritePtr");
  }

  TFT_HALFWIDTH = tft.width() / 2;
  TFT_HALFHEIGHT = tft.height() / 2;

  uint16_t centericonposx = ( TFT_HALFWIDTH - 32 / 2 );
  uint16_t centericonposy = ( TFT_HALFHEIGHT - 32 / 2 );

  setFontStyle( &sprite, LedWeekStatusFontStyle );
  getTextBounds( &sprite, "W", &LedWeekStatusFontWidth, &LedWeekStatusFontHeight );

  setFontStyle( &sprite, LedErrorStatusFontStyle );
  getTextBounds( &sprite, "W", &LedErrorStatusFontWidth, &LedErrorStatusFontHeight );

  setFontStyle( &sprite, LedParityStatusFontStyle );
  getTextBounds( &sprite, "W", &LedParityStatusFontWidth, &LedParityStatusFontHeight );

  setFontStyle( &sprite, LedDCFStatusFontStyle );
  getTextBounds( &sprite, "W", &LedDCFStatusFontWidth, &LedDCFStatusFontHeight );

  setFontStyle( &sprite, ErrorsCountFontStyle );
  getTextBounds( &sprite, "W", &ErrorsCountFontWidth, &ErrorsCountFontHeight );

  setFontStyle( &sprite, RingLabelsFontStyle );
  getTextBounds( &sprite, "W", &RingLabelsFontWidth, &RingLabelsFontHeight );

  setFontStyle( &sprite, LedDisplayFontStyle );
  getTextBounds( &sprite, "W", &w, &LedDisplayFontHeight );

  setFontStyle( &sprite,  myRTCDateFontStyle );
  getTextBounds( &sprite, "W", &w, &RTCDateFontHeight );

  setFontStyle( &sprite, BoxSelected->style );
  getTextBounds( &sprite, "LEAP", &LEAPWidth, &h );
  getTextBounds( &sprite, "CET", &CETWidth, &h );
  getTextBounds( &sprite, "CEST", &CESTWidth, &h );
  CESTXpos = tft.width() - ( CESTWidth + 2 );
  CETXPos  = tft.width() - ( CETWidth + 2 + 2 + CESTWidth + 2 );
  LEAPXPos = tft.width() - ( LEAPWidth + 2 );

  #ifdef DCF77_DO_WEATHER
    InitPrefs();
  #endif

  LogoSprite.fillSprite( TFT_BLACKISH );

  setFontStyle( &sprite, LedParityStatusFontStyle );
  getTextBounds( &sprite, "011001010 P", &tmpFontWidth, &tmpFontHeight );
  sprite.fillRect( 0, tft.height() - (LedParityStatusFontHeight + 2), tmpFontWidth - LedParityStatusFontWidth, LedParityStatusFontHeight+1, TFT_DARKGRAY );
  sprite.fillRect( tmpFontWidth - LedParityStatusFontWidth, tft.height() - (LedParityStatusFontHeight + 2), LedParityStatusFontWidth + 2, LedParityStatusFontHeight+1, TFT_ORANGE);
  sprite.setTextColor( TFT_DARKGRAY );
  sprite.drawString( "011001010 P", 1, tft.height() - (LedParityStatusFontHeight + 1)  );
  #ifdef DCF77_DO_WEATHER
    // clear area from dummy text
    sprite.fillRect( 0, tft.height() - (LedParityStatusFontHeight + 2), tmpFontWidth - LedParityStatusFontWidth, LedParityStatusFontHeight+1, TFT_DARKGRAY );
  #endif

/*
  sprite.setCursor( 1, tft.height() - (LedParityStatusFontHeight + 1 ) );
  sprite.setTextColor( TFT_DARKGRAY, TFT_LIGHTGRAY );
  sprite.print("01100101 ");
  sprite.setTextColor( TFT_DARKGRAY, TFT_ORANGE );
  sprite.print("P");
*/

  LedParityStatus( 1, -1 ); // H
  LedParityStatus( 2, -1 ); // M
  LedParityStatus( 3, -1 ); // S
  LedDCFStatus( -1 );

  LedWeekStatus( LED_CEST, LOW );
  LedWeekStatus( LED_CET, LOW );
  LedWeekStatus( LED_LEAPYEAR, LOW );

  drawIcon( atomic, centericonposx, centericonposy );

  setRingCoords(); // cache ring coords
  drawRing();
  //drawWeekDays( -1 );

  sprite.pushSprite( 0, 0, TFT_BLACK );
}


void displayData( void )
{
  if( dcfWeekDay + weekNumber + dcfDay + dcfMonth + dcfYear + dcfDST + leapYear == 0 ) {
    // uh-oh
    log_e("Cowardly refusing to render with an empty data load (also zero is evil)");
    return;
  }
  // display Day of Week
  switch ( dcfWeekDay ) {
    case 1: LedWeekStatus( LED_MONDAY, HIGH ); break;
    case 2: LedWeekStatus( LED_TUESDAY, HIGH ); break;
    case 3: LedWeekStatus( LED_WEDNESDAY, HIGH ); break;
    case 4: LedWeekStatus( LED_THURSDAY, HIGH ); break;
    case 5: LedWeekStatus( LED_FRIDAY, HIGH ); break;
    case 6: LedWeekStatus( LED_SATURDAY, HIGH ); break;
    case 7: LedWeekStatus( LED_SUNDAY, HIGH ); break;
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
  Serial.printf( "DCF77 Timestamp : [20%02d-%02d-%02d %02d:%02d:%02d], DST: %d, leap: %d, weekDay: %d, weekNum: %d\n",
    dcfYear,
    dcfMonth,
    dcfDay,
    dcfHour,
    dcfMinute,
    0,
    dcfDST,
    leapYear,
    dcfWeekDay,
    weekNumber
  );
  #ifdef DCF77_DO_WEATHER
    LedWeatherStatus( dcfWeatherBitNum, dcfWeatherDecodeStatus );
  #endif
}


void LedTest()
{
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
  drawWeekDays( -1 );
  sprite.pushSprite( 0, 0, TFT_BLACK );

  #ifdef DCF77_DO_WEATHER
    for( int e=-1; e<2; e++ ) {
      for( int i=1; i<4; i++ ) {
        LedWeatherStatus( i%3, e, false );
        sprite.pushSprite( 0, 0, TFT_BLACK );
        delay(10);
      }
      delay(20);
    }
    LedWeatherStatus( 1, 0, false );
    sprite.pushSprite( 0, 0, TFT_BLACK );
  #endif


}


void initialize(void)
{

  M5Begin();
  Serial.printf("Started serial at %d bauds\n", SERIAL_SPEED);

  #if defined tftBrightness
    tft.setBrightness(tftBrightness);
  #endif

  //M5.NVS.Dump();
  //M5.NVS.Erase();

  InitPins();
  InitUI();

  // Reset DCFbitBuffer array, positions 0-59 (=60 bits)
  for ( int i = 0; i < 60; i++ ) {
    DCFbitBuffer[i] = 0;
    DCFbitFinalBuffer[i] = 0;
  }

  initSpeaker();
  #ifdef USE_RTC
    initRTC();
  #endif
  if ( PERFORM_LED_TEST == 1 ) {
    LedTest();
  }
  // activate errorCounter display after LED test
  LedDisplay( DisplayBufferBitError, "R", 0 );

  // reset timers
  leadingEdge         = millis();
  trailingEdge        = millis();
  previousLeadingEdge = millis()-1000;
  bufferPosition      = second()+1; // attempt to recover position from RTC

  // spawn tasks
  initTasks();
}


#endif // _UI_H_
