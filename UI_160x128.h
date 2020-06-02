#define float_r1 50.0 // inner ring
#define float_r2 60.0 // outer ring
#define float_r3 44.0 // inner 10's mark
#define float_r4 38.0 // inner 10's label*/
#define int_ringLedRadius 2
#define RTCTimeYPos 38
#define RTCDateYPos 88
#define displayRotation 1
#define scrollSpeed 1 // pixels offset by scroll cycle
// some extra named colors
#define TFT_BLACKISH  tft.color565( 0x0f, 0x0f, 0x0f )
#define TFT_UGRAY     tft.color565( 0xdd, 0xdd, 0xdd ) // 87%
#define TFT_LIGHTGRAY tft.color565( 0x88, 0x88, 0x88 ) // 75%
#define TFT_GRAY      tft.color565( 0x44, 0x44, 0x44 ) // 25%
#define TFT_DARKGRAY  tft.color565( 0x22, 0x22, 0x22 ) // 12%
#define TFT_BLUEGRAY  tft.color565( 0x22, 0x22, 0x44 )

static uint16_t DCFBitBufferYPos = 8;
static uint16_t CETCESTYPos      = 118;
static uint16_t LeapYearYpos     = 109;
static uint16_t WeekNumberYpos   = 93;
static uint16_t WeekDayNamesXpos = 127;
static uint16_t WeekDayNamesYpos = 122;
static uint16_t WeekDayLedYpos   = 117;

// text styles
TextStyle *myRTCDateTimeTextStyleOK = new TextStyle( 0xccffccU, 0x222222U, 1, 1, middle_center, false,  true );
TextStyle *myRTCDateTimeTextStyleKO = new TextStyle( 0xff8888U, 0x222222U, 1, 1, middle_center, false,  true );
TextStyle *myRTCDateTimeTextStyleNA = new TextStyle( 0xddddddU, 0x222222U, 1, 1, middle_center, false,  true );
TextStyle *RingLabelsTextStyle      = new TextStyle( 0x444444U, 0x222222U, 1, 1, middle_center, false,  true );
TextStyle *ErrorsCountStyleOK       = new TextStyle( 0x00ff00U, 0x222222U, 1, 1, top_left, false,  true );
TextStyle *ErrorsCountStyleKO       = new TextStyle( 0xff0000U, 0x222222U, 1, 1, top_left, false,  true );
TextStyle *LedDCFStatusStyleOK      = new TextStyle( 0x00ff00U, 0x222222U, 1, 1, top_left, false,  true );
TextStyle *LedDCFStatusStyleKO      = new TextStyle( 0xff0000U, 0x222222U, 1, 1, top_left, false,  true );
TextStyle *LedParityStatusStyleNA   = new TextStyle( 0x888888U, 0x222222U, 1, 1, top_left, false,  true );
TextStyle *LedParityStatusStyleOK   = new TextStyle( 0x00ff00U, 0x222222U, 1, 1, top_left, false,  true );
TextStyle *LedParityStatusStyleKO   = new TextStyle( 0xff0000U, 0x222222U, 1, 1, top_left, false,  true );
TextStyle *LedErrorStatusStyleGray  = new TextStyle( 0x444444U, 0x222222U, 1, 1, top_left, false,  true );
TextStyle *LedErrorStatusStyleRed   = new TextStyle( 0xff0000U, 0x222222U, 1, 1, top_left, false,  true );
TextStyle *LedErrorStatusStyleGreen = new TextStyle( 0x00ff00U, 0x222222U, 1, 1, top_left, false,  true );
TextStyle *LedWeekStatusStyle       = new TextStyle( 0x888888U, 0xffffffU, 1, 1, top_left, false, true );
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
FontStyle *scrollFontStyle           = new FontStyle( &fonts::Font2, scrollStyle );
