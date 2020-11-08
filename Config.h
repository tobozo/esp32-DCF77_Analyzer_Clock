#ifndef _CONFIG_H
#define _CONFIG_H

// enable either one or none of those:

#define CONFIG_WROVER_KIT
//#define CONFIG_TTGO // default
//#define CONFIG_BRETT

#if defined CONFIG_WROVER_KIT
  #include "configs/Config.WROVER_KIT.h"
#elif defined CONFIG_TTGO
  #include "configs/Config.TTGO-T1.h"
#elif defined CONFIG_BRETT
  #include "configs/Config.Brett.h"
#else
  #include "configs/Config.Default.h"
#endif

#if defined SPEAKER_PIN
  #warning USING SPEAKER
  #define USE_SPEAKER
#endif

#endif // _CONFIG_H
