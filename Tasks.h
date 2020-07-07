#ifndef _TASKS_H_
#define _TASKS_H_

#include "Config.h"

// called from here but loaded later from UI.h
extern void checkButtons( void );
extern void initScroll( String text="                             Meteo is loading" );
extern void handleScroll();
extern void displayRtcTime();
extern void displayRtcDate();
extern void displayData( void );


void IRAM_ATTR int0handler() {
  DCFSignalState = digitalRead( DCF77PIN );
}


static void mainTask( void * param ) {
  log_e( "Starting main DCF77 signal task" );
  // Initialize DCF77 pulse interrupt on pin interrupt(DCF77PIN), looking for a change of the signal,
  // so either rising or falling edge pulses will trigger the interrupt handler and
  // execute the int0handler function.
  attachInterrupt(digitalPinToInterrupt(DCF77PIN), int0handler, CHANGE);
  while( true ) {
    // check if pulse direction is changed (rising or falling)
    if ( DCFSignalState != previousSignalState ) {
      previousSignalState = DCFSignalState; // 'reset' state of variable
      scanSignal(); // evaluate incoming pulse
    }
    vTaskDelay( 1 );
  }
}


#ifdef SPEAKER_PIN
  static void soundTask( void * param ) {
    log_e( "Starting sound task" );
    while( true ) {
      if(willBuzzNote!=0 && willBuzzDuration!=0) {
        Speaker.tone( willBuzzNote, willBuzzDuration );
        vTaskDelay( 1 );
        willBuzzDuration = 0;
        willBuzzNote = 0;
      }
      Speaker.update();
      vTaskDelay( 1 );
    }
  }
#endif


#ifdef USE_BUTTONS
  static void buttonsTask( void * param ) {
    //UIModes currentMode = UIMode;
    log_e("Starting buttons task");
    while( true ) {
      checkButtons();
      delay(50);
      /*
      if( currentMode != UIMode ) {
        takeMuxSemaphore();
        tft.fillScreen( TFT_BLACKISH );
        giveMuxSemaphore();
        currentMode = UIMode;
      }*/
    }
  }
#endif


static void doSecondlyTask() {

  displayRtcTime();

  switch ( second() ) {
    case 0:
      if ( dayTime == 1 && minute() == 0 ) {
        log_d( "new hour" );
      }
      if ( hour() == 00 && minute() == 00 ) {
        // do something at midnight
        log_d( "midnight" );
      }
      if( UIMode == DCF_CLOCK ) {
        displayRtcDate();
      }
      break;
    case 2:
      // hourly chime output: DEACTIVATE
      break;
    case 30:
      // display 'HI' on display for Hi temperature
      break;
    case 31:
    case 32:
      // display Max temperature on DCF display LED display
      break;
    case 33:
      // display 'LO' on display for Low temperature
      break;
    case 34:
    case 35:
      // display Min temperature on DCF display LED display
      break;
    case 36:
      // display 'CU' on display for Current temperature
      break;
    case 37:
      // read temperature, store in min/max memory and display current temperature
      //displayTemp();
      break;
  }// switch
}

/*
static void tasksEverySecond() {
  if ( second() != previousSecond ) {
    // 'reset' variable state
    previousSecond = second();
    doSecondlyTask();
  }
}*/


static void doMinutelyTask() {
  // display date, week LED's, week nr etc.
  if ( dcfValidSignal == true ) {
    if( UIMode == DCF_CLOCK ) {
      displayData();
    }
  }
}
/*
void tasksEveryMinute() {
  // display date, week LED's, week nr etc. if time is changed
  if ( minute() != previousMinute ) {
    // 'reset' state of variable
    previousMinute = minute();
    doMinutelyTask();
  }
}*/


static void doHourlyTask() {
  errorCounter = 0;
  // update error counter display
  //if( UIMode == DCF_CLOCK ) {
    LedDisplay( DisplayBufferBitError, "R", errorCounter );
  //}
}

void tasksEveryHour() {
  if ( hour() != previousHour ) {
    // 'reset' variable state
    previousHour = hour();
    doHourlyTask();
  }
}


static void secondlyTask( void *param ) {
  doSecondlyTask();
  vTaskDelete( NULL );
}


static void minutelyTask( void *param ) {
  doMinutelyTask();
  vTaskDelete( NULL );
}


static void hourlyTask( void *param ) {
  doHourlyTask();
  vTaskDelete( NULL );
}


void finalizeBufferTask( void *param ) {
  finalizeBuffer();
  vTaskDelete( NULL );
}



static void timerTasks( void *param ) {
  log_w( "Entering timers task" );
  #ifdef DCF77_DO_WEATHER
    //initScroll();
  #endif
  byte lastPushCounter = 0;
  while(1) {
    if ( second() != previousSecond ) {
      previousSecond = second();
      xTaskCreatePinnedToCore( secondlyTask, "secondlyTask", 2048, NULL, 1, NULL, 0 );
      vTaskDelay( 20 );
      continue;
    }
    if ( minute() != previousMinute ) {
      previousMinute = minute();
      xTaskCreatePinnedToCore( minutelyTask, "minuteTask", 2048, NULL, 1, NULL, 0 );
      vTaskDelay( 20 );
      continue;
    }
    if (hour() != previousHour) {
      previousHour = hour();
      xTaskCreatePinnedToCore( hourlyTask, "hourlyTask", 2048, NULL, 1, NULL, 0 );
      vTaskDelay( 20 );
      continue;
    }
    if( UIMode == COOK_TIMER ) {
      cookTimerloop();
    } else {
      #ifdef DCF77_DO_WEATHER
      if( weatherReady ) {
        showWeather();
        weatherReady = false;
      }
      #endif
    }
    if( lastPushCounter != longPushCounter ) {
      lastPushCounter = longPushCounter;

      takeMuxSemaphore();
      if( longPushCounter == 0 ) {
        sprite.fillRect( sprite.width()-15, 30, 10, 31, TFT_BLACK );
      } else {
        sprite.fillRect( sprite.width()-15, 60-longPushCounter, 10, longPushCounter, TFT_BLUE );
      }
      giveMuxSemaphore();

    }
    // retrieve last buffer changes

/*
    takeMuxSemaphore();
    MainSprite.createSprite( tft.width(), tft.height() );
    MainSprite.fillSprite( TFT_BLACK );
    MainSprite.setSwapBytes( true );
    MainSprite.pushImage( 0, 0, tft.width(), tft.height(), (const uint16_t*)sprite.frameBuffer(1) );
    giveMuxSemaphore();
*/
    #ifdef DCF77_DO_WEATHER
      //handleScroll();
    #endif

    takeMuxSemaphore();
    sprite.pushSprite(0, 0, TFT_BLACK );
    giveMuxSemaphore();
/*
    takeMuxSemaphore();
    MainSprite.pushSprite( 0, 0, TFT_BLACK ); // last = transparent color
    MainSprite.deleteSprite();
    giveMuxSemaphore();*/
    vTaskDelay( 20 );
  }
}

static uint8_t needrendering;// = 0;
static bool isrendering = false;

static void drawIcon( SpriteSheetIcon icon, uint16_t x, uint16_t y ) {
  while( isrendering ) {
    vTaskDelay( 10 );
  }
  isrendering = true;
  log_w("Icon #%d will be rendererd at [%d,%d] (%d in queue)", icon, x, y, needrendering );
  takeMuxSemaphore();
  tft.fillRect( x, y, 32, 32, TFT_BLACK );
  //bool swap = sprite.getSwapBytes();
  //sprite.setSwapBytes( !swap ); // dafuq
  sprite_drawSpriteSheet( sprite, icon, x, y );
  giveMuxSemaphore();
  needrendering--;
  isrendering = false;
  log_w("Icon #%d was rendererd !", icon );
}


static void drawIcon( void * param ) {
  if( param == nullptr ) {
    vTaskDelete( NULL );
    return;
  }
  //*param = reinterpret_cast<UI_Icon*>(param);
  UI_Icon *taskicon = reinterpret_cast<UI_Icon*>(param);
  //UI_Icon *taskicon = (UI_Icon*) param;
  drawIcon( taskicon->spriteSheetIcon, taskicon->x, taskicon->y );
  vTaskDelete( NULL );
}

static void drawIconTask( SpriteSheetIcon icon, uint16_t x, uint16_t y ) {
  UI_Icon *taskicon = new UI_Icon(x, y, icon);
  xTaskCreatePinnedToCore( drawIcon, "drawIcon", 16000, (void*)taskicon, 8, NULL, 0 );
}


static void setupTasks( void* params=NULL ) {
  mux = xSemaphoreCreateMutex();
  #ifdef USE_BUTTONS
    xTaskCreatePinnedToCore( buttonsTask, "buttonsTask", 4096, NULL, 1, NULL, 0 );
    vTaskDelay( 10 );
  #endif
  #ifdef SPEAKER_PIN
    xTaskCreatePinnedToCore( soundTask,   "soundTask",   2048, NULL, 4, NULL, 0 );
    vTaskDelay( 10 );
  #endif
  xTaskCreatePinnedToCore( timerTasks, "timerTasks", 8192, NULL, 4, NULL, 0 );
  vTaskDelay( 10 );
  xTaskCreatePinnedToCore( mainTask,   "mainTask",   2048, NULL, 8, NULL, 1 );
  vTaskDelay( 10 );
  vTaskDelete( NULL );
}


void initTasks() {
  xTaskCreate( setupTasks, "setupTasks", 16000, NULL, 2, NULL );
}


#endif // _TASKS_H_
