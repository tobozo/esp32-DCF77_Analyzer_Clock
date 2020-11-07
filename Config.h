#ifndef _CONFIG_H
#define _CONFIG_H


#define CONFIG_WROVER_KIT
//#define CONFIG_TTGO // default
//#define CONFIG_BRETT

#if defined CONFIG_WROVER_KIT
  #include "Config.WROVER_KIT.h"
#elif defined CONFIG_TTGO
  #include "Config.TTGO-T1.h"
#elif defined CONFIG_BRETT
  #include "Config.Brett.h"
#else
  #include "Config.Default.h"
#endif

#if defined SPEAKER_PIN
  #warning USING SPEAKER
  #define USE_SPEAKER
#endif

#endif // _CONFIG_H
