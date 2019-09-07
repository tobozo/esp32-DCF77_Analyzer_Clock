#include "Assets.h"

#define float_r1 50.0 // inner ring
#define float_r2 60.0 // outer ring
#define float_r3 44.0 // inner 10's mark
#define float_r4 38.0 // inner 10's label*/
#define int_ringLedRadius 2

#define RTCTimeYPos 38
#define RTCDateYPos 88

// alias/declare some of font we'll use
static const GFXfont *RingLabelsFont = &Picopixel;
static const GFXfont *ErrorsCountFont = &Picopixel;
int myRTCTimeFont = 1; //static const GFXfont *myRTCTimeFont = &FreeMono9pt7b;
static const GFXfont *myRTCDateFont = &Picopixel;
static const GFXfont *LedDCFStatusFont = &Picopixel;
static const GFXfont *LedParityStatusFont = &Picopixel;
static const GFXfont *LedErrorStatusFont = &Picopixel;
static const GFXfont *LedWeekStatusFont = &Picopixel;

#define int_scrollFontSize 4 // font size
#define int_scrollFontSizeSmall 2 // font size
int BufferFont = 1; // font number
int LedDisplayFont = 1; // font number

static uint16_t CETCESTYPos      = 115;
static uint16_t LeapYearYpos     = 109;
static uint16_t WeekNumberYpos   = 93;
static uint16_t WeekNumberXpos   = 143;
static uint16_t WeekDayNamesXpos = 127;
static uint16_t WeekDayNamesYpos = 125;
static uint16_t WeekDayLedYpos   = 117;

#define displayRotation 0

#define scrollSpeed 1 // pixels offset by scroll cycle

#undef ST7735_YELLOW
#undef ST7735_RED
#undef ST7735_BLUE

#define ST7735_GREEN     M5.Lcd.color565( 0x00, 0xff, 0x00 )
#define ST7735_BLACK     M5.Lcd.color565( 0x00, 0x00, 0x00 )
#define ST7735_BLACKISH  M5.Lcd.color565( 0x0f, 0x0f, 0x0f )
#define ST7735_WHITE     M5.Lcd.color565( 0xff, 0xff, 0xff )
#define ST7735_UGRAY     M5.Lcd.color565( 0xdd, 0xdd, 0xdd ) // 87%
#define ST7735_LIGHTGRAY M5.Lcd.color565( 0x88, 0x88, 0x88 ) // 75%
#define ST7735_GRAY      M5.Lcd.color565( 0x44, 0x44, 0x44 ) // 25%
#define ST7735_DARKGRAY  M5.Lcd.color565( 0x22, 0x22, 0x22 ) // 12%
#define ST7735_YELLOW    M5.Lcd.color565( 0xff, 0xff, 0x00 )
#define ST7735_RED       M5.Lcd.color565( 0xff, 0x00, 0x00 )
#define ST7735_BLUE      M5.Lcd.color565( 0x00, 0x00, 0xff )
#define ST7735_BLUEGRAY  M5.Lcd.color565( 0x22, 0x22, 0x44 )
#define ST7735_ORANGE    M5.Lcd.color565( 0xff, 0xa5, 0x00 ) // #FFA500
#define ST7735_PURPLE    M5.Lcd.color565( 0xee, 0x82, 0xee ) // 
