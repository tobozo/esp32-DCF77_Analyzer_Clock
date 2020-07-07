
// code inspired by https://github.com/FroggySoft/AlarmClock and http://arduino-projects4u.com/home-weather-station/

#define WEATHER_SIZE      82
#define WEATHER_INFO_SIZE  3

// from UI.h
extern void updateScroll( String text );

int fourdayforecast = 0;
int twodayforecast = 0;
int significantweather[60] = {0};
String weathermemory[60];
int mWeatherData[WEATHER_SIZE];
byte mWeatherArea;
byte mWeatherSection;
extern int dcfDST, dcfMinute, dcfHour;

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
  return strcmp(lhs.code, rhs.code)==0 && strcmp(lhs.name, rhs.name)==0;/* your comparison code goes here */
}
bool operator==(const countryBycode& lhs, countryBycode* rhs) {
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


countryBycode France       = {"F",   "France"};
countryBycode Belgium      = {"B",   "Belgium"};
countryBycode Switzerland  = {"CH",  "Switzerland"};
countryBycode Germany      = {"D",   "Germany"};
countryBycode GreatBritain = {"GB",  "Great Britain"};
countryBycode Denmark      = {"DK",  "Denmark"};
countryBycode Italy        = {"I",   "Italy"};
countryBycode Netherlands  = {"NL",  "Netherlands"};
countryBycode Austria      = {"A",   "Austria"};
countryBycode Slovakia     = {"SK",  "Slovakia"};
countryBycode Czechia      = {"CZ",  "Czechia"};
countryBycode Sweden       = {"S",   "Sweden"};
countryBycode Norway       = {"N",   "Norway"};
countryBycode Hungary      = {"H",   "Hungary"};
countryBycode Spain        = {"E",   "Spain"};
countryBycode Andorra      = {"AND", "Andorra"};
countryBycode Portugal     = {"P",   "Portugal"};
countryBycode Ireland      = {"IRL", "Ireland"};
countryBycode Poland       = {"PL",  "Poland"};
countryBycode Croatia      = {"HR",  "Croatia"};

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
  {Poland, "Warszawa"}, {Poland, "Krakow"}, {Sweden, "Umea"}, {Sweden, "Oestersund"}, {Switzerland, "Samedan"}, {Croatia, "Zagreb"}, {Switzerland, "Zermatt"}, {Croatia, "Split"}, {nullptr, nullptr}
};


const countryBycode *findCountryByName( const char* countryName ) {
  int citiesLength = (sizeof(cities)/sizeof(cityByCountry)) -1;
  for( int i=0; i< citiesLength; i++ ) {
    if( strcmp( cities[i].country.name, countryName ) == 0 ) {
      return &cities[i].country;
    }
  }
  return nullptr;
}

const char *findCityByName( const char* cityName ) {
  int citiesLength = (sizeof(cities)/sizeof(cityByCountry)) -1;
  for( int i=0; i< citiesLength; i++ ) {
    if( strcmp( cities[i].name, cityName ) == 0 ) {
      return cityName;
    }
  }
  return nullptr;
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


/*
countryBycode countries[] = {
  France, Belgium, Switzerland, Germany, GreatBritain, Denmark, Italy, Netherlands, Austria, Slovakia, Czechia,
  Sweden, Norway, Hungary, Spain, Andorra, Portugal, Ireland, Poland, Croatia
};*/



void copyWeatherInfo( int* aMessage, byte aIndex ) {
  byte j=aIndex;
  for(byte i=0; i<14; i++) {
    mWeatherData[j++] = aMessage[i+1];
  }
}

bool addToWeatherInfo( int* aMessage ) {
  bool lCompleted = false;
  byte j = 0;
  if( aMessage[20]==1 ) {
    byte minute = dcfMinute;
    minute--;
    byte part = minute % 3;
    switch( part ) {
      case 0:  // first part
        // clear all old data
        for( byte i=0; i<WEATHER_SIZE; i++ )
          mWeatherData[i]=0;
        //copy received data into correct part
        copyWeatherInfo( aMessage,0 );
      break;
      case 1: // second part
        copyWeatherInfo( aMessage,14 );
        j = 14+14+14;
        // add time info
        for(byte i=21; i<28; i++)   // add minutes
          mWeatherData[j++] = aMessage[i];
        j++;
        for(byte i=29; i<35; i++)   // add hours
          mWeatherData[j++] = aMessage[i];
        j+=2;
        for(byte i=36; i<42; i++)   // add day
          mWeatherData[j++] = aMessage[i];
        j+=2;
        for(byte i=45; i<50; i++)   // add month
          mWeatherData[j++] = aMessage[i];
        for(byte i=42; i<45; i++)   // add week day
          mWeatherData[j++] = aMessage[i];
        for(byte i=50; i<58; i++)   // add year
          mWeatherData[j++] = aMessage[i];
      break;
      case 2: // third part
        copyWeatherInfo( aMessage,14+14 );
        lCompleted = true;
      break;
    }
  }
  return lCompleted;
}


byte GetWeatherArea() {
  return mWeatherArea;
}


byte GetWeatherSection() {
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


byte getArea()  {
  unsigned int minutes = getMinutesSince2200();
  // each block of data takes 3 minutes
  // in total 60 areas
  int area = minutes%(60*3);
  area /= 3;
  area--;
  return area;
}


byte getSection() {
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
const byte mUintArrBitPattern30_2[] /*PROGMEM*/ = {
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
 const byte mByteArrLookupTable1C_1[] /*PROGMEM*/ = {
  0xBB, 0x0E, 0x22, 0xC5, 0x73, 0xDF, 0xF7, 0x6D, 0x90, 0xE9, 0xA1, 0x38, 0x1C, 0x84, 0x4A, 0x56,
  0x64, 0x8D, 0x28, 0x0B, 0xD1, 0xBA, 0x93, 0x52, 0x1C, 0xC5, 0xA7, 0xF0, 0xE9, 0x7F, 0x36, 0x4E,
  0xC1, 0x77, 0x3D, 0xB3, 0xAA, 0xE0, 0x0C, 0x6F, 0x14, 0x88, 0xF6, 0x2B, 0xD2, 0x99, 0x5E, 0x45,
  0x1F, 0x70, 0x96, 0xD3, 0xB3, 0x0B, 0xFC, 0xEE, 0x81, 0x42, 0xCA, 0x34, 0xA5, 0x58, 0x29, 0x67
};

/// <summary>
/// bit pattern for 12-15 from 16-19 (2/3)
/// </summary>
const byte mByteArrLookupTable1C_2[] /*PROGMEM*/ = {
  0xAB, 0x3D, 0xFC, 0x74, 0x65, 0xE6, 0x0E, 0x4F, 0x97, 0x11, 0xD8, 0x59, 0x83, 0xC2, 0xBA, 0x20,
  0xC5, 0x1B, 0xD2, 0x58, 0x49, 0x37, 0x01, 0x7D, 0x93, 0xFA, 0xE0, 0x2F, 0x66, 0xB4, 0xAC, 0x8E,
  0xB7, 0xCC, 0x43, 0xFF, 0x58, 0x66, 0xEB, 0x35, 0x82, 0x2A, 0x99, 0xDD, 0x00, 0x71, 0x14, 0xAE,
  0x4E, 0xB1, 0xF7, 0x70, 0x18, 0x52, 0xAA, 0x9F, 0xD5, 0x6B, 0xCC, 0x3D, 0x04, 0x83, 0xE9, 0x26
};

/// <summary>
/// bit pattern for 12-15 from 16-19 (3/3)
/// </summary>
const byte mByteArrLookupTable1C_3[] /*PROGMEM*/ = {
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
  byte Bytes[4];
  uint32_t  FullUint;
};

/// Container, wich contains all former global vars
struct DataContainer {
  ByteUInt mByteUint1;  // Registers R12 to R15
  ByteUInt mByteUint2;  // Registers R08 to R0A
  ByteUInt mByteUint3;  // Registers R0B to R0E
  ByteUInt mByteUint4;  // Registers R1C to R1E
  byte mByteUpperTime2; // mByteR1B;
  uint32_t mUintLowerTime;
};


byte flipByte(byte aByte) {
  byte result = 0;
  byte source = aByte;
  for(int i=0; i<8; i++) {
    result >>= 1;
    result |= source&0x80;
    source <<= 1;
  }
  return result;
}

void CopyTimeToByteUint(byte* data, byte* key, struct DataContainer* container) {
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
  container->mByteUint2.Byte2 = (byte)(data[2] & 0x0F);
}


void ShiftTimeRight(int round, struct DataContainer* container) {
  int count;
  byte tmp;

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
  byte tmp, helper; //mByteR1B;

  helper = container->mByteUint1.Byte3;                // R1B = R15;
  container->mByteUint1.Byte3 = container->mByteUint1.Byte2;      // R15 = R14

  // INNER LOOP
  for (int i = 5; i > 0; i--) {
      if ((i & 1) == 0) { // round 4,2
          tmp = (byte)(container->mByteUint1.Byte0 >> 4);    // swap R12
          tmp |= (byte)((container->mByteUint1.Byte0 & 0x0f) << 4);
          container->mByteUint1.Byte0 = tmp;
      }
      container->mByteUint1.Byte3 &= 0xF0;           // set R1C
      tmp = (byte)((container->mByteUint1.Byte0 & 0x0F) | container->mByteUint1.Byte3);

      if ((i & 4) != 0)
          tmp = mByteArrLookupTable1C_1[(tmp & 0x3F)];

      if ((i & 2) != 0)
          tmp = mByteArrLookupTable1C_2[(tmp & 0x3F)];

      else if (i == 1)
          tmp = mByteArrLookupTable1C_3[(tmp & 0x3F)];

      if ((i & 1) != 0)
          container->mByteUint4.Byte0 = (byte)(tmp & 0x0F);
      else
          container->mByteUint4.Byte0 |= (byte)(tmp & 0xF0);

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


void Decrypt(byte* cipher, byte* key, byte* result) {
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
  container.mByteUint2.Byte2 |= (byte)(container.mByteUint3.Byte0 & 0xF0);

  //R0B0C0D0E.byte.R0D |= (R08090A.byte.R08 & 0xF0);
  result[0] = container.mByteUint2.Byte0;
  result[1] = container.mByteUint2.Byte1;
  result[2] = container.mByteUint2.Byte2;
  result[3] = container.mByteUint3.Byte1;
  result[4] = container.mByteUint3.Byte2;
}


bool GetWeatherInfo(byte* aWeatherInfo) {
  bool lValid = false;
  int uiBitCnt;
  byte ucTemp = 0;
  byte cipher[5];
  byte key[5];
  byte PlainBytes[5];
  uiBitCnt = 0;
  int uiCnt = 1;

  for (; uiCnt < 42; uiCnt++) {
      if (uiCnt != 7) {
          ucTemp = (byte)(ucTemp >> 1);
          if (mWeatherData[uiCnt] == 1)
              ucTemp |= 0x80;
          uiBitCnt++;
          if ((uiBitCnt & 7) == 0)
              cipher[(uiBitCnt >> 3) - 1] = ucTemp;
      }
  }
  uiBitCnt=0;
  for (; uiCnt < 82; uiCnt++) {
          ucTemp = (byte)(ucTemp >> 1);
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


int weatherunbinary(int a, int b) {
  int val = 0;
  for (int i = 0; i < 16; i++) {
    if (! weathermemory[fourdayforecast].substring(a, b) [i]) {
      break;
    }
    val <<= 1;
    val |= (weathermemory[fourdayforecast].substring(a, b) [i] == '1') ? 1 : 0;
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


String decToBinStr( byte aInfo[3] ) {
  String out = "";
  out.reserve(25);
  for(byte a=0;a<3;a++) {
    for (int i = 7; i >=0; i--) {
      //bits[i] = (aInfo[a] >> i) & 1;
      out += String( (aInfo[a] >> i) & 1 ); // bits[i];
    }
  }
  return out;
}


static void updateIcons( SpriteSheetIcon icon1, SpriteSheetIcon icon2, SpriteSheetIcon icon3 );
static bool weatherReady = false;

// TODO: move this to UI.h
void showWeather() {

  String out = "";
  String citiesNames = "";
  char str[255];
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
  int anm1 = binStrToDec( meteodata.substring(8, 10) );
  int anm2 = binStrToDec( meteodata.substring(10, 12) );
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
  Serial.print("");
  delay(10);

  if (dcfHour < 21) { //Between 21:00-23:59 significant weather & temperature is for cities 60-89 wind and wind direction for cities 0-59.
    if ((dcfHour) % 6 < 3) {
      weathermemory[fourdayforecast] = meteodata;  // Extreme weather is valid from this hour but also +3 hour
      significantweather[fourdayforecast] = weatherunbinary(8, 12);
    }



    sprintf( str, "Area 4day f/c =   %d %s (%s) ", fourdayforecast, cities[fourdayforecast].name, cities[fourdayforecast].country.name);
    Serial.println( str );
    out += String(str);
    sprintf( str, "4day f/c %5s =      %d ", String( (((dcfHour) % 6) > 2) ? "Night" : "Day" ).c_str(), int( (dcfHour / 6) + 1 ) );
    Serial.println( str );
    out += String(str);
    sprintf( str, "Day               =   %s %02x ", meteodata.substring(0, 4).c_str(), dcw);
    Serial.print( str );
    out += String(str);

    sprintf( str, "%s ", weather[dcw].label);
    if (dcw == 5 || dcw == 6 || dcw == 13 || dcw == 7) {
      if (significantweather[fourdayforecast] == 1 || significantweather[fourdayforecast] == 2) {
        sprintf( str, "%s%s with thunderstorms ", (dcw != 6) ? "Heavy " : "", weather[dcw].label);
      }
    }
    Serial.println( str );
    out += String(str);

    sprintf( str, "Night             =   %s %02x ", meteodata.substring(4, 8).c_str(), ncw);
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
    out += String(str);

    if ((dcfHour) % 6 < 3) {
      sprintf( str, "Extremeweather    =   ");
      Serial.print( str );
      out += String(str);
      if (xwh == 0) { // DI=0 and WA =0
        sprintf( str, "%s %02x %s ", weathermemory[fourdayforecast].substring(8, 12).c_str(), significantweather[fourdayforecast], heavyweather[significantweather[fourdayforecast]].label);
        Serial.println( str );
        out += String(str);
      } else {
        sprintf( str, "%s %02x %s ", meteodata.substring(8, 10).c_str(), anm1, anomaly1[anm1].label );
        Serial.println( str );
        out += String(str);
        sprintf( str, "%s %02x %s ", meteodata.substring(10, 12).c_str(), anm2, anomaly2[anm2].label );
        Serial.println( str );
        out += String(str);
      }
      sprintf( str, "Rainprobability   =    %s %02x %s ", meteodata.substring(12, 15).c_str(), rpb, rainprobability[rpb].label );
      Serial.println( str );
      out += String(str);
    }
    if ((dcfHour) % 6 > 2) {
      icon2 = winddirection[xwh].icon;
      sprintf( str, "Winddirection     =   %s %02x %s ", meteodata.substring(8, 12).c_str(), xwh, winddirection[xwh].label);
      Serial.println( str );
      out += String(str);
      sprintf( str, "Windstrength      =    %s %02x %s  Bft ", meteodata.substring(12, 15).c_str(), rpb, windstrength[rpb].label);
      Serial.println( str );
      out += String(str);
      sprintf( str, "Extremeweather    =   %s %02x %s ", weathermemory[fourdayforecast].substring(8, 12).c_str(), significantweather[fourdayforecast], heavyweather[significantweather[fourdayforecast]].label);
      Serial.println( str );
      out += String(str);
    }
    sprintf( str, "Weather Anomality =      %s %02x %s ", meteodata.substring(15, 16).c_str(), anm, (anm == 1) ? "Yes" : "No");
    Serial.println( str );
    out += String(str);

    sprintf( str, " 4 day: %s ", cities[fourdayforecast].name );
    citiesNames += str;


  } else {
    //Serial.printf("Area 4dayforecast =   %d %s\n", fourdayforecast, town[fourdayforecast].c_str());
    sprintf( str, "Area 4dayforecast =   %d %s (%s) ", fourdayforecast, cities[fourdayforecast].name, cities[fourdayforecast].country.name);
    Serial.println( str );
    out += String(str);
    sprintf( str, "fourday f/c %5s =      %d ", (((dcfHour) % 6) > 2) ? "Night" : "Day", (dcfHour / 6) + 1);
    Serial.println( str );
    out += String(str);
    icon2 = winddirection[xwh].icon;
    sprintf( str, "Winddirection     =   %s %02x %s ", meteodata.substring(8, 12).c_str(), xwh, winddirection[xwh].label);
    Serial.println( str );
    out += String(str);
    sprintf( str, "Windstrength      =    %s %02x %s  Bft ", meteodata.substring(12, 15).c_str(), rpb, windstrength[rpb].label);
    Serial.println( str );
    out += String(str);
    sprintf( str, "Area 2dayforecast =   %d %s (%s) ", twodayforecast, cities[twodayforecast].name, cities[twodayforecast].country.name);
    Serial.println( str );
    out += String(str);
    sprintf( str, "twoday f/c day    =      %d ", (((dcfHour - 21) * 60 + dcfMinute) > 90) ? 2 : 1);
    Serial.println( str );
    out += String(str);
    sprintf( str, "Day               =   %s %02x %s ", meteodata.substring(0, 4).c_str(), dcw, weather[dcw].label);
    Serial.println( str );
    out += String(str);
    sprintf( str, "Night             =   %s %02x %s ", meteodata.substring(4, 8).c_str(), ncw, (ncw == 1) ? "Clear" : weather[ncw].label);
    Serial.println( str );
    out += String(str);
    sprintf( str, "Weather Anomality =      %s %02x %s ", meteodata.substring(15, 16).c_str(), anm, (anm == 1) ? "Yes" : "No");
    Serial.println( str );
    out += String(str);

    sprintf( str, " 2 day: %s ", cities[twodayforecast].name );
    citiesNames += str;
  }

  String temperatureStr;

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
  sprintf( str, "Temperature       = %s %02x %s ", meteodata.substring(16, 22).c_str(), tem, temperatureStr.c_str());
  Serial.println( str );
  out += String(str);
  //sprintf( str, "Decoder status    =     %s    %s ", meteodata.substring(22, 24).c_str(), meteodata.substring(22, 24) == "10" ? "OK" : "NOT OK");
  //Serial.println( str );
  //out += String(str);
  #ifdef DCF77_DO_WEATHER

    if(  ( cities[twodayforecast].country  == watchedCountry && strcmp( cities[twodayforecast].name, watchedCity ) == 0 )
      || ( cities[fourdayforecast].country == watchedCountry && strcmp( cities[fourdayforecast].name, watchedCity ) == 0)
    ) {
      Serial.println("Country + city match, should update scroll and icons !!");
      //updateScroll( out );
      updateScroll( citiesNames );
      updateIcons( icon1, icon2, icon3 );
    } else {
      Serial.println("Forecast for this country+city will not be displayed");
    }
  #endif
}
