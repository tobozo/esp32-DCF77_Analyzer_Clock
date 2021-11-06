// Contributed by dl9sec 06.11.2021 (https://github.com/dl9sec)
// Licensed under the MIT license

#ifndef _CONFIG_H_OGO
#define _CONFIG_H_OGO

#define DCFDisplay M5Display
#include <ESP32-Chimera-Core.h> // https://github.com/tobozo/ESP32-Chimera-Core or regular M5Stack Core
#define tft M5.Lcd // syntax sugar

// editable values

#define UI_320x240
#define displayRotation 1
#define tftBrightness 255
#define DCFDisplay M5Display

//#define FORCED_WATCHED_COUNTRY "Germany"
//#define FORCED_WATCHED_CITY    "Stuttgart"

// DCF77 from https://www.universal-solder.ca/product/canaduino-60khz-atomic-clock-receiver-module-wwvb-msf-jjy60/ (WWVB/JJY60/MSF/DCF77)
// or https://www.pollin.de/p/dcf-77-empfangsmodul-dcf1-810054 (DCF77)
#define DCF77PIN       4  // input  - DCF signal from antenna pcb. Pin must be an interrupt input!
#define DCF77_DO_WEATHER  // handle weather data (comment this out to get rid of the text scroll and/or weather icons)

// RTC module (DS1307 or DS3231) support, optional but strongly recommended
#define DS1307_SDA     15
#define DS1307_SCL     12
#define DS1307_CTRL_ID 0x68

#define USE_RTC
#define USE_BUTTONS
#define USE_SPEAKER
//#undef SPEAKER_PIN // disable speaker

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

#endif // _CONFIG_H_OGO
