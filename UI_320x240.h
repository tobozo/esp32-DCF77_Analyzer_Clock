#define float_r1 90.0 // inner ring
#define float_r2 112.0 // outer ring
#define float_r3 78.0 // inner 10's mark
#define float_r4 66.0 // inner 10's label*/
#define int_ringLedRadius 4

#define RTCTimeYPos 72
#define RTCDateYPos 150

// alias/declare some of font we'll use
int RingLabelsFont = 2; //static const GFXfont *RingLabelsFont = &FreeMono9pt7b;
static const GFXfont *ErrorsCountFont = &FreeMono9pt7b;
static const GFXfont *myRTCTimeFont = &FreeMono9pt7b;
static const GFXfont *myRTCDateFont = &FreeMono9pt7b;
int LedDCFStatusFont = 2;
int LedParityStatusFont = 2; //static const GFXfont *LedParityStatusFont = &FreeMono9pt7b;
int LedErrorStatusFont = 2;
int LedWeekStatusFont = 2; //static const GFXfont *LedWeekStatusFont = &FreeMono9pt7b;

#define int_scrollFontSize 4 // font size
#define int_scrollFontSizeSmall 4 // font size
int BufferFont = 2; // font number
int LedDisplayFont = 2; // font number

static uint16_t CETCESTYPos      = 209;
static uint16_t LeapYearYpos     = 192;
static uint16_t WeekNumberYpos   = 158;
//static uint16_t WeekNumberXpos   = 303;
static uint16_t WeekDayNamesXpos = 259;
static uint16_t WeekDayNamesYpos = 224;
static uint16_t WeekDayLedYpos   = 216;

#define displayRotation 1

#define scrollSpeed 2 // pixels offset by scroll cycle

// some extra named colors
#define TFT_BLACKISH  M5.Lcd.color565( 0x0f, 0x0f, 0x0f )
#define TFT_UGRAY     M5.Lcd.color565( 0xdd, 0xdd, 0xdd ) // 87%
#define TFT_LIGHTGRAY M5.Lcd.color565( 0x88, 0x88, 0x88 ) // 75%
#define TFT_GRAY      M5.Lcd.color565( 0x44, 0x44, 0x44 ) // 25%
#define TFT_DARKGRAY  M5.Lcd.color565( 0x22, 0x22, 0x22 ) // 12%
#define TFT_BLUEGRAY  M5.Lcd.color565( 0x22, 0x22, 0x44 )
