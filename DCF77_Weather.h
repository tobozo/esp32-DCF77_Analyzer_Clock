// code inspired by https://github.com/FroggySoft/AlarmClock and http://arduino-projects4u.com/home-weather-station/

#define WEATHER_SIZE      82
#define WEATHER_INFO_SIZE  3


int significantweather[60] = {0};
String weathermemory[60];
int mWeatherData[WEATHER_SIZE+1];
uint8_t mWeatherArea;
uint8_t mWeatherSection;


String decToBinStr( uint8_t aInfo[3] );
int binStrToDec( String input );

// outside this scope
extern int dcfDST, dcfMinute, dcfHour;
static void updateScroll( String text );
static void updateIcons( SpriteSheetIcon icon1, SpriteSheetIcon icon2, SpriteSheetIcon icon3 );
static bool weatherReady = false;

// for serial output
char degreeSign[] = {
  /* ° */
  0xC2,  0xB0,  0
};
char clockSign[] = {
  /* ⏰ */
  0xE2,  0x8F,  0xB0,  0
};


typedef struct {
  const char* code;
  const char* name;
} countryBycode;

bool operator==(const countryBycode& lhs, const countryBycode& rhs) {
  if( lhs.code == nullptr || rhs.code == nullptr ) return false;
  return strcmp(lhs.code, rhs.code)==0 && strcmp(lhs.name, rhs.name)==0;/* your comparison code goes here */
}
bool operator==(const countryBycode lhs, countryBycode* rhs) {
  if( lhs.code == nullptr || rhs->code == nullptr ) return false;
  if( lhs.name == nullptr || rhs->name == nullptr ) return false;
  return strcmp(lhs.code, rhs->code)==0 && strcmp(lhs.name, rhs->name)==0;/* your comparison code goes here */
}

typedef struct {
  countryBycode country;
  const char* name;
} cityByCountry;

typedef struct {
  const char* label;
  SpriteSheetIcon icon;
} iconByLabel;

const countryBycode nullCity     = {nullptr, nullptr};
const countryBycode France       = {"F",   "France"};
const countryBycode Belgium      = {"B",   "Belgium"};
const countryBycode Switzerland  = {"CH",  "Switzerland"};
const countryBycode Germany      = {"D",   "Germany"};
const countryBycode GreatBritain = {"GB",  "Great Britain"};
const countryBycode Denmark      = {"DK",  "Denmark"};
const countryBycode Italy        = {"I",   "Italy"};
const countryBycode Netherlands  = {"NL",  "Netherlands"};
const countryBycode Austria      = {"A",   "Austria"};
const countryBycode Slovakia     = {"SK",  "Slovakia"};
const countryBycode Czechia      = {"CZ",  "Czechia"};
const countryBycode Sweden       = {"S",   "Sweden"};
const countryBycode Norway       = {"N",   "Norway"};
const countryBycode Hungary      = {"H",   "Hungary"};
const countryBycode Spain        = {"E",   "Spain"};
const countryBycode Andorra      = {"AND", "Andorra"};
const countryBycode Portugal     = {"P",   "Portugal"};
const countryBycode Ireland      = {"IRL", "Ireland"};
const countryBycode Poland       = {"PL",  "Poland"};
const countryBycode Croatia      = {"HR",  "Croatia"};

// for convenience
const countryBycode countries[20] = {
  France, Belgium, Switzerland, Germany, GreatBritain, Denmark, Italy, Netherlands, Austria, Slovakia, Czechia,
  Sweden, Norway, Hungary, Spain, Andorra, Portugal, Ireland, Poland, Croatia
};

static countryBycode *watchedCountry = nullptr;
static char *watchedCity = nullptr;

// order matters !
const cityByCountry cities[] = {
  {France, "Bordeaux"}, {France, "La Rochelle"}, {France, "Paris"}, {France, "Brest"}, {France, "Clermont-Ferrand"}, {France, "Beziers"}, {Belgium, "Bruxelles"}, {France, "Dijon"}, {France, "Marseille"}, {France, "Lyon"}, {France, "Grenoble"},
  {Switzerland, "La Chaux de Fonds"}, {Germany, "Frankfurt am Main"}, {Germany, "Trier"}, {Germany, "Duisburg"}, {GreatBritain, "Swansea"}, {GreatBritain, "Manchester"}, {France, "Le Havre"}, {GreatBritain, "London"}, {Germany, "Bremerhaven"},
  {Denmark, "Herning"}, {Denmark, "Arhus"}, {Germany, "Hannover"}, {Denmark, "Copenhagen"}, {Germany, "Rostock"}, {Germany, "Ingolstadt"}, {Germany, "Muenchen"}, {Italy, "I Bolzano"}, {Germany, "Nuernberg"}, {Germany, "Leipzig"},
  {Germany, "Erfurt"}, {Switzerland, "Lausanne"}, {Switzerland, "Zuerich"}, {Switzerland, "Adelboden"}, {Switzerland, "Sion"}, {Switzerland, "Glarus"}, {Switzerland, "Davos"}, {Germany, "Kassel"}, {Switzerland, "Locarno"}, {Italy, "Sestriere"},
  {Italy, "Milano"}, {Italy, "Roma"}, {Netherlands, "Amsterdam"}, {Italy, "Genova"}, {Italy, "Venezia"}, {Germany, "Strasbourg"}, {Austria, "Klagenfurt"}, {Austria, "Innsbruck"}, {Austria, "Salzburg"}, {Slovakia, "Wien/Bratislava"},
  {Czechia, "Praha"}, {Czechia, "Decin"}, {Germany, "Berlin"}, {Sweden, "Gothenburg"}, {Sweden, "Stockholm"}, {Sweden, "Kalmar"}, {Sweden, "Joenkoeping"}, {Germany, "Donaueschingen"}, {Norway, "Oslo"}, {Germany, "Stuttgart"},
  {Italy, "Napoli"}, {Italy, "Ancona"}, {Italy, "Bari"}, {Hungary, "Budapest"}, {Spain, "Madrid"}, {Spain, "Bilbao"}, {Italy, "Palermo"}, {Spain, "Palma de Mallorca"}, {Spain, "Valencia"}, {Spain, "Barcelona"}, {Andorra, "Andorra"},
  {Spain, "Sevilla"}, {Portugal, "Lissabon"}, {Italy, "Sassari"}, {Spain, "Gijon"}, {Ireland, "Galway"}, {Ireland, "Dublin"}, {GreatBritain, "Glasgow"}, {Norway, "Stavanger"}, {Norway, "Trondheim"}, {Sweden, "Sundsvall"}, {Poland, "Gdansk"},
  {Poland, "Warszawa"}, {Poland, "Krakow"}, {Sweden, "Umea"}, {Sweden, "Oestersund"}, {Switzerland, "Samedan"}, {Croatia, "Zagreb"}, {Switzerland, "Zermatt"}, {Croatia, "Split"}, nullCity
};

//static const citiesLength = (sizeof(cities)/sizeof(cityByCountry)) -1;
static const int citiesLength = 90;


const countryBycode *findCountryByName( const char* countryName ) {
  for( int i=0; i< citiesLength; i++ ) {
    if( strcmp( cities[i].country.name, countryName ) == 0 ) {
      return &cities[i].country;
    }
  }
  return nullptr;
}

int findCityIndexByCityId( int cityID ) {
  for( int i=0;i< citiesLength; i++ ) {
    if( cities[i].name == nullptr ) break;
  }
  return -1;
}

int findCityIDByName( const char* cityName ) {
  //int citiesLength = (sizeof(cities)/sizeof(cityByCountry)) -1;
  if( cityName==nullptr || cityName[0] == '\0' ) return -1;
  //log_w("searching for city: %s", cityName);
  for( int i=0; i< citiesLength; i++ ) {
    if( cities[i].name == nullptr ) break;
    if( strcmp( cities[i].name, cityName ) == 0 ) {

      return i;
    }
  }
  return -1;
}


iconByLabel weather[] = {
  { "Reserved",       na },
  { "Sunny",          day_sunny },
  { "Partly clouded", cloud },
  { "Mostly clouded", cloudy },
  { "Overcast",       cloud_down },
  { "Heat storms",    storm_warning },
  { "Heavy rain",     rain },
  { "Snow",           snow },
  { "Fog",            fog },
  { "Sleet",          sleet },
  { "Rain showers",   showers },
  { "light rain",     rain_mix },
  { "Snow showers",   snow_wind },
  { "Frontal storms", storm_showers },
  { "Stratus cloud",  cloud_up },
  { "Sleet storms",   day_sleet_storm }
};


iconByLabel heavyweather[] = {
  { "None",                  na },
  { "Heavy Weather 24 hrs.", na },
  { "Heavy weather Day",     na },
  { "Heavy weather Night",   na },
  { "Storm 24hrs.",          na },
  { "Storm Day",             day_storm_showers },
  { "Storm Night",           night_storm_showers },
  { "Wind gusts Day",        cloudy_gusts },
  { "Wind gusts Night",      night_alt_cloudy_gusts },
  { "Icy rain morning",      day_rain_mix },
  { "Icy rain evening",      night_alt_rain_mix },
  { "Icy rain night",        night_snow_wind },
  { "Fine dust",             dust },
  { "Ozon",                  raindrop },
  { "Radiation",             wind_deg },
  { "High water",            tsunami }
};

iconByLabel rainprobability[] = {
  { "0 %",   humidity },
  { "15 %",  humidity },
  { "30 %",  humidity },
  { "45 %",  humidity },
  { "60 %",  humidity },
  { "75 %",  humidity },
  { "90 %",  humidity },
  { "100 %", humidity }
};

iconByLabel winddirection[] = {
  { "N",          direction_up },
  { "NO",         direction_up_left },
  { "O",          direction_left },
  { "SO",         direction_down_left },
  { "S",          direction_down },
  { "SW",         direction_down_right },
  { "W",          direction_right },
  { "NW",         direction_up_right },
  { "Changeable", nullicon },
  { "Foen",       nullicon },
  { "Biese N/O",  nullicon },
  { "Mistral N",  nullicon },
  { "Scirocco S", nullicon },
  { "Tramont W",  nullicon },
  { "reserved",   nullicon },
  { "reserved",   nullicon }
};

iconByLabel windstrength[] = {
  { "0",    wind_beaufort_0 },
  { "0-2",  wind_beaufort_2 },
  { "3-4",  wind_beaufort_4 },
  { "5-6",  wind_beaufort_6 },
  { "7",    wind_beaufort_7 },
  { "8",    wind_beaufort_8 },
  { "9",    wind_beaufort_9 },
  { ">=10", wind_beaufort_10 }
};

iconByLabel anomaly1[] {
  { "Same Weather ",  nullicon },
  { "Jump 1 ",        nullicon },
  { "Jump 2 ",        nullicon },
  { "Jump 3 ",        nullicon }
};

iconByLabel anomaly2[] {
  { "0-2 hrs",  nullicon },
  { "2-4 hrs",  nullicon },
  { "5-6 hrs",  nullicon },
  { "7-8 hrs",  nullicon }
};


String meteodata;



void setTmpForecast( int forecastID );


struct SignificantWeatherData {
  int xwh = -1;  // Extremeweather
  int anm1 = -1; // Weather anomality 1
  int anm2 = -1; // Weather anomality 2
  int wdir = -1; // wind direction
  int wstr = -1; // wind strength
  void setWeather( int _xwh, int _anm1, int _anm2 ) {
    xwh  = _xwh;
    anm1 = _anm1;
    anm2 = _anm2;
  }
  void setWind( int _wdir, int _wstr ) {
    wdir = _wdir;
    wstr = _wstr;
  }
};

static SignificantWeatherData** SignificantWeatherDataByCity = nullptr;


void setWeather( int cityID, int _xwh, int _anm1, int _anm2 ) {
  if( cityID < 0 || cityID > 90 ) {
    log_e("Invalid city ID : %d", cityID ); return;
  }
  if( SignificantWeatherDataByCity == nullptr || SignificantWeatherDataByCity[cityID] == nullptr ) {
    log_e("Premature call for city ID %d", cityID );
  }
  SignificantWeatherDataByCity[cityID]->setWeather( _xwh, _anm1, _anm2 );
}

void setWind( int cityID, int _wdir, int _wstr ) {
  if( cityID < 0 || cityID > 90 ) {
    log_e("Invalid city ID : %d", cityID ); return;
  }
  if( SignificantWeatherDataByCity == nullptr || SignificantWeatherDataByCity[cityID] == nullptr ) {
    log_e("Premature call for city ID %d", cityID );
  }
  SignificantWeatherDataByCity[cityID]->setWind( _wdir, _wstr );
}



int16_t *PrefsCache = new int16_t[480*3];




struct WeatherForecast {

  int hour=-1, minute=-1, forecastID=-1;
  uint8_t data[3] = {0,0,0};

  // save to ram
  void set( int _forecastID, int _hour, int _minute, uint8_t _data[3] ) {
    hour       = _hour;
    minute     = _minute;
    forecastID = _forecastID;
    // store in current object
    memcpy( data, _data, 3);
    // also copy to prefs cache
    PrefsCache[forecastID*3]    = data[0];
    PrefsCache[forecastID*3 +1] = data[1];
    PrefsCache[forecastID*3 +2] = data[2];

    setTmpForecast( forecastID );

    log_w("Saved 3 bytes in weather cache {%d,%d,%d} for forecast #%d [%02d:%02d]", data[0], data[1], data[2], forecastID, hour, minute );

  }

  // load from sram
  bool load( int _forecastID ) {
    forecastID = _forecastID;
    bool ret = false;

    if( PrefsCache[forecastID*3] > -1 ) {
      data[0] = PrefsCache[forecastID*3];
      data[1] = PrefsCache[forecastID*3 +1];
      data[2] = PrefsCache[forecastID*3 +2];
      // pref exists, speculate hour/minute from that
      hour   = forecastID / 20;
      minute = (((forecastID)%20)*3)+2;
      ret = true;
      log_w("Thawed 3 {%d,%d,%d} bytes from NVS for forecast #%d with key '%s' [%02d:%02d]", data[0], data[1], data[2], forecastID, String(forecastID).c_str(), hour, minute );
    } else {
      // this sketch has never run more than 24h
    }
    return ret;

  }
};

WeatherForecast** AllCitiesForecast = nullptr;


void setForecast( int forecastID, int forecastHour, int forecastMinute, uint8_t aInfo[3] ) {
  if( AllCitiesForecast == nullptr ) {
    log_e("Premature requests ignored for forecastID #%d", forecastID);
    return;
  }
  if( forecastID > 480 || forecastID < 0 ) {
    log_e("Invalid forecastID : %d", forecastID);
    return;
  }
  AllCitiesForecast[forecastID]->set( forecastID, forecastHour, forecastMinute, aInfo );
}


/*
  0110 Day critical weather
      0001 Night critical weather
          0001 Extreme weather
              011 Rain prob.
                  0 Anomaly
                  101101 Temp
*/
#define CRLF    "\n"
#define TIM_TPL "Weather data at %02dh%02dm:"    CRLF
#define AR1_TPL "Area: %s"  CRLF
#define AR2_TPL "Area1: %s" CRLF "Area2: %s"     CRLF
#define DCW_TPL "%4s Day critical weather"       CRLF
#define NCW_TPL "    %4s Night critical weather" CRLF
#define XWH_TPL "        %4s Extreme weather"    CRLF
#define RPB_TPL "            %3s Rain prob."     CRLF
#define ANM_TPL "               %s Anomaly"      CRLF
#define TEM_TPL "                %6s Temp."      CRLF

#define WeatherDataTpl1 CRLF TIM_TPL AR1_TPL DCW_TPL NCW_TPL XWH_TPL RPB_TPL ANM_TPL TEM_TPL
#define WeatherDataTpl2 CRLF TIM_TPL AR2_TPL DCW_TPL NCW_TPL XWH_TPL RPB_TPL ANM_TPL TEM_TPL
//#define WeatherDataTpl "\n%4s Day critical weather\n    %4s Night critical weather\n        %4s Extreme weather\n            %3s Rain prob.\n               %s Anomaly\n                %6s Temp.\n"
char *WeatherDataStr = new char[512];// = {0};

struct WeatherData {

  int hour, minute;
  //int minuteOfDay;

  int dcw;  // Day weather
  int ncw;  // Night weather

  int xwh;  // Extremeweather
  int anm1; // Weather anomality 1
  int anm2; // Weather anomality 2

  int rpb;  // Rainprobability
  int anm;  // Anomaly
  int tem;  // Temperature
  //int twodayforecast; // city index
  //int fourdayforecast; // city index
  uint8_t city4DayIndex = 255;
  uint8_t city2Dayindex = 255;
  bool is4dayforecast = false;

  void set( WeatherForecast *fcast, bool debugEveryCity=false ) {
    reset();
    bool debugToSerial = debugEveryCity; // default
    hour   = fcast->hour;
    minute = fcast->minute;
    WeatherDataStr[0] = '\0';

    String wData = decToBinStr( fcast->data );
    dcw  = binStrToDec( wData.substring(0, 4) );   // Day critical weather
    ncw  = binStrToDec( wData.substring(4, 8) );   // Night critical weather
    xwh  = binStrToDec( wData.substring(8, 12) );  // Weather anomalities combined
    anm1 = binStrToDec( wData.substring(8, 10) );  // Weather anomality 1
    anm2 = binStrToDec( wData.substring(10, 12) ); // Weather anomality 2
    rpb  = binStrToDec( wData.substring(12, 15) ); // Rainprobability
    anm  = binStrToDec( wData.substring(15, 16) ); // Anomaly
    tem  = binStrToDec( wData.substring(16, 22) ); // Temperature

    city4DayIndex = ((hour) % 3) * 20;
    if (minute > 0) {
      city4DayIndex += (minute - 1) / 3;
    }

    if (minute > 0) {
      city2Dayindex = ((((hour) * 60) + (minute - 1)) % 90) / 3;
    } else {
      city2Dayindex = ((hour) * 60);
    }
    city2Dayindex += 60;

    int city4ID = -1;
    int city2ID = -1;

    if( city2Dayindex <= 90 ) {
      city2ID = findCityIDByName( cities[city2Dayindex].name );
      if( !debugEveryCity && strcmp( cities[city2Dayindex].name, watchedCity ) == 0 ) {
        debugToSerial = true;
      }
    } else {
      log_e("********** bogus value for city2Dayindex %d at [%02d:%02d]", city2Dayindex, hour, minute );
      return;
    }
    if( city4DayIndex <= 90 ) {
      city4ID = findCityIDByName( cities[city4DayIndex].name );
      if( !debugEveryCity && strcmp( cities[city4DayIndex].name, watchedCity ) == 0 ) {
        debugToSerial = true;
      }
    } else {
      log_e("********** bogus value for city4DayIndex %d at [%02d:%02d]", city4DayIndex, hour, minute );
      return;
    }

    if (hour < 21) {

      if ((hour) % 6 < 3) {
        // Extreme weather is valid from this hour but also +3 hour
        // Cache the heavy weather data for this city
        log_d("1: Will %s update weather info for cityID %d / %d / [%02d:%02d]", city4ID > -1 ? "":"NOT", city4ID, city4DayIndex, hour, minute);
        setWeather( city4ID, xwh, anm1, anm2);
      }
      if ((hour) % 6 > 2) {
        // Cache the wind data for this city
        log_d("1: Will %s update wind info for cityID %d / %d / [%02d:%02d]", city4ID > -1 ? "":"NOT", city4ID, city4DayIndex, hour, minute);
        setWind( city4ID, xwh, rpb );
      }
      is4dayforecast = true;
    } else {
      // Between 21:00-23:59 significant weather & temperature is for cities 60-89, wind and wind direction for cities 0-59.
      // Cache the wind data for this city
      //int cityID = city2ID;
      log_d("2: Will %s update wind info for cityID %d / %d / [%02d:%02d]", city2ID > -1 ? "":"NOT", city2ID, city2Dayindex, hour, minute);
      setWind( city2ID, xwh, rpb );
      is4dayforecast = false;
    }

    if( debugToSerial ) {
      if( is4dayforecast ) {
        sprintf( WeatherDataStr, WeatherDataTpl1,
          hour,
          minute,
          cities[city4DayIndex].name,
          wData.substring(0, 4).c_str(),   // Day critical weather
          wData.substring(4, 8).c_str(),   // Night critical weather
          wData.substring(8, 12).c_str(),  // Weather anomalities combined
          wData.substring(12, 15).c_str(), // Rainprobability
          wData.substring(15, 16).c_str(), // Anomaly
          wData.substring(16, 22).c_str()  // Temperature
        );
      } else {
        sprintf( WeatherDataStr, WeatherDataTpl2,
          hour,
          minute,
          cities[city4DayIndex].name,
          cities[city2Dayindex].name,
          wData.substring(0, 4).c_str(),   // Day critical weather
          wData.substring(4, 8).c_str(),   // Night critical weather
          wData.substring(8, 12).c_str(),  // Weather anomalities combined
          wData.substring(12, 15).c_str(), // Rainprobability
          wData.substring(15, 16).c_str(), // Anomaly
          wData.substring(16, 22).c_str()  // Temperature
        );
      }
      Serial.printf( "[%03d] %s", fcast->forecastID, WeatherDataStr );
      render();
    }

  }


  int                    AreaCountryID = -1;
  const char*            AreaCountryName = nullptr;
  const char*            AreaCityName = nullptr;
  int                    AreaTimeSlice1 = -1;
  const char*            AreaTime1Label = nullptr;
  const char*            TimeSlice1WeatherPrefix = nullptr;
  const char*            TimeSlice1WeatherLabel = nullptr;
  const SpriteSheetIcon* TimeSlice1WeatherIcon = nullptr;
  int                    AreaTimeSlice2 = -1;
  const char*            AreaTime2Label = nullptr;
  const char*            TimeSlice2WeatherPrefix = nullptr;
  const char*            TimeSlice2WeatherLabel = nullptr;
  const SpriteSheetIcon* TimeSlice2WeatherIcon = nullptr;
  const char*            ExtremeweatherLabel = nullptr;
  const SpriteSheetIcon* ExtremeweatherIcon = nullptr;
  const char*            ExtremeweatherAnomaly1Label = nullptr;
  const SpriteSheetIcon* ExtremeweatherAnomaly1Icon = nullptr;
  const char*            ExtremeweatherAnomaly2Label = nullptr;
  const SpriteSheetIcon* ExtremeweatherAnomaly2Icon = nullptr;
  int                    RainprobabilityRawValue = -1;
  const char*            Rainprobability = nullptr;
  const char*            WinddirectionLabel = nullptr;
  const SpriteSheetIcon* WinddirectionIcon = nullptr;
  const char*            WindstrengthLabel = nullptr;
  const SpriteSheetIcon* WindstrengthIcon = nullptr;
  const char*            WeatherAnomality = nullptr;
  const char*            Temperature = nullptr;
  const char*            TemperatureTrend = nullptr;

  void reset() {
    AreaCountryID = -1;
    AreaCountryName = nullptr;
    AreaCityName = nullptr;
    AreaTimeSlice1 = -1;
    AreaTime1Label = nullptr;
    TimeSlice1WeatherPrefix = nullptr;
    TimeSlice1WeatherLabel = nullptr;
    TimeSlice1WeatherIcon = nullptr;
    AreaTimeSlice2 = -1;
    AreaTime2Label = nullptr;
    TimeSlice2WeatherPrefix = nullptr;
    TimeSlice2WeatherLabel = nullptr;
    TimeSlice2WeatherIcon = nullptr;
    ExtremeweatherLabel = nullptr;
    ExtremeweatherIcon = nullptr;
    ExtremeweatherAnomaly1Label = nullptr;
    ExtremeweatherAnomaly1Icon = nullptr;
    ExtremeweatherAnomaly2Label = nullptr;
    ExtremeweatherAnomaly2Icon = nullptr;
    RainprobabilityRawValue = -1;
    Rainprobability = nullptr;
    WinddirectionLabel = nullptr;
    WinddirectionIcon = nullptr;
    WindstrengthLabel = nullptr;
    WindstrengthIcon = nullptr;
    WeatherAnomality = nullptr;
    Temperature = nullptr;
    TemperatureTrend = nullptr;
  }

  void setAreaCountryName(const char* name) { AreaCountryName=name; };
  void setAreaCityName(const char* name) { AreaCityName=name; };
  void setAreaTimeSlice(int slicenum) { AreaTimeSlice1=slicenum; };
  void setAreaTime1Label(const char* label) { AreaTime1Label=label; }
  void setTimeSlice1WeatherPrefix(const char* prefix) { TimeSlice1WeatherPrefix=prefix; };
  void setTimeSlice1WeatherLabel(const char* label) { TimeSlice1WeatherLabel=label; };
  void setTimeSlice1WeatherIcon( const SpriteSheetIcon* icon ) { TimeSlice1WeatherIcon=icon; };
  void setAreaTimeSlice2( int slicenum ) { AreaTimeSlice2=slicenum; };
  void setAreaTime2Label(const char* label) { AreaTime2Label=label; }
  void setTimeSlice2WeatherPrefix(const char* prefix) { TimeSlice2WeatherPrefix=prefix; };
  void setTimeSlice2WeatherLabel(const char* label) { TimeSlice2WeatherLabel=label; };
  void setTimeSlice2WeatherIcon( SpriteSheetIcon* icon ) { TimeSlice2WeatherIcon=icon; };
  void setExtremeweatherLabel(const char* label) { ExtremeweatherLabel=label; };
  void setExtremeweatherIcon( SpriteSheetIcon* icon )  { ExtremeweatherIcon=icon; };
  void setExtremeweatherAnomaly1Label(const char* label) { ExtremeweatherAnomaly1Label=label; };
  void setExtremeweatherAnomaly1Icon( SpriteSheetIcon* icon ) { ExtremeweatherAnomaly1Icon=icon; };
  void setExtremeweatherAnomaly2Label(const char* label) { ExtremeweatherAnomaly2Label=label; };
  void setExtremeweatherAnomaly2Icon( SpriteSheetIcon* icon ) { ExtremeweatherAnomaly2Icon=icon; };
  void setRainprobability(const char* percentage) { Rainprobability=percentage; };
  void setWinddirectionLabel(const char* label) { WinddirectionLabel=label; };
  void setWinddirectionIcon( SpriteSheetIcon* icon ) { WinddirectionIcon=icon; };
  void setWindstrengthLabel(const char* label) { WindstrengthLabel=label; };
  void setWindstrengthIcon( SpriteSheetIcon* icon ) { WindstrengthIcon=icon; };
  void setWeatherAnomality(const char* yesno) { WeatherAnomality=yesno; };
  void setTemperature(const char* temperature) { Temperature=temperature; };
  void setTemperatureTrend(const char* trend) { TemperatureTrend=trend; };

  // const'ed for use in the Serial output and the TFT UI
  const char* LABEL_EMPTY = "";
  const char* LABEL_NIGHT = "Night";
  const char* LABEL_DAY   = "Day";
  const char* LABEL_HEAVY = "Heavy ";
  const char* LABEL_CLEAR = "Clear";
  const char* LABEL_YES   = "Yes";
  const char* LABEL_NO    = "No";
  const char* LABEL_21    = "<-21 ";
  const char* LABEL_40    = ">40 ";
  const char* LABEL_MIN   = "min";
  const char* LABEL_MAX   = "max";

  enum RenderPattern {
    PATH_NONE,
    PATH_1,
    PATH_1_1,
    PATH_1_1_1,
    PATH_1_1_2,
    PATH_1_2,
    PATH_2,
    PATH_3
  };

  RenderPattern pattern = PATH_NONE;

  void renderSerial() {
    switch( pattern ) {
      case PATH_1: log_e("wtf"); break; // should never happen
      case PATH_1_1:   break; // (hour%6<3): no extreme or significant weather
      case PATH_1_1_1: break; // (hour%6<3): significant weather but no extreme
      case PATH_1_1_2: break; // (hour%6<3): extreme weather
      case PATH_1_2: break;   // (hour%6>2): extreme weather + wind strength/direction
      case PATH_2: break;
      case PATH_3: break;
      case PATH_NONE: break;
    }
  }

  void renderSprite() {

  }


  void render( bool do_sprite = true ) {

    switch(tem) {
      case 0:
        setTemperature( LABEL_21 );
      break;
      case 63:
        setTemperature( LABEL_40 );
      break;
      default:
        setTemperature( String( tem - 22 ).c_str() );
      break;
    }
    setTemperatureTrend( (((hour) % 6) > 2 && (hour < 21)) ? LABEL_MIN : LABEL_MAX );

    if (hour < 21) {
      // Between 00:00-20:59 weather data is for all cities
      pattern = PATH_1;
      int cityID = findCityIDByName( cities[city4DayIndex].name );
      int significantWeather = SignificantWeatherDataByCity[cityID]->xwh;
      setAreaCountryName( cities[city4DayIndex].country.name );
      setAreaCityName( cities[city4DayIndex].name );
      setAreaTimeSlice( int( (hour / 6) + 1 ) );
      setAreaTime1Label( (((hour) % 6) > 2) ? LABEL_NIGHT : LABEL_DAY );
      if( do_sprite ) setTimeSlice1WeatherIcon( &weather[dcw].icon );
      if (dcw == 5 || dcw == 6 || dcw == 13 || dcw == 7) {
        if (significantWeather == 1 || significantWeather == 2) {
          setTimeSlice1WeatherPrefix( (dcw != 6) ? LABEL_HEAVY : LABEL_EMPTY );
          setTimeSlice1WeatherLabel( weather[dcw].label );
        } else {
          setTimeSlice1WeatherLabel( weather[dcw].label );
        }
      } else {
        setTimeSlice1WeatherLabel( weather[dcw].label );
      }
      setAreaTime2Label( LABEL_NIGHT );
      if( do_sprite ) setTimeSlice2WeatherIcon( &weather[ncw].icon );
      if (ncw == 5 || ncw == 6 || ncw == 13 || ncw == 7) {
        if (significantWeather == 1 || significantWeather == 3) {
          setTimeSlice2WeatherPrefix( (ncw != 6) ? LABEL_HEAVY : LABEL_EMPTY );
          setTimeSlice2WeatherLabel( weather[ncw].label );
        } else {
          setTimeSlice2WeatherLabel( weather[ncw].label );
        }
      } else {
        if (ncw == 1) {
          setTimeSlice2WeatherLabel( LABEL_CLEAR );
        } else {
          setTimeSlice2WeatherLabel( weather[ncw].label );
        }
      }
      if ((hour) % 6 < 3) {
        pattern = PATH_1_1;
        setRainprobability( rainprobability[rpb].label );
        if (xwh == 0) { // DI=0 and WA =0
          if( significantWeather > -1 ) {
            pattern = PATH_1_1_1;
            setExtremeweatherLabel( heavyweather[significantWeather].label );
            if( do_sprite ) setExtremeweatherIcon( &heavyweather[significantWeather].icon );
          }
        } else {
          pattern = PATH_1_1_2;
          setExtremeweatherAnomaly1Label( anomaly1[anm1].label );
          if( do_sprite ) setExtremeweatherAnomaly1Icon( &anomaly1[anm1].icon );
          setExtremeweatherAnomaly2Label( anomaly2[anm2].label );
          if( do_sprite ) setExtremeweatherAnomaly2Icon( &anomaly2[anm2].icon );
        }
      } else if ((hour) % 6 > 2) {
        pattern = PATH_1_2;
        if( do_sprite ) setWinddirectionIcon( &winddirection[xwh].icon );
        setWinddirectionLabel( winddirection[xwh].label );
        if( do_sprite ) setWindstrengthIcon( &windstrength[rpb].icon );
        setWindstrengthLabel( windstrength[rpb].label );
        if( do_sprite ) setExtremeweatherIcon( &heavyweather[significantWeather].icon );
        setExtremeweatherLabel( heavyweather[significantWeather].label );
      }
      setWeatherAnomality( (anm == 1) ? LABEL_YES : LABEL_NO );

      renderSerial();
      if( do_sprite ) renderSprite();
    } else {
      // Between 21:00-23:59 significant weather & temperature is for cities 60-89, wind and wind direction for cities 0-59.
      pattern = PATH_2;
      ///*Area1 Country name*/      cities[city4DayIndex].country.name;
      ///*Area1 City name*/         cities[city4DayIndex].name;
      ///*Area1 Time slice*/        (((hour) % 6) > 2) ? LABEL_NIGHT : LABEL_DAY, int((hour / 6) + 1);
      ///*Area1 Day weather*/       weather[dcw].label;
      ///*Area1 Night weather*/     ncw, (ncw == 1) ? LABEL_CLEAR : weather[ncw].label;
      ///*Area1 Weather Anomality*/ (anm == 1) ? LABEL_YES : LABEL_NO;
      renderSerial();
      if( do_sprite ) renderSprite();
      pattern = PATH_3;
      ///*Area2 Country name*/      cities[city2Dayindex].country.name;
      ///*Area2 City name*/         cities[city2Dayindex].name;
      ///*Area2 Time slice*/        (((hour - 21) * 60 + minute) > 90) ? 2 : 1;
      ///*Area2 Winddirection*/     winddirection[xwh].label;
      ///*Area2 Winddirection Icon*/winddirection[xwh].icon;
      ///*Area2 Windstrength*/      windstrength[rpb].label;
      renderSerial();

    }

  }

};

WeatherData tmpWeatherData;
WeatherData** MyCityForecasts;

void setTmpForecast( int forecastID ) {
  if ( AllCitiesForecast == nullptr ) {
    log_e("Premature call for id %d", forecastID );
    return;
  }
  if( forecastID > 480 || forecastID < 0 ) {
    log_e("Invalid id : %d", forecastID );
    return;
  }
  tmpWeatherData.set( AllCitiesForecast[forecastID] );
}

void initMyCityForecastsCache() {

  MyCityForecasts = (WeatherData**)calloc( 8, sizeof( WeatherData* ) );
  for(int i=0;i<8;i++) {
    MyCityForecasts[i] = (WeatherData*)calloc( 1, sizeof( WeatherData ) );
    if( MyCityForecasts[i] == NULL ) {
      log_e("Failed to alloc memory for city forecast slot %d", i);
    }
  }
}


void printPrefsCache() {
  Serial.print("00h: ");
  for( int i=0;i<480*3;i+=3 ) {
    if(PrefsCache[i] != -1) {
      Serial.print("+");
    } else {
      Serial.print(".");
    }
    if( i%60>56 ) {
      Serial.println();
      int t = (i/60)+1;
      if( t < 24 ) {
        Serial.printf("%02dh: ", (i/60)+1);
      }
    }
  }
}

void printSignificantWeatherDataCache() {
  log_w("SignificantWeatherDataCache");
  for( int i=0; i< citiesLength; i++ ) {
    if( SignificantWeatherDataByCity[i]->xwh != -1 || SignificantWeatherDataByCity[i]->wdir != -1 ) {
      Serial.print("+");
    } else {
      Serial.print(".");
    }
  }
  Serial.println();
}



// save to NVS
static void saveWeatherCache() {
  log_w("Saving weather prefs cache to NVS");
  prefs.begin("DCF77WData", false);
  prefs.putBytes( "cache", PrefsCache, 480*3*2 );
  prefs.end();
  printPrefsCache();
}


void dumpWeather() {
  log_w("Will dump weather data for watched city");
  for(int i=0;i<480;i++) {
    if( AllCitiesForecast[i] == NULL || AllCitiesForecast[i]->hour == -1 ) continue;
    setTmpForecast( i );
  }
}


void initWeatherForecastCache() {

  prefs.begin("DCF77WData", true);
  int prefsCacheSize = prefs.getBytes( "cache", PrefsCache, 480*3*2 );
  if( prefsCacheSize == 0 ) {
    log_w("Initiating empty weather prefs cache");
    for(int i=0;i<480*3;i++) {
      PrefsCache[i] = -1;
    }
    saveWeatherCache();
  } else {
    log_w("Loaded %d bytes into weather prefs cache from NVS", prefsCacheSize);
  }
  prefs.end();

  printPrefsCache();

  SignificantWeatherDataByCity = (SignificantWeatherData**)calloc( citiesLength, sizeof( SignificantWeatherData* ) );
  if( SignificantWeatherDataByCity == NULL ) {
    log_e("Failed to alloc memory for significant weather data cache");
  }
  for( int i=0; i< citiesLength; i++ ) {
    SignificantWeatherDataByCity[i] = new  SignificantWeatherData(); //(SignificantWeatherData*)calloc( 1, sizeof( SignificantWeatherData ) );
    if( SignificantWeatherDataByCity[i] == NULL ) {
      log_e("Failed to alloc memory for significant weather cache item #%d", i);
    }
  }

  AllCitiesForecast = (WeatherForecast**)calloc( 480, sizeof( WeatherForecast* ) );
  log_n("Init weather forecast cache");
  for(int i=0;i<480;i++) {
    AllCitiesForecast[i] = (WeatherForecast*)calloc( 1, sizeof( WeatherForecast ) );
    if( AllCitiesForecast[i] == NULL ) {
      log_e("Failed to alloc memory for slot %d", i);
    } else {
      if( AllCitiesForecast[i]->load( i ) ) {
        log_w("Found cached data for forecast #%d [%02dh:%02dm]", i, AllCitiesForecast[i]->hour, AllCitiesForecast[i]->minute);
        setTmpForecast( i );
        //tmpWeatherData.set( AllCitiesForecast[i] );
      } else {
        //Serial.print(".");
      }
    }
  }

  printSignificantWeatherDataCache();

}






void copyWeatherInfo( int* aMessage, uint8_t aIndex ) {
  uint8_t j=aIndex;
  for(uint8_t i=0; i<14; i++) {
    mWeatherData[j++] = aMessage[i+1];
  }
}

bool addToWeatherInfo( int* aMessage ) {
  bool lCompleted = false;
  uint8_t j = 0;
  if( aMessage[20]==1 ) {
    uint8_t _minute = dcfMinute;
    if( _minute==0 ) _minute = 60; //
    _minute--;
    uint8_t part = _minute % 3;
    switch( part ) {
      case 0:  // first part
        // clear all old data
        for( uint8_t i=0; i<WEATHER_SIZE; i++ )
          mWeatherData[i]=0;
        //copy received data into correct part
        copyWeatherInfo( aMessage,0 );
      break;
      case 1: // second part
        copyWeatherInfo( aMessage,14 );
        j = 14+14+14;
        // add time info
        for(uint8_t i=21; i<28; i++)   // add minutes
          mWeatherData[j++] = aMessage[i];
        j++;
        for(uint8_t i=29; i<35; i++)   // add hours
          mWeatherData[j++] = aMessage[i];
        j+=2;
        for(uint8_t i=36; i<42; i++)   // add day
          mWeatherData[j++] = aMessage[i];
        j+=2;
        for(uint8_t i=45; i<50; i++)   // add month
          mWeatherData[j++] = aMessage[i];
        for(uint8_t i=42; i<45; i++)   // add week day
          mWeatherData[j++] = aMessage[i];
        for(uint8_t i=50; i<58; i++)   // add year
          mWeatherData[j++] = aMessage[i];
      break;
      case 2: // third part
        copyWeatherInfo( aMessage, 14+14 );
        lCompleted = true;
      break;
    }
  }
  return lCompleted;
}


uint8_t GetWeatherArea() {
  return mWeatherArea;
}


uint8_t GetWeatherSection() {
  return mWeatherSection;
}


unsigned int getMinutesSince2200() {
  int hours = dcfHour;;
  hours--;    // CET -> UTC
  //if (aTime.dst)  // correction DST
  if( dcfDST==1 )
    hours--;
  hours -= 22;
  if ( hours<0 )
    hours += 24;
  int minutes = dcfMinute + hours*60;
  return minutes;
}


uint8_t getArea()  {
  unsigned int minutes = getMinutesSince2200();
  // each block of data takes 3 minutes
  // in total 60 areas
  int area = minutes%(60*3);
  area /= 3;
  area--;
  return area;
}


uint8_t getSection() {
  unsigned int minutes = getMinutesSince2200();
  // each block of data takes 3 minutes
  // in total 60 areas
  int section = minutes/(60*3);
  return section;
}


/// bit pattern for 0D,0E from 0B-0D
const uint32_t mUintArrBitPattern12[] /*PROGMEM*/ = {
  0x80000, //0b10000000000000000000, // 0D.3
  0x00010, //0b00000000000000010000, // 0B.4
  0x00008, //0b00000000000000001000, // 0B.3
  0x00100, //0b00000000000100000000, // 0C.0
  0x00080, //0b00000000000010000000, // 0B.7
  0x01000, //0b00000001000000000000, // 0C.4
  0x00800, //0b00000000100000000000, // 0C.3
  0x10000, //0b00010000000000000000, // 0D.0
  0x08000, //0b00001000000000000000, // 0C.7
  0x00001, //0b00000000000000000001, // 0B.0
  0x00000, //0b00000000000000000000, // xxxx
  0x00000  //0b00000000000000000000  // xxxx
};

/// <summary>
/// 12-15 from 16-19 (time)
/// </summary>
const uint32_t mUintArrBitPattern30_1[] /*PROGMEM*/ = {
  0x00000200, //0b00000000000000000000001000000000, // 17.1
  0x00000020, //0b00000000000000000000000000100000, // 16.5
  0x02000000, //0b00000010000000000000000000000000, // 19.1
  0x00000000, //0b00000000000000000000000000000000, // 1A.3
  0x00000000, //0b00000000000000000000000000000000, // 1A.5
  0x00000080, //0b00000000000000000000000010000000, // 16.7
  0x40000000, //0b01000000000000000000000000000000, // 19.6
  0x01000000, //0b00000001000000000000000000000000, // 19.0

  0x04000000, //0b00000100000000000000000000000000, // 19.2
  0x00000000, //0b00000000000000000000000000000000, // 1A.4
  0x00010000, //0b00000000000000010000000000000000, // 18.0
  0x00000000, //0b00000000000000000000000000000000, // 1A.2
  0x00400000, //0b00000000010000000000000000000000, // 18.6
  0x00000010, //0b00000000000000000000000000010000, // 16.4
  0x00200000, //0b00000000001000000000000000000000, // 18.5
  0x00080000, //0b00000000000010000000000000000000, // 18.3

  0x00004000, //0b00000000000000000100000000000000, // 17.6
  0x00000000, //0b00000000000000000000000000000000, // 1A.6
  0x00020000, //0b00000000000000100000000000000000, // 18.1
  0x00100000, //0b00000000000100000000000000000000, // 18.4
  0x00008000, //0b00000000000000001000000000000000, // 17.7
  0x00000040, //0b00000000000000000000000001000000, // 16.6
  0x00001000, //0b00000000000000000001000000000000, // 17.4
  0x00000400, //0b00000000000000000000010000000000, // 17.2

  0x00000001, //0b00000000000000000000000000000001, // 16.0
  0x80000000, //0b10000000000000000000000000000000, // 19.7
  0x00000008, //0b00000000000000000000000000001000, // 16.3
  0x00000002, //0b00000000000000000000000000000010, // 16.1
  0x00040000, //0b00000000000001000000000000000000, // 18.2
  0x10000000  //0b00010000000000000000000000000000 // 19.4
};

/// <summary>
/// bit pattern for 12-15 from 1A (time2)
/// </summary>
const uint8_t mUintArrBitPattern30_2[] /*PROGMEM*/ = {
  0x00, //0b00000000,  // 17.1
  0x00, //0b00000000,  // 16.5
  0x00, //0b00000000,  // 19.1
  0x08, //0b00001000,  // 1A.3
  0x20, //0b00100000,  // 1A.5
  0x00, //0b00000000,  // 16.7
  0x00, //0b00000000,  // 19.6
  0x00, //0b00000000,  // 19.0

  0x00, //0b00000000,  // 19.2
  0x10, //0b00010000,  // 1A.4
  0x00, //0b00000000,  // 18.0
  0x04, //0b00000100,  // 1A.2
  0x00, //0b00000000,  // 18.6
  0x00, //0b00000000,  // 16.4
  0x00, //0b00000000,  // 18.5
  0x00, //0b00000000,  // 18.3

  0x00, //0b00000000,  // 17.6
  0x40, //0b01000000,  // 1A.6
  0x00, //0b00000000,  // 18.1
  0x00, //0b00000000,  // 18.4
  0x00, //0b00000000,  // 17.7
  0x00, //0b00000000,  // 16.6
  0x00, //0b00000000,  // 17.4
  0x00, //0b00000000,  // 17.2

  0x00, //0b00000000,  // 16.0
  0x00, //0b00000000,  // 19.7
  0x00, //0b00000000,  // 16.3
  0x00, //0b00000000,  // 16.1
  0x00, //0b00000000,  // 18.2
  0x00  //0b00000000  // 19.4
};

/// <summary>
/// 12-14 from 1C-1E (result from F)
/// </summary>
const uint32_t mUintArrBitPattern20[] /*PROGMEM*/ = {
  0x000004, //0b000000000000000000000100, // 1C.2
  0x002000, //0b000000000010000000000000, // 1E.5
  0x008000, //0b000000001000000000000000, // 1E.7
  0x400000, //0b010000000000000000000000, // 1D.6
  0x000100, //0b000000000000000100000000, // 1E.0
  0x100000, //0b000100000000000000000000, // 1D.4
  0x000400, //0b000000000000010000000000, // 1E.2
  0x800000, //0b100000000000000000000000, // 1D.7

  0x040000, //0b000001000000000000000000, // 1D.2
  0x020000, //0b000000100000000000000000, // 1D.1
  0x000008, //0b000000000000000000001000, // 1C.3
  0x000200, //0b000000000000001000000000, // 1E.1
  0x004000, //0b000000000100000000000000, // 1E.6
  0x000002, //0b000000000000000000000010, // 1C.1
  0x001000, //0b000000000001000000000000, // 1E.4
  0x080000, //0b000010000000000000000000, // 1D.3

  0x000800, //0b000000000000100000000000, // 1E.3
  0x200000, //0b001000000000000000000000, // 1D.5
  0x010000, //0b000000010000000000000000, // 1D.0
  0x000001  //0b000000000000000000000001  // 1C.0
};

/// <summary>
/// bit pattern for 12-15 from 16-19 (1/3)
/// </summary>
 const uint8_t mByteArrLookupTable1C_1[] /*PROGMEM*/ = {
  0xBB, 0x0E, 0x22, 0xC5, 0x73, 0xDF, 0xF7, 0x6D, 0x90, 0xE9, 0xA1, 0x38, 0x1C, 0x84, 0x4A, 0x56,
  0x64, 0x8D, 0x28, 0x0B, 0xD1, 0xBA, 0x93, 0x52, 0x1C, 0xC5, 0xA7, 0xF0, 0xE9, 0x7F, 0x36, 0x4E,
  0xC1, 0x77, 0x3D, 0xB3, 0xAA, 0xE0, 0x0C, 0x6F, 0x14, 0x88, 0xF6, 0x2B, 0xD2, 0x99, 0x5E, 0x45,
  0x1F, 0x70, 0x96, 0xD3, 0xB3, 0x0B, 0xFC, 0xEE, 0x81, 0x42, 0xCA, 0x34, 0xA5, 0x58, 0x29, 0x67
};

/// <summary>
/// bit pattern for 12-15 from 16-19 (2/3)
/// </summary>
const uint8_t mByteArrLookupTable1C_2[] /*PROGMEM*/ = {
  0xAB, 0x3D, 0xFC, 0x74, 0x65, 0xE6, 0x0E, 0x4F, 0x97, 0x11, 0xD8, 0x59, 0x83, 0xC2, 0xBA, 0x20,
  0xC5, 0x1B, 0xD2, 0x58, 0x49, 0x37, 0x01, 0x7D, 0x93, 0xFA, 0xE0, 0x2F, 0x66, 0xB4, 0xAC, 0x8E,
  0xB7, 0xCC, 0x43, 0xFF, 0x58, 0x66, 0xEB, 0x35, 0x82, 0x2A, 0x99, 0xDD, 0x00, 0x71, 0x14, 0xAE,
  0x4E, 0xB1, 0xF7, 0x70, 0x18, 0x52, 0xAA, 0x9F, 0xD5, 0x6B, 0xCC, 0x3D, 0x04, 0x83, 0xE9, 0x26
};

/// <summary>
/// bit pattern for 12-15 from 16-19 (3/3)
/// </summary>
const uint8_t mByteArrLookupTable1C_3[] /*PROGMEM*/ = {
  0x0A, 0x02, 0x00, 0x0F, 0x06, 0x07, 0x0D, 0x08, 0x03, 0x0C, 0x0B, 0x05, 0x09, 0x01, 0x04, 0x0E,
  0x02, 0x09, 0x05, 0x0D, 0x0C, 0x0E, 0x0F, 0x08, 0x06, 0x07, 0x0B, 0x01, 0x00, 0x0A, 0x04, 0x03,
  0x08, 0x00, 0x0D, 0x0F, 0x01, 0x0C, 0x03, 0x06, 0x0B, 0x04, 0x09, 0x05, 0x0A, 0x07, 0x02, 0x0E,
  0x03, 0x0D, 0x00, 0x0C, 0x09, 0x06, 0x0F, 0x0B, 0x01, 0x0E, 0x08, 0x0A, 0x02, 0x07, 0x04, 0x05
};

#define Byte0  Bytes[0]
#define Byte1  Bytes[1]
#define Byte2  Bytes[2]
#define Byte3  Bytes[3]

union ByteUInt {
  uint8_t Bytes[4];
  uint32_t  FullUint;
};

/// Container, wich contains all former global vars
struct DataContainer {
  ByteUInt mByteUint1;  // Registers R12 to R15
  ByteUInt mByteUint2;  // Registers R08 to R0A
  ByteUInt mByteUint3;  // Registers R0B to R0E
  ByteUInt mByteUint4;  // Registers R1C to R1E
  uint8_t mByteUpperTime2; // mByteR1B;
  uint32_t mUintLowerTime;
};


uint8_t flipByte(uint8_t aByte) {
  uint8_t result = 0;
  uint8_t source = aByte;
  for(int i=0; i<8; i++) {
    result >>= 1;
    result |= source&0x80;
    source <<= 1;
  }
  return result;
}

void CopyTimeToByteUint(uint8_t* data, uint8_t* key, struct DataContainer* container) {
  container->mByteUint1.FullUint = 0;
  container->mByteUint2.FullUint = 0;
  container->mByteUint3.FullUint = 0;
  container->mUintLowerTime = 0;
  container->mByteUpperTime2 = 0;

  for (int i = 0; i < 4; i++) {
      container->mUintLowerTime <<= 8;
      container->mUintLowerTime |= key[3 - i];
  }
  container->mByteUpperTime2 = key[4];

  // copy R
  container->mByteUint3.Byte0 = data[2];
  container->mByteUint3.Byte1 = data[3];
  container->mByteUint3.Byte2 = data[4];
  container->mByteUint3.FullUint >>= 4;

  // copy L
  container->mByteUint2.Byte0 = data[0];
  container->mByteUint2.Byte1 = data[1];
  container->mByteUint2.Byte2 = (uint8_t)(data[2] & 0x0F);
}


void ShiftTimeRight(int round, struct DataContainer* container) {
  int count;
  uint8_t tmp;

  if ((round == 16) || (round == 8) || (round == 7) || (round == 3))
      count = 2;
  else
      count = 1;

  while (count-- != 0) {
      tmp = 0;
      if ((container->mUintLowerTime & (uint32_t)0x00100000) != 0)         // save time bit 20
          tmp = 1;

      container->mUintLowerTime &= (uint32_t)0xFFEFFFFF;
      if ((container->mUintLowerTime & 1) != 0)
          container->mUintLowerTime |= (uint32_t)0x00100000;       // copy time bit 0 to time bit 19
      container->mUintLowerTime >>= 1;             // time >>= 1

      if ((container->mByteUpperTime2 & 1) != 0)
          container->mUintLowerTime |= (uint32_t)0x80000000;
      container->mByteUpperTime2 >>= 1;
      if (tmp != 0)
          container->mByteUpperTime2 |= 0x80;          // insert time bit 20 to time bit 39
  }
}

void ExpandR(struct DataContainer* container) {
  uint32_t tmp;

  container->mByteUint3.FullUint &= 0x000FFFFF;      // clear 0D(4-7),0E
  tmp = 0x00100000;         // and set bits form 0B-0D(0-3)
  for (int i = 0; i < 12; i++) {
      if ((container->mByteUint3.FullUint & mUintArrBitPattern12[i]) != 0)
          container->mByteUint3.FullUint |= tmp;
      tmp <<= 1;
  }
}

void CompressKey(struct DataContainer* container) {
  uint32_t tmp = (uint32_t)0x00000001;         // and set bits from 16-1A (time)

  container->mByteUint1.FullUint = 0;          // clear 12-15
  for (int i = 0; i < 30; i++) {
      if ((container->mUintLowerTime & mUintArrBitPattern30_1[i]) != 0 || (container->mByteUpperTime2 & mUintArrBitPattern30_2[i]) != 0)
          container->mByteUint1.FullUint |= tmp;
      tmp <<= 1;
  }
}

void DoSbox(struct DataContainer* container) {
  uint8_t tmp, helper; //mByteR1B;

  helper = container->mByteUint1.Byte3;                // R1B = R15;
  container->mByteUint1.Byte3 = container->mByteUint1.Byte2;      // R15 = R14

  // INNER LOOP
  for (int i = 5; i > 0; i--) {
      if ((i & 1) == 0) { // round 4,2
          tmp = (uint8_t)(container->mByteUint1.Byte0 >> 4);    // swap R12
          tmp |= (uint8_t)((container->mByteUint1.Byte0 & 0x0f) << 4);
          container->mByteUint1.Byte0 = tmp;
      }
      container->mByteUint1.Byte3 &= 0xF0;           // set R1C
      tmp = (uint8_t)((container->mByteUint1.Byte0 & 0x0F) | container->mByteUint1.Byte3);

      if ((i & 4) != 0)
          tmp = mByteArrLookupTable1C_1[(tmp & 0x3F)];

      if ((i & 2) != 0)
          tmp = mByteArrLookupTable1C_2[(tmp & 0x3F)];

      else if (i == 1)
          tmp = mByteArrLookupTable1C_3[(tmp & 0x3F)];

      if ((i & 1) != 0)
          container->mByteUint4.Byte0 = (uint8_t)(tmp & 0x0F);
      else
          container->mByteUint4.Byte0 |= (uint8_t)(tmp & 0xF0);

      if ((i & 1) == 0) { // copy 14->13->12, 1C->1E->1D
          tmp = container->mByteUint1.Byte3;
          container->mByteUint1.FullUint >>= 8;
          container->mByteUint1.Byte3 = tmp;
          container->mByteUint4.FullUint <<= 8;
      }

      container->mByteUint1.Byte3 >>= 1;         // rotate R1B>R15 twice
      if ((helper & 1) != 0)
          container->mByteUint1.Byte3 |= 0x80;
      helper >>= 1;

      container->mByteUint1.Byte3 >>= 1;
      if ((helper & 1) != 0)
          container->mByteUint1.Byte3 |= 0x80;
      helper >>= 1;
  } // end of inner loop
}

void DoPbox(struct DataContainer* container) {
  uint32_t tmp = (uint32_t)0x00000001;         // and set bits from 1C-1E (result from F)

  container->mByteUint1.FullUint = (uint32_t)0xFF000000;     // clear 12-14
  for (int i = 0; i < 20; i++) {
      if ((container->mByteUint4.FullUint & mUintArrBitPattern20[i]) != 0)
          container->mByteUint1.FullUint |= tmp;
      tmp <<= 1;
  }
}


void Decrypt(uint8_t* cipher, uint8_t* key, uint8_t* result) {
  DataContainer container;

  CopyTimeToByteUint(cipher, key, &container);

  // OUTER LOOP 1
  for (int i = 16; i > 0; i--) {
    ShiftTimeRight(i, &container);
    ExpandR(&container);
    CompressKey(&container);

    // expR XOR compr.Key
    container.mByteUint1.FullUint ^= container.mByteUint3.FullUint; // 12-15 XOR 0B-0E
    container.mByteUint3.Byte2 &= 0x0F;       // clear 0D(4-7)

    DoSbox(&container);
    DoPbox(&container);

    // L XOR P-Boxed Round-Key (L')
    container.mByteUint1.FullUint ^= container.mByteUint2.FullUint;

    // L = R
    container.mByteUint2.FullUint = container.mByteUint3.FullUint & (uint32_t)0x00FFFFFF;

    // R = L'
    container.mByteUint3.FullUint = container.mByteUint1.FullUint & (uint32_t)0x00FFFFFF;
  } // end of outer loop 1

  container.mByteUint3.FullUint <<= 4;
  container.mByteUint2.Byte2 &= 0x0F;
  container.mByteUint2.Byte2 |= (uint8_t)(container.mByteUint3.Byte0 & 0xF0);

  //R0B0C0D0E.uint8_t.R0D |= (R08090A.uint8_t.R08 & 0xF0);
  result[0] = container.mByteUint2.Byte0;
  result[1] = container.mByteUint2.Byte1;
  result[2] = container.mByteUint2.Byte2;
  result[3] = container.mByteUint3.Byte1;
  result[4] = container.mByteUint3.Byte2;
}


bool GetWeatherInfo(uint8_t* aWeatherInfo) {
  bool lValid = false;
  int uiBitCnt;
  uint8_t ucTemp = 0;
  uint8_t cipher[5];
  uint8_t key[5];
  uint8_t PlainBytes[5];
  uiBitCnt = 0;
  int uiCnt = 1;

  for (; uiCnt < 42; uiCnt++) {
      if (uiCnt != 7) {
          ucTemp = (uint8_t)(ucTemp >> 1);
          if (mWeatherData[uiCnt] == 1)
              ucTemp |= 0x80;
          uiBitCnt++;
          if ((uiBitCnt & 7) == 0)
              cipher[(uiBitCnt >> 3) - 1] = ucTemp;
      }
  }
  uiBitCnt=0;
  for (; uiCnt < 82; uiCnt++) {
          ucTemp = (uint8_t)(ucTemp >> 1);
          if (mWeatherData[uiCnt] == 1)
              ucTemp |= 0x80;
          uiBitCnt++;
          if ((uiBitCnt & 7) == 0)
              key[(uiBitCnt >> 3) - 1] = ucTemp;
  }

  Decrypt(cipher,key, PlainBytes);

  uint16_t checkSum;
  checkSum = PlainBytes[2] & 0x0Fu;
  checkSum <<= 8;
  checkSum |= PlainBytes[1];
  checkSum <<= 4;
  checkSum |= (uint16_t)(PlainBytes[0] >> 4);

  if (checkSum == 0x2501) {
    aWeatherInfo[0] = (PlainBytes[3]&0x0F)<<4;
    aWeatherInfo[0] |= (PlainBytes[2]&0xF0)>>4;
    aWeatherInfo[0] = flipByte(aWeatherInfo[0]);

    aWeatherInfo[1] = (PlainBytes[4]&0x0F)<<4;
    aWeatherInfo[1] |= (PlainBytes[3]&0xF0)>>4;
    aWeatherInfo[1] = flipByte(aWeatherInfo[1]);

    aWeatherInfo[2] = (PlainBytes[0]&0x0F)<<4;
    aWeatherInfo[2] |= (PlainBytes[4]&0xF0)>>4;
    aWeatherInfo[2] = flipByte(aWeatherInfo[2]);
    aWeatherInfo[2] &= 0xFC;
    aWeatherInfo[2] |= 0x02;
    lValid = true;
  }

  return lValid;
}


int weatherunbinary(int index, int a, int b) {
  int val = 0;
  for (int i = 0; i < 16; i++) {
    if (! weathermemory[index].substring(a, b) [i]) {
      break;
    }
    val <<= 1;
    val |= (weathermemory[index].substring(a, b) [i] == '1') ? 1 : 0;
  }
  return val;
}


int binStrToDec( String input ) {
  int out = 0;
  int len = input.length();
  for( int i=0; i<len; i++ ) {
    if( input.substring( i, i+1 ) == "1" ) {
      out += pow(2, i);
    }
  }
  log_d("IN: %s, OUT: %d", input.c_str(), out);
  return out;
}


String decToBinStr( uint8_t aInfo[3] ) {
  String out = "";
  out.reserve(25);
  for(uint8_t a=0;a<3;a++) {
    for (int i = 7; i >=0; i--) {
      //bits[i] = (aInfo[a] >> i) & 1;
      out += String( (aInfo[a] >> i) & 1 ); // bits[i];
    }
  }
  return out;
}




// TODO: move this to UI.h
void showWeather() {

  char str[255];
  int fourdayforecast = 0;
  int twodayforecast = 0;

  /*
    0110  Day critical weather
        0001  Night critical weather
            0001  Extremeweather
                011  Rainprobability
                   0  Anomaly
                    101101  Temperature
  */
  log_d("Deciphered Weather Data: %s", meteodata.c_str());
  int dcw  = binStrToDec( meteodata.substring(0, 4) );   // Day critical weather
  int ncw  = binStrToDec( meteodata.substring(4, 8) );   // Night critical weather
  int xwh  = binStrToDec( meteodata.substring(8, 12) );  // Extremeweather
  int anm1 = binStrToDec( meteodata.substring(8, 10) );  // Weather anomality 1
  int anm2 = binStrToDec( meteodata.substring(10, 12) ); // Weather anomality 2
  int rpb  = binStrToDec( meteodata.substring(12, 15) ); // Rainprobability
  int anm  = binStrToDec( meteodata.substring(15, 16) ); // Anomaly
  int tem  = binStrToDec( meteodata.substring(16, 22) ); // Temperature

  SpriteSheetIcon icon1 = weather[dcw].icon;
  SpriteSheetIcon icon2 = atomic;
  SpriteSheetIcon icon3 = weather[ncw].icon;

  log_d("dcw=%s, ncw=%s, xwh=%s, rpb=%s, anm=%s, tem=%d%sC", weather[dcw].label, weather[ncw].label, winddirection[xwh].label, rainprobability[rpb].label, anm==0 ? "yes" : "no", tem-22, String( degreeSign ).c_str() );

  //DCFDateTime mDcfTime = mDcf.GetTime();
  // uint8_t yOff, m, d, hh, mm;

  fourdayforecast = ((dcfHour) % 3) * 20;

  if (dcfMinute > 0) {
    fourdayforecast += (dcfMinute - 1) / 3;
  }
  if (dcfMinute > 0) {
    twodayforecast = ((((dcfHour) * 60) + (dcfMinute - 1)) % 90) / 3;
  } else {
    twodayforecast = ((dcfHour) * 60);
  }
  twodayforecast += 60;

  log_d("four: %d, two: %d", fourdayforecast, twodayforecast);
  Serial.println("*******************************"); // 31 stars
  delay(10);

  if (dcfHour < 21) { //Between 21:00-23:59 significant weather & temperature is for cities 60-89 wind and wind direction for cities 0-59.
    if ((dcfHour) % 6 < 3) {
      weathermemory[fourdayforecast] = meteodata;  // Extreme weather is valid from this hour but also +3 hour
      significantweather[fourdayforecast] = weatherunbinary(fourdayforecast, 8, 12);
    }
    sprintf( str, "Area 4day f/c =   %d %s (%s) ", fourdayforecast, cities[fourdayforecast].name, cities[fourdayforecast].country.name); Serial.println( str );
    sprintf( str, "4day f/c %5s =      %d ", String( (((dcfHour) % 6) > 2) ? "Night" : "Day" ).c_str(), int( (dcfHour / 6) + 1 ) ); Serial.println( str );
    sprintf( str, "Day Forecast      =   %s %02x %s ", meteodata.substring(0, 4).c_str(), dcw, weather[dcw].label); Serial.print( str );
    if (dcw == 5 || dcw == 6 || dcw == 13 || dcw == 7) {
      if (significantweather[fourdayforecast] == 1 || significantweather[fourdayforecast] == 2) {
        sprintf( str, "%s%s with thunderstorms ", (dcw != 6) ? "Heavy " : "", weather[dcw].label); Serial.println( str );
      } else Serial.println();
    } else Serial.println();
    sprintf( str, "Night Forecast    =   %s %02x ", meteodata.substring(4, 8).c_str(), ncw); Serial.print( str );
    if (ncw == 5 || ncw == 6 || ncw == 13 || ncw == 7) {
      if (significantweather[fourdayforecast] == 1 || significantweather[fourdayforecast] == 3) {
        sprintf( str, "%s%s with thunderstorms ", (ncw != 6) ? "Heavy " : "", weather[ncw].label);
      } else {
        sprintf( str, "%s ", weather[ncw].label);
      }
    } else {
      if (ncw == 1) {
        sprintf( str, "Clear");
      } else {
        sprintf( str, "%s ", weather[ncw].label);
      }
    }
    Serial.println( str );
    if ((dcfHour) % 6 < 3) {
      sprintf( str, "Extremeweather    =   "); Serial.print( str );
      if (xwh == 0) { // DI=0 and WA =0
        sprintf( str, "%s %02x %s ", weathermemory[fourdayforecast].substring(8, 12).c_str(), significantweather[fourdayforecast], heavyweather[significantweather[fourdayforecast]].label); Serial.println( str );
      } else {
        sprintf( str, "%s %02x %s ", meteodata.substring(8, 10).c_str(), anm1, anomaly1[anm1].label ); Serial.println( str );
        sprintf( str, "%s %02x %s ", meteodata.substring(10, 12).c_str(), anm2, anomaly2[anm2].label ); Serial.println( str );
      }
      sprintf( str, "Rainprobability   =    %s %02x %s ", meteodata.substring(12, 15).c_str(), rpb, rainprobability[rpb].label ); Serial.println( str );
    }
    if ((dcfHour) % 6 > 2) {
      icon2 = winddirection[xwh].icon;
      sprintf( str, "Winddirection     =   %s %02x %s ", meteodata.substring(8, 12).c_str(), xwh, winddirection[xwh].label); Serial.println( str );
      sprintf( str, "Windstrength      =    %s %02x %s  Bft ", meteodata.substring(12, 15).c_str(), rpb, windstrength[rpb].label); Serial.println( str );
      sprintf( str, "Extremeweather    =   %s %02x %s ", weathermemory[fourdayforecast].substring(8, 12).c_str(), significantweather[fourdayforecast], heavyweather[significantweather[fourdayforecast]].label); Serial.println( str );
    }
    sprintf( str, "Weather Anomality =      %s %02x %s ", meteodata.substring(15, 16).c_str(), anm, (anm == 1) ? "Yes" : "No"); Serial.println( str );

  } else {
    //Serial.printf("Area 4dayforecast =   %d %s\n", fourdayforecast, town[fourdayforecast].c_str());
    sprintf( str, "Area 4dayforecast =   %d %s (%s) ", fourdayforecast, cities[fourdayforecast].name, cities[fourdayforecast].country.name); Serial.println( str );
    sprintf( str, "fourday f/c %5s =      %d ", (((dcfHour) % 6) > 2) ? "Night" : "Day", (dcfHour / 6) + 1); Serial.println( str );
    icon2 = winddirection[xwh].icon;
    sprintf( str, "Winddirection     =   %s %02x %s ", meteodata.substring(8, 12).c_str(), xwh, winddirection[xwh].label); Serial.println( str );
    sprintf( str, "Windstrength      =    %s %02x %s  Bft ", meteodata.substring(12, 15).c_str(), rpb, windstrength[rpb].label); Serial.println( str );
    sprintf( str, "Area 2dayforecast =   %d %s (%s) ", twodayforecast, cities[twodayforecast].name, cities[twodayforecast].country.name); Serial.println( str );
    sprintf( str, "twoday f/c day    =      %d ", (((dcfHour - 21) * 60 + dcfMinute) > 90) ? 2 : 1); Serial.println( str );
    sprintf( str, "Day               =   %s %02x %s ", meteodata.substring(0, 4).c_str(), dcw, weather[dcw].label); Serial.println( str );
    sprintf( str, "Night             =   %s %02x %s ", meteodata.substring(4, 8).c_str(), ncw, (ncw == 1) ? "Clear" : weather[ncw].label); Serial.println( str );
    sprintf( str, "Weather Anomality =      %s %02x %s ", meteodata.substring(15, 16).c_str(), anm, (anm == 1) ? "Yes" : "No"); Serial.println( str );
  }

  String temperatureStr = "xxxxx yyyC xxxxmum";

  if (tem == 0) {
    temperatureStr = "<-21 " + String(degreeSign) + "C";
  } else if (tem == 63) {
    temperatureStr = ">40 " + String(degreeSign) + "C";
  } else {
    temperatureStr = String(tem - 22) + String(degreeSign) + "C";
  }
  // Night temperature is minimum
  // Daytime temperature is daytime maximum
  if (((dcfHour) % 6) > 2 && (dcfHour < 21)) {
    temperatureStr += " minimum";
  } else {
    temperatureStr += " maximum";
  }
  sprintf( str, "Temperature       = %s %02x %s ", meteodata.substring(16, 22).c_str(), tem, temperatureStr.c_str()); Serial.println( str );
  sprintf( str, "Decoder status    =     %s    %s ", meteodata.substring(22, 24).c_str(), meteodata.substring(22, 24) == "10" ? "OK" : "NOT OK"); Serial.println( str );

  if(  ( twodayforecast <=90 && strcmp( cities[twodayforecast].name, watchedCity ) == 0 )
    || ( fourdayforecast<=90 && strcmp( cities[fourdayforecast].name, watchedCity ) == 0)
  ) {
    Serial.println("City match, should update scroll and icons !!");
    //updateScroll( citiesNames );
    updateIcons( icon1, icon2, icon3 );
  } else {
    Serial.println("Forecast for this country+city will not be displayed");
  }

}
