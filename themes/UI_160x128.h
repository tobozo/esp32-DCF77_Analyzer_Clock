#define float_r1 50.0 // inner ring
#define float_r2 60.0 // outer ring
#define float_r3 44.0 // inner 10's mark
#define float_r4 38.0 // inner 10's label*/
#define int_ringLedRadius 2
#define RTCTimeYPos 38
#define RTCDateYPos 88
#define scrollSpeed 1 // pixels offset by scroll cycle
// some extra named colors
#define TFT_BLACKISH  tft.color565( 0x0f, 0x0f, 0x0f )
#define TFT_UGRAY     tft.color565( 0xdd, 0xdd, 0xdd ) // 87%
#define TFT_LIGHTGRAY tft.color565( 0x88, 0x88, 0x88 ) // 75%
#define TFT_GRAY      tft.color565( 0x44, 0x44, 0x44 ) // 25%
#define TFT_DARKGRAY  tft.color565( 0x22, 0x22, 0x22 ) // 12%
#define TFT_BLUEGRAY  tft.color565( 0x22, 0x22, 0x44 )
#define TFT_DARKRED   tft.color565( 0xBB, 0x00, 0x00 )
#define TFT_GREENISH  tft.color565( 0x00, 0xdd, 0x00 )

//static uint16_t DCFBitBufferYPos = 8;
static uint16_t CETCESTYPos      = 118;
static uint16_t LeapYearYpos     = 109;
static uint16_t WeekNumberYpos   = 93;
//static uint16_t WeekDayNamesXpos = 127;
//static uint16_t WeekDayNamesYpos = 122;
//static uint16_t WeekDayLedYpos   = 117;
static uint16_t MHDXPos[3]       = {0, 16, 26};
static int16_t LedWeekStatusMarginX = -2;
static int16_t LedWeekStatusMarginY = 9;

const char* TIME_FORMAT = "%02d:%02d:%02d";
const char* DATE_FORMAT = "%04d-%02d-%02d";

// text styles
TextStyle *myRTCDateTimeTextStyleOK = new TextStyle( 0xccffccU, 0x222222U, 1, 1, middle_center, false,  true );
TextStyle *myRTCDateTimeTextStyleKO = new TextStyle( 0xff8888U, 0x222222U, 1, 1, middle_center, false,  true );
TextStyle *myRTCDateTimeTextStyleNA = new TextStyle( 0xddddddU, 0x222222U, 1, 1, middle_center, false,  true );
TextStyle *RingLabelsTextStyle      = new TextStyle( 0x888888U, 0x222222U, 1, 1, middle_center, false,  true );
TextStyle *ErrorsCountStyleOK       = new TextStyle( 0x00ff00U, 0x222222U, 1, 1, top_left, false, true );
TextStyle *ErrorsCountStyleKO       = new TextStyle( 0xff0000U, 0x222222U, 1, 1, top_left, false, true );
TextStyle *LedDCFStatusStyleOK      = new TextStyle( 0x00ff00U, 0x222222U, 1, 1, top_left, false, true );
TextStyle *LedDCFStatusStyleKO      = new TextStyle( 0xff0000U, 0x222222U, 1, 1, top_left, false, true );
TextStyle *LedParityStatusStyleNA   = new TextStyle( 0x888888U, 0x222222U, 1, 1, top_left, false, true );
TextStyle *LedParityStatusStyleOK   = new TextStyle( 0x00ff00U, 0x222222U, 1, 1, top_left, false, true );
TextStyle *LedParityStatusStyleKO   = new TextStyle( 0xff0000U, 0x222222U, 1, 1, top_left, false, true );
TextStyle *LedErrorStatusStyleGray  = new TextStyle( 0x444444U, 0x222222U, 1, 1, top_left, false, true );
TextStyle *LedErrorStatusStyleRed   = new TextStyle( 0xff0000U, 0x222222U, 1, 1, top_left, false, true );
TextStyle *LedErrorStatusStyleGreen = new TextStyle( 0x00ff00U, 0x222222U, 1, 1, top_left, false, true );
TextStyle *LedWeekStatusStyle       = new TextStyle( 0x888888U, 0xffffffU, 1, 1, middle_center, false, true );
TextStyle *LedDisplayStylePingtime  = new TextStyle( 0xffa500U, 0x222222U, 1, 1, top_left, false, true );
TextStyle *LedDisplayStyleLeftover  = new TextStyle( 0xffff00U, 0x222222U, 1, 1, top_left, false, true );
TextStyle *myTimerStyle             = new TextStyle( 0xddddddU, 0x222222U, 1, 1, middle_center, false, true );
TextStyle *scrollStyleEnabled       = new TextStyle( 0x444444U, 0x0f0f0fU, 1, 1, middle_center, false, true );
TextStyle *scrollStyleDisabled      = new TextStyle( 0x222222U, 0x0f0f0fU, 1, 1, middle_center, false, true );

// text styles aliases
TextStyle *myRTCDateTimeTextStyle   = myRTCDateTimeTextStyleNA;
TextStyle *ErrorsCountStyle         = ErrorsCountStyleOK;
TextStyle *LedDCFStatusStyle        = LedDCFStatusStyleOK;
TextStyle *LedParityStatusStyle     = LedParityStatusStyleNA;
TextStyle *LedErrorStatusStyle      = LedErrorStatusStyleGray;
TextStyle *LedDisplayStyle          = LedDisplayStylePingtime;
TextStyle *scrollStyle              = scrollStyleDisabled;

// font styles
FontStyle *myRTCDateFontStyle       = new FontStyle( &fonts::Font0, myRTCDateTimeTextStyle ); //
FontStyle *myRTCTimeFontStyle       = new FontStyle( &fonts::Font0, myRTCDateTimeTextStyle ); //
FontStyle *RingLabelsFontStyle      = new FontStyle( &Picopixel,    RingLabelsTextStyle );
FontStyle *ErrorsCountFontStyle     = new FontStyle( &Picopixel,    ErrorsCountStyle );
FontStyle *LedDCFStatusFontStyle    = new FontStyle( &Picopixel,    LedDCFStatusStyle );
FontStyle *LedParityStatusFontStyle = new FontStyle( &Picopixel,    LedParityStatusStyle );
FontStyle *LedErrorStatusFontStyle  = new FontStyle( &Picopixel,    LedErrorStatusStyle ); //
FontStyle *LedWeekStatusFontStyle   = new FontStyle( &Picopixel,    LedWeekStatusStyle );
FontStyle *LedDisplayFontStyle      = new FontStyle( &Picopixel,    LedDisplayStyle );
FontStyle *myTimerFontStyle         = new FontStyle( &fonts::Font0, myTimerStyle );
FontStyle *scrollFontStyle          = new FontStyle( &fonts::Font2, scrollStyle );

// for weeknum/leap/cet/cest boxes
TextStyle *BoxStyleEnabled          = new TextStyle( 0x888888U, 0xffffffU, 1, 1, top_left, false, true );
TextStyle *BoxStyleDisabled         = new TextStyle( 0x222222U, 0x888888U, 1, 1, top_left, false, true );

FontStyle *BoxFontStylePicoEnabled  = new FontStyle( &Picopixel, BoxStyleEnabled );
FontStyle *BoxFontStylePicoDisabled = new FontStyle( &Picopixel, BoxStyleDisabled );

BoxStyle  *BoxSelected              = new BoxStyle( BoxStyleEnabled->back_rgb888,  BoxFontStylePicoEnabled );
BoxStyle  *BoxUnSelected            = new BoxStyle( BoxStyleDisabled->back_rgb888, BoxFontStylePicoDisabled );


TextStyle *CountryCityStyleEnabled       = new TextStyle( 0x888888U, 0xffffffU, 1, 1, top_left, false, true );
TextStyle *CountryCityStyleDisabled      = new TextStyle( 0x222222U, 0x888888U, 1, 1, top_left, false, true );
FontStyle *CountryCityFontStylePicoEnabled  = new FontStyle( &fonts::Font0, CountryCityStyleEnabled );
FontStyle *CountryCityFontStylePicoDisabled = new FontStyle( &fonts::Font0, CountryCityStyleDisabled );

TextStyle *StatusTextStyle = new TextStyle( 0xccffccU, 0x000000U, 1, 1, middle_center, false, true );
FontStyle *StatusFontStyle = new FontStyle( &fonts::Font0, StatusTextStyle );



/// experm


void setTextStyle( M5Display *_tft, TextStyle *style );
void setFontStyle( M5Display *_tft, FontStyle *myFontStyle );
void setTextStyle( LGFX_Sprite *sprite, TextStyle *style );
void setFontStyle( LGFX_Sprite *sprite, FontStyle *myFontStyle );


static const lgfx::U8g2font emoticon16( u8g2_font_unifont_t_emoticons );
static const lgfx::U8g2font meteo16( u8g2_font_unifont_t_cards );

TextStyle *EmoticonTextStyle = new TextStyle( 0xccffccU, 0x000000U, 2, 2, top_left, false, true );
TextStyle *MeteoTextStyle    = new TextStyle( 0xccffccU, 0x222222U, 2, 2, middle_center, false, true );

FontStyle *EmoticonFontStyle = new FontStyle( &emoticon16, EmoticonTextStyle );
FontStyle *MeteoFontStyle    = new FontStyle( &meteo16,    MeteoTextStyle );


#define COORDS_MAX 1024
uint8_t src[COORDS_MAX*2];
uint8_t dst[COORDS_MAX*2];
int numSrc = 0;
int numDst = 0;
int tt = 0;
int mode = 0;

LGFX_Sprite morphSpriteFrom = LGFX_Sprite( &tft );
LGFX_Sprite morphSpriteTo   = LGFX_Sprite( &tft );

void morph(LGFX_Sprite *sprite, int32_t _x, int32_t _y, int mode )
{
  int i,i0,i1,x,y;
  int n = max(numSrc,numDst);
  for(int i=0;i<n;i++) {
    i0 = (numSrc<n) ? i*numSrc/n : i;
    i1 = (numDst<n) ? i*numDst/n : i;
    if(mode&1) i1 = numDst-1-i1;
    x = src[i0*2]   + ( dst[i1*2]   - src[i0*2]   ) *tt/sprite->height();
    y = src[i0*2+1] + ( dst[i1*2+1] - src[i0*2+1] ) *tt/sprite->height();
    sprite->drawPixel( x, y , TFT_WHITE );
  }
  sprite->pushSprite( _x, _y );
  sprite->fillSprite( TFT_BLACK );
}




int genSpritePoints( byte *tab, int max, LGFX_Sprite *sprite, char symbol )
{
  char str[2] = {symbol,'\0'};
  int pixelsOn = 0;
  for( int y=0;y<sprite->height();y++) {
    for( int x=0;x<sprite->width();x++) {
      uint16_t pixcolor = sprite->readPixel( x, y );
      if( pixcolor != TFT_BLACK ) {
        tab[pixelsOn*2+0] = x;
        tab[pixelsOn*2+1] = y;//b+j*8;
        if(++pixelsOn>max) { Serial.printf("Size too big (%d coords) for: %s\n", pixelsOn, str); return pixelsOn; }
      }
    }
  }
  return pixelsOn;
}



void morphLoop( LGFX_Sprite *sprite, int32_t x, int32_t y, FontStyle *fstyle, char morphFrom, char morphTo )
{

  morphSpriteFrom.setColorDepth( 16 );
  morphSpriteTo.setColorDepth( 16 );

  morphSpriteFrom.createSprite( sprite->width(), sprite->height() );
  morphSpriteTo.createSprite(   sprite->width(), sprite->height() );

  char str1[2] = {morphFrom,'\0'};
  char str2[2] = {morphTo,'\0'};

  setFontStyle( &morphSpriteFrom, fstyle );
  morphSpriteFrom.fillSprite( TFT_BLACK );
  morphSpriteFrom.setTextColor( TFT_WHITE, TFT_BLACK );
  morphSpriteFrom.setTextDatum( TL_DATUM );
  morphSpriteFrom.drawString( str1, 0, 0 );

  setFontStyle( &morphSpriteTo, fstyle );
  morphSpriteTo.fillSprite( TFT_BLACK );
  morphSpriteTo.setTextColor( TFT_WHITE, TFT_BLACK );
  morphSpriteTo.setTextDatum( TL_DATUM );
  morphSpriteTo.drawString( str2, 0, 0 );

  for(tt=0;tt<=sprite->height();tt++) {
    unsigned long morphStart = millis();
    numSrc = genSpritePoints( src, COORDS_MAX, &morphSpriteFrom, morphTo );
    numDst = genSpritePoints( dst, COORDS_MAX, &morphSpriteTo,   morphFrom );
    morph( sprite, x, y, mode );
    while(millis()-morphStart<5) {}
  }
  // mode=(mode+1)&3;
  morphSpriteFrom.deleteSprite();
  morphSpriteTo.deleteSprite();
}


void morphSprite()
{

  LGFX_Sprite morphSprite = LGFX_Sprite( &tft );

  morphSprite.setColorDepth( 16 );
  morphSprite.createSprite( 32, 32 );
  morphSprite.fillSprite( TFT_BLACK );

  char oldSymbol = 0x20;

  for(uint8_t i = 0x20; i<0x90; i++ ) {
    char newSymbol = i;
    char symbol[2] = {i, '\0'};
    setFontStyle( &tft, myRTCDateFontStyle );
    tft.setCursor( 32, 32 );
    tft.printf("0x%02X: ", i );
    morphLoop( &morphSprite, tft.width()/2, tft.height()/2, EmoticonFontStyle, oldSymbol, newSymbol );
    oldSymbol = newSymbol;
    delay( 200 );
  }

  morphSprite.deleteSprite();

  delay( 5000 );
}


