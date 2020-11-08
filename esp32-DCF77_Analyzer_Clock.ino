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

  Improvements by Brett Oliver (http://www.brettoliver.org.uk/):

    - Analyzer output works on 2.2" & 2.8" Touch ILI9341 TFT via a Lolin D32 Pro v2 board.
    - Serial ouput Modified to work on a TFT serial monitor
    - working version 01
    - Test version 02
      Brett Oliver's notes
      Had to add #include <TimeLib.h> to DS1307RTC.h tab to prevent complile errors
      Complies LOLIN D32 PRO and TTGO T1

      Changed ESP32 to transmit serial at 9600 to work better on my serial monitor.
      config tab #define SERIAL_SPEED 9600

      Have changed UI.h
      panel.panel_width to 240 from 320 and panel.panel_height to 320 from 240(prob the wrong place to change it ran out of time must be a rotation setting somewhere?)
    - v3
    - v4
      Timestamp-DCF77.h line 340 timestamp evey complete minute decode
      DCF77_Weather Line 1562 Brett2
      from Serial.println("City match, should update scroll and icons !!");
      to  Serial.println("City match, weather icons on");//Brett2
      DCF77_Weather Line 1566
      from Serial.println("Forecast for this country+city will not be displayed");
      to Serial.println("No City match, weather icons off");//Brett3
      DCF77_Weather H line 1534
      from  sprintf( str, "twoday f/c day = %d ", (((dcfHour - 21) * 60 + dcfMinute) > 90) ? 2 : 1); Serial.println( str );
      to sprintf( str, "twoday f/c day = %d ", (((dcfHour - 23) * 60 + dcfMinute) > 90) ? 2 : 1); Serial.println( str );
      Change Great Britain to UK
      Modify Serial output for TFT serial monitor display
    - v6
      UI.h added lin to show time every minute of Meteo Data decode
      DCFweather line 101 changed clouded to cloudy

      374  Serial.println("Minute Parity P1 Pass");//Brett
      383 Serial.println("Minute Parity P1 Fail");//Brett
      400 Serial.println("Hour Parity P2 Pass");//Brett
      409 Serial.println("Hour Parity P2 Fail");//Brett
      428 Serial.println("Date Parity P3 Pass");//Brett
      437 Serial.println("Date Parity P3 Fail");//Brett
      464 Serial.println("Error Buffer Overflow");//Brett
      501 Serial.println("DCF77 Time decode correct");//Brett


    - v7 change fonts U320x240 font styles changed from picopixels
    - v8 miss out
    - v9 adding DCF77 decode progress to serial terminal  DCFWeather.H  Serial.println("Received Meteo Data 1-14"); etc
      remove serial.print sig weather DCF77 weather.h void printPrefsCache()
      line 565 UI.h //Brett add space between buffer number and DCF bit value
      line 589 UI.h // Brett creates space between period time and pulse indicators on TFT
      line 89 etc UI320 240 remove hi-light from Week no,LEAP, CEST and CET to make txt green if on and black if off
    - v10 corrected weekday text //Brett UI.h 795 move weekday text left right -x   down and up -y
    - v11 adding switches and DCFWEATHER line 1605 set watched city manually
      //Brett DCF Weather.h line 1597 manual setting of watched city
      Serial.print("Watched city detected ");//Brett
        Serial.println(watchedCity);//Brett
      Serial.print("Watched city is set to ");//Brett
        Serial.println(watchedCity);//Brett

      v10 add watched location to TFT
      DCF77 weather.h  line 1621 prints watchewd city name on the TFT
      v11 add println to day critical waether DCW etc DCF77weather.h line 1511 etc
      continue changing location of watched location DCF77 weather.h  line 1621 prints watchewd city name on the TFT
    - v12 parity M S D re-positioned
    - v13  21:00 & 22:00 hr crash fix ( Modified nowweather()) working
    - v14 etc miss
    - v15 to stop crash on receipt of all 42 bits weather data (some LOLIN D32 Pros only)
      comment out this line in DCF77.h => setForecast( forecastID, forecastHour, forecastMinute, aInfo );
    - v16 void
    - v17/v18 added receive bits info to TFT display to show what data is being received
        added after line203 task.h
    - v19 add leap second detection not working yet
      if leapSec == 1,sec == 59 ,min == 59 buffer ==59
      in DCF77.h 366
      dcfDST     = bitDecode( DCFbitFinalBuffer, 17, 17 ); checks for summertime

    - v20 continue leap second detection leap second works but does not show 60 seconds.
    - v21 miss out
    - v22 miss out
    - v23 continue leap second does not show 60 sec on leap second RTC issue?
    - v24 leap year clean up and display clean up of leap second bits
    - v25 display leap second inserted on TFT
    - v26 record Leap Sec time and date on TFT

*/

#include "UI.h" // TFT Support for Clock Analyzer + JPEG/LGFX layer

void loop() {
  vTaskSuspend( NULL );
}

void setup() {
  initialize();
}
