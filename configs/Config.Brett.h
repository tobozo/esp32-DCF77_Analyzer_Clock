#ifndef _CONFIG_H_BRETT
#define _CONFIG_H_BRETT

#define LGFX_ONLY
#include <LovyanGFX.hpp>
#include <Wire.h>
#define DCFDisplay LGFX

// editable values
#define UI_320x240
//#define UI_160x128
#define displayRotation 1
#define tftBrightness 255

struct LGFX_Config
{
  // values picked from /LovyanGFX/src/config/
  static constexpr spi_host_device_t spi_host = VSPI_HOST;
  static constexpr int dma_channel = 1;
  static constexpr int spi_mosi    = 23;
  static constexpr int spi_miso    = 19;
  static constexpr int spi_sclk    =  18;
};

static lgfx::Panel_ILI9341 panel; // panel name picked from /LovyanGFX/src/lgfx/panel/
static lgfx::LGFX_SPI<LGFX_Config> tft;

static void setDisplayPins() {
  panel.freq_write      = 40000000;
  panel.panel_width     = 240;
  panel.panel_height    = 320;
  panel.offset_x        = 0;
  panel.offset_y        = 0;
  panel.offset_rotation = 0;
  panel.rgb_order       = false;
  panel.spi_3wire       = false;
  panel.spi_cs          = 14;
  panel.spi_dc          = 27;
  panel.gpio_rst        = 33;
  panel.gpio_bl         = 32;
  panel.pwm_ch_bl       = 7;
}
#include <LGFX_TFT_eSPI.h>

#define FORCED_WATCHED_COUNTRY "United Kingdom" // Is not Great Britain
#define FORCED_WATCHED_CITY    "London"         // Fish, chips, cup 'o tea, bad food, worse weather, Mary fucking Poppins… LONDON

// DCF77 from https://www.tindie.com/products/universalsolder/atomic-clock-am-receiver-kit-dcf77-wwvb-msf-jjy60/
//#define DCF77_pdnPort  26 // optional, DCF77 power down port
//#define DCF77_gainPort 32 // optional, DCF77 gain port
#define DCF77PIN       13 // input  - DCF signal from antenna pcb. Pin must be an interrupt input!
#define DCF77_DO_WEATHER  // handle weather data (comment this out to get rid of the text scroll and/or weather icons)

// RTC module (DS1307) support, optional but strongly recommended
#define DS1307_SDA    21
#define DS1307_SCL    22
#define DS1307_CTRL_ID 0x68

#define USE_RTC
// #define USE_BUTTONS
// define USE_SPEAKER
// #undef SPEAKER_PIN // disable speaker (Chimera-Core only)

// output baudrate to serial console
#define SERIAL_SPEED 115200 //9600 DCF77 Analyzer  115200 DCF77 Generator

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

#endif // _CONFIG_H_BRETT
