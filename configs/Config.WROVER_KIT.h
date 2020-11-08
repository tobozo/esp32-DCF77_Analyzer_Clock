#ifndef _CONFIG_H_WROVER_KIT
#define _CONFIG_H_WROVER_KIT

#define DCFDisplay M5Display
#include <ESP32-Chimera-Core.h> // https://github.com/tobozo/ESP32-Chimera-Core or regular M5Stack Core
#define tft M5.Lcd // syntax sugar

// editable values

#define UI_320x240
//#define UI_160x128
#define displayRotation 1
#define tftBrightness 255
#define DCFDisplay M5Display

#define FORCED_WATCHED_COUNTRY "France" // Is le pays des frogs
#define FORCED_WATCHED_CITY    "Paris"  // What a Seine-sational city!

// DCF77 from https://www.tindie.com/products/universalsolder/atomic-clock-am-receiver-kit-dcf77-wwvb-msf-jjy60/
#define DCF77_pdnPort  35 // optional, DCF77 power down port
#define DCF77_gainPort 34 // optional, DCF77 gain port
#define DCF77PIN       39 // input  - DCF signal from antenna pcb. Pin must be an interrupt input!
#define DCF77_DO_WEATHER  // handle weather data (comment this out to get rid of the text scroll and/or weather icons)

// RTC module (DS1307) support, optional but strongly recommended
#define DS1307_SDA     26
#define DS1307_SCL     27
#define DS1307_CTRL_ID 0x68

#define USE_RTC
//#define USE_BUTTONS
#undef SPEAKER_PIN // disable speaker

// output baudrate to serial console
#define SERIAL_SPEED 115200

// some named tones to customize beep sounds (for reference)

// tone off
#define NOTE_D0  -1
// lower octave
#define NOTE_DL1 147
#define NOTE_DL2 165
#define NOTE_DL3 175
#define NOTE_DL4 196
#define NOTE_DL5 221
#define NOTE_DL6 248
#define NOTE_DL7 278
// middle octave
#define NOTE_D1  294
#define NOTE_D2  330
#define NOTE_D3  350
#define NOTE_D4  393
#define NOTE_D5  441
#define NOTE_D6  495
#define NOTE_D7  556
// higher octave
#define NOTE_DH1 589
#define NOTE_DH2 661
#define NOTE_DH3 700
#define NOTE_DH4 786
#define NOTE_DH5 882
#define NOTE_DH6 990
#define NOTE_DH7 1023 // was 112 ?

// actual tones (can be edited)
#define BEEPNOTE_BIT_HIGH NOTE_D5  // when the signal bit=1
#define BEEPNOTE_BIT_LOW  NOTE_D3  // when the signal bit=0
#define BEEPNOTE_ERROR    NOTE_DL1 // when an signal bit decoding error occurs

#endif // _CONFIG_H_WROVER_KIT
