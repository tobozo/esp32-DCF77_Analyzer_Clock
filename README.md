# esp32-DCF77_Analyzer_Clock
An ESP32 TFT interpretation of the @deruiter's [DCF77-Analyzer-Clock-V2.0](https://github.com/deruiter/DCF77-Analyzer-Clock-V2.0)


[![image](https://raw.githubusercontent.com/tobozo/esp32-DCF77_Analyzer_Clock/master/img/dcf77-demo.gif)](https://twitter.com/TobozoTagada/status/1163339957538344960?s=20)



Hardware Requirements:
----------------------
  - ESP32
  - TFT (160*128 minimum recommended, working on elastic design to cover higher resolutions)
  - DCF77 Atomic clock module ( such as this one https://www.tindie.com/products/universalsolder/atomic-clock-am-receiver-kit-dcf77-wwvb-msf-jjy60/ )
  - RTC Module (preferably i2c, e,g, a DS1307)
  - Buttons (optional)
  - Speaker (optional)

Software Requirements:
----------------------
  - M5Stack Core or ESP32-Chimera-Core for compatibility https://github.com/tobozo/ESP32-Chimera-Core (replaces M5Stack Core, adds support for Odroid-Go, TTGO-TS, Wrover-Kit LoLin D-32Pro)
  - Bodmer's JPEGDecoder for rendering jpeg in sprites https://github.com/Bodmer/JPEGDecoder

CREDITS:
--------
  I learned a lot from Erik de Ruiter who learned a lot from the work of Matthias Dalheimer and Thijs Elenbaas 
  who made their own DCF77 decoders.
  
  Although the changes I made are far from optimistic, this code has wisdom DNA in its roots!
  
  Without the incredible work of these geniuses I would not have known where to start and how to write those credits :)

Author:
-------
  - https://github.com/tobozo/
  - https://twitter.com/tobozotagada/
  - https://www.youtube.com/c/tobozotagada


Interesting websites:
---------------------
  - Inspirational video     : https://www.youtube.com/watch?v=ZadSU_DT-Ks
  - UI code based on        : https://github.com/deruiter/DCF77-Analyzer-Clock-V2.0
  - Crypto code based on    : https://github.com/FroggySoft/AlarmClock 
  - Weather code based on   : http://arduino-projects4u.com/home-weather-station/
  - Erik de Ruiter          : https://www.hackster.io/edr1924
  - Brett Oliver            : http://www.brettoliver.org.uk/
  - Joop Tap                : http://www.jooptap.nl
  - Thijs Ellenbaas         : http://thijs.elenbaas.net/2012/04/arduino-dcf77-radio-clock-receiver-hardware-2/
  - Mathias Dalheimer       : https://github.com/roddi/DCF77-Arduino/blob/master/DCF77Servoclock/DCF77.h
  - DCF77 wikipedia         : https://en.wikipedia.org/wiki/DCF77
  - Much more DCF77 info    : http://www.picbasic.nl/indexes_uk.htm
  - Erik de Ruiter's Flickr : https://www.flickr.com/photos/edr1924/albums
  - Erik de Ruiter's Github : https://github.com/deruiter
  - FroggySoft's Github     : https://github.com/FroggySoft
  
