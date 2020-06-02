// how was the spritesheet made ?
//  - https://github.com/erikflowers/weather-icons
//  - inkscape (svg to png)
//  - imagemagick (negate png + montage)
//  - xxd (binary to const char*)

// $> git clone https://github.com/erikflowers/weather-icons
// $> cd weather-icons/svg/
// $> find "./" -name "*.svg" | while read file; do inkscape -z -e "$file.png" -w 32 -h 32 "$file"; done
// $> find "./" -name "*.png" | while read file; do convert "$file" -negate "$file" ; done
// $> montage -background black -geometry +0+0*.png sprite.jpg
// $> xxd -i sprite.jpg > spritesheet.h


/*
 *

@import "icon-variables/variables-beaufort.less";
@import "icon-variables/variables-time.less";
@import "icon-variables/variables-moon.less";
@import "icon-variables/variables-direction.less";
@import "icon-variables/variables-day.less";
@import "icon-variables/variables-night.less";
@import "icon-variables/variables-neutral.less";
@import "icon-variables/variables-misc.less";


 	classes-beaufort.less
	classes-day.less
	classes-direction.less
	classes-misc.less
	classes-moon-aliases.less
	classes-moon.less
	classes-neutral.less
	classes-night.less
	classes-time.less
	classes-wind-aliases.less
	classes-wind-degrees.less
	classes-wind.less



 *
 *
 * */


#include "Assets.h"

// icon coords [x,y] = [(iconMap%16)*32, (iconMap/16)*32]
enum SpriteSheetIcon {
  // 1
  alien,                      barometer,                   celsius,                    cloud_down,
  cloud,                      cloud_refresh,               cloud_up,                   cloudy_gusts,
  cloudy,                     cloudy_windy,                day_cloudy_gusts,           day_cloudy_high,
  day_cloudy,                 day_cloudy_windy,            day_fog,                    day_hail,
  // 2
  day_haze,                   day_lightning,               day_light_wind,             day_rain_mix,
  day_rain,                   day_rain_wind,               day_showers,                day_sleet,
  day_sleet_storm,            day_snow,                    day_snow_thunderstorm,      day_snow_wind,
  day_sprinkle,               day_storm_showers,           day_sunny_overcast,         day_sunny,
  // 3
  day_thunderstorm,           day_windy,                   degrees,                    direction_down_left,
  direction_down,             direction_down_right,        direction_left,             direction_right,
  direction_up_left,          direction_up,                direction_up_right,         dust,
  earthquake,                 fahrenheit,                  fire,                       flood,
  // 4
  fog,                        gale_warning,                hail,                       horizon_alt,
  horizon,                    hot,                         humidity,                   hurricane,
  hurricane_warning,          lightning,                   lunar_eclipse,              meteor,
  moon_alt_first_quarter,     moon_alt_full,               moon_alt_new,               moon_alt_third_quarter,
  // 5
  moon_alt_waning_crescent_1, moon_alt_waning_crescent_2,  moon_alt_waning_crescent_3, moon_alt_waning_crescent_4,
  moon_alt_waning_crescent_5, moon_alt_waning_crescent_6,  moon_alt_waning_gibbous_1,  moon_alt_waning_gibbous_2,
  moon_alt_waning_gibbous_3,  moon_alt_waning_gibbous_4,   moon_alt_waning_gibbous_5,  moon_alt_waning_gibbous_6,
  moon_alt_waxing_crescent_1, moon_alt_waxing_crescent_2,  moon_alt_waxing_crescent_3, moon_alt_waxing_crescent_4,
  // 6
  moon_alt_waxing_crescent_5, moon_alt_waxing_crescent_6,  moon_alt_waxing_gibbous_1,  moon_alt_waxing_gibbous_2,
  moon_alt_waxing_gibbous_3,  moon_alt_waxing_gibbous_4,   moon_alt_waxing_gibbous_5,  moon_alt_waxing_gibbous_6,
  moon_first_quarter,         moon_full,                   moon_new,                   moonrise,
  moonset,                    moon_third_quarter,          moon_waning_crescent_1,     moon_waning_crescent_2,
  // 7
  moon_waning_crescent_3,     moon_waning_crescent_4,      moon_waning_crescent_5,     moon_waning_crescent_6,
  moon_waning_gibbous_1,      moon_waning_gibbous_2,       moon_waning_gibbous_3,      moon_waning_gibbous_4,
  moon_waning_gibbous_5,      moon_waning_gibbous_6,       moon_waxing_crescent_1,     moon_waxing_crescent_2,
  moon_waxing_crescent_3,     moon_waxing_crescent_4,      moon_waxing_crescent_5,     moon_waxing_crescent_6,
  // 8
  moon_waxing_gibbous_1,      moon_waxing_gibbous_2,       moon_waxing_gibbous_3,      moon_waxing_gibbous_4,
  moon_waxing_gibbous_5,      moon_waxing_gibbous_6,       na,                         night_alt_cloudy_gusts,
  night_alt_cloudy_high,      night_alt_cloudy,            night_alt_cloudy_windy,     night_alt_hail,
  night_alt_lightning,        night_alt_partly_cloudy,     night_alt_rain_mix,         night_alt_rain,
  // 9
  night_alt_rain_wind,        night_alt_showers,           night_alt_sleet,            night_alt_sleet_storm,
  night_alt_snow,             night_alt_snow_thunderstorm, night_alt_snow_wind,        night_alt_sprinkle,
  night_alt_storm_showers,    night_alt_thunderstorm,      night_clear,                night_cloudy_gusts,
  night_cloudy_high,          night_cloudy,                night_cloudy_windy,         night_fog,
  // 10
  night_hail,                 night_lightning,             night_partly_cloudy,        night_rain_mix,
  night_rain,                 night_rain_wind,             night_showers,              night_sleet,
  night_sleet_storm,          night_snow,                  night_snow_thunderstorm,    night_snow_wind,
  night_sprinkle,             night_storm_showers,         night_thunderstorm,         raindrop,
  // 11
  raindrops,                  rain_mix,                    rain,                       rain_wind,
  refresh_alt,                refresh,                     sandstorm,                  showers,
  sleet,                      small_craft_advisory,        smog,                       smoke,
  snowflake_cold,             snow,                        snow_wind,                  solar_eclipse,
  // 12
  sprinkle,                   stars,                       storm_showers,              storm_warning,
  strong_wind,                sunrise,                     sunset,                     thermometer_exterior,
  thermometer_internal,       thermometer,                 thunderstorm,               time_10,
  time_11,                    time_12,                     time_1,                     time_2,
  // 13
  time_3,                     time_4,                      time_5,                     time_6,
  time_7,                     time_8,                      time_9,                     tornado,
  train,                      tsunami,                     umbrella,                   volcano,
  wind_beaufort_0,            wind_beaufort_10,            wind_beaufort_11,           wind_beaufort_12,
  // 14
  wind_beaufort_1,            wind_beaufort_2,             wind_beaufort_3,            wind_beaufort_4,
  wind_beaufort_5,            wind_beaufort_6,             wind_beaufort_7,            wind_beaufort_8,
  wind_beaufort_9,            wind_deg,                    windy,                      atomic,
  hourglass,                  nullicon
};


static SpriteSheetIcon lasticon1=nullicon, lasticon2=nullicon, lasticon3=nullicon;

struct UI_Icon {
  UI_Icon( uint16_t _x, uint16_t _y, SpriteSheetIcon _spriteSheetIcon ) : x(_x), y(_y), spriteSheetIcon(_spriteSheetIcon){ };
  void set( uint16_t _x, uint16_t _y, SpriteSheetIcon _spriteSheetIcon ) {
    x = _x;
    y = _y;
    spriteSheetIcon = _spriteSheetIcon;
  }
  uint16_t x;
  uint16_t y;
  SpriteSheetIcon spriteSheetIcon;
};

UI_Icon *myIcon = nullptr;
