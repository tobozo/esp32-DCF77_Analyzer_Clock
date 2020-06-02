/*


  ESP32 DCF77 Analyzer / Clock
  A TFT interpretation of the original led-based project by tobozo - August 2019
  Source: https://github.com/tobozo/esp32-DCF77_Analyzer_Clock/

  MIT License

  Copyright (c) 2018 tobozo

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

  -----------------------------------------------------------------------------

  Hardware Requirements:
    - ESP32
    - TFT (160*128 minimum recommended, working on elastic design to cover higher resolutions)
    - DCF77 Atomic clock module ( such as this one https://www.tindie.com/products/universalsolder/atomic-clock-am-receiver-kit-dcf77-wwvb-msf-jjy60/ )
    - RTC Module (preferably i2c, e,g, a DS1307)
    - Buttons (optional)
    - Speaker (optional)

  Software Requirements:
    - M5Stack Core or ESP32-Chimera-Core for compatibility https://github.com/tobozo/ESP32-Chimera-Core (replaces M5Stack Core, adds support for Odroid-Go, TTGO-TS, Wrover-Kit LoLin D-32Pro)
    - Bodmer's JPEGDecoder for rendering jpeg in sprites https://github.com/Bodmer/JPEGDecoder

  CREDITS:
    I learned a lot from Erik de Ruiter who learned a lot from the work of Matthias Dalheimer and Thijs Elenbaas
    who made their own DCF77 decoders.
    Although the changes I made are far from optimistic, this code has wisedom DNA in its roots!
    Without the incredible work of these geniuses I would not have known where to start and how to write those credits :)
    I ended up writing my own code (using bits and pieces of their ideas) so I could understand what is happening...
    My code is far from efficient or advanced but it does work and I know what is going on.

  Author:
    - https://github.com/tobozo/
    - https://twitter.com/tobozotagada/
    - https://www.youtube.com/c/tobozotagada

  Interesting websites:
    - Inspirational video     : https://www.youtube.com/watch?v=ZadSU_DT-Ks
    - UI code based on        : https://github.com/deruiter/DCF77-Analyzer-Clock-V2.0
    - Crypto code based on    : https://github.com/FroggySoft/AlarmClock
    - Weather code based on   : http://arduino-projects4u.com/home-weather-station/
    - Erik de Ruiter          : https://www.hackster.io/edr1924
    - Brett Oliver            : http://www.brettoliver.org.uk/
    - Oliver's clocks code    : https://github.com/brettoliver
    - Joop Tap                : http://www.jooptap.nl
    - Thijs Ellenbaas         : http://thijs.elenbaas.net/2012/04/arduino-dcf77-radio-clock-receiver-hardware-2/
    - Mathias Dalheimer       : https://github.com/roddi/DCF77-Arduino/blob/master/DCF77Servoclock/DCF77.h
    - DCF77 wikipedia         : https://en.wikipedia.org/wiki/DCF77
    - Much more DCF77 info    : http://www.picbasic.nl/indexes_uk.htm
    - Erik de Ruiter's Flickr : https://www.flickr.com/photos/edr1924/albums
    - Erik de Ruiter's Github : https://github.com/deruiter
    - FroggySoft's Github     : https://github.com/FroggySoft

*/


#include "Config.h"
#include "UI.h" // TFT Support for Clock Analyzer + JPEG/Adafruit layer

void loop() {
  vTaskSuspend( NULL );
}

void setup() {
  initialize();
}
