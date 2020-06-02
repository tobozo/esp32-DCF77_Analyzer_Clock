
#include <Time.h>


// button states (may be adjusted)
#define BUTTON_RIGHT 0
#define BUTTON_NEUTRAL 4
#define BUTTON_LEFT 3
#define BUTTON_CLICK 1

#define CNT_IDLE_ZERO 0x00
#define CNT_IDLE_MAX 0x01
#define CNT_DECREASING 0x02


TFT_eSprite cookTimerSprite = TFT_eSprite( &tft );


byte CookTimerButtonPos, CookTimerButtonLastPos;
bool changed = false;

uint8_t currentState = CNT_IDLE_MAX;
static byte longPushCounter = 0;

//static void InitUI();
static void displayRtcDate();

time_t maxTime = 600;
time_t currentTime = maxTime;
time_t start = now();
unsigned long millistart = millis();
unsigned long millioffset;


const int8_t POTINCREMENT = 5;
const int8_t POTDELAY = 50;
uint8_t potincrement = POTINCREMENT;
uint8_t potdelay = POTDELAY;

/*
void buttonScan() {
  int val2 = analogRead( SWOUT );
  CookTimerButtonPos = map(val2, 0, 1024, 0, 5); // adjust the last value this to fit your resistors
  if(CookTimerButtonPos!=CookTimerButtonLastPos) {
    changed = true;
    CookTimerButtonLastPos = CookTimerButtonPos;
  } else {
    changed = false;
  }
}*/


static char hStr[3] = "00";
static char mStr[3] = "00";
static char sStr[3] = "00";
static char timeToStr[12] = "           ";

//static char ddStr[3] = "00";
//static char mmStr[3] = "00";
//static char yyyyStr[5] = "0000";


// t is time in seconds = millis()/1000;
char * TimeToString(unsigned long t) {
  memset( timeToStr, 0, 12);
  long h = t / 3600;
  t = t % 3600;
  int m = t / 60;
  int s = t % 60;

  sprintf(hStr, "%02d", (int)h);
  sprintf(mStr, "%02d", (int)m);
  sprintf(sStr, "%02d", (int)s);

  //sprintf(ddStr, "%01d", d);
  //sprintf(mmStr, "%02d", m);
  //sprintf(yyyyStr, "%02d", y);

/*
  Serial.print(mStr);
  Serial.print(" ");
  Serial.println(sStr);
*/

  sprintf(timeToStr, "%02d %02d", m, s);

  return timeToStr;
}


struct Box {
  uint16_t x = 0;
  uint16_t y = 0;
  uint16_t height = 0;
  uint16_t width = 0;
};

static Box timerBox;
//static bool hourglassRendered = false;


void clearLastTimer() {
  sprite.fillRect( TFT_HALFWIDTH-30, RTCDateYPos-RTCDateFontHeight, 60, RTCDateFontHeight*2, TFT_BLACK );
  /*

  if( timerBox.height > 0 && timerBox.width > 0 ) {
    sprite.fillRect( timerBox.x, timerBox.y, timerBox.width, timerBox.height, TFT_BLACK );
  }
  */
}


void drawTimer( uint16_t posx, uint16_t posy ) {

  TimeToString(currentTime);

  uint16_t sectoggler = ((millis() - millistart)/10) % 100;
  char separatorChar[2] = ":";
  char outTime[16];

  if(currentState==CNT_DECREASING) {
    if(sectoggler<=50) {
      separatorChar[0] = ':';
    } else {
      separatorChar[0] = ' ';
    }
  }

  if(String(hStr)!="00") {
    sprintf( outTime, " %2s%s%2s%s%2s ", hStr, separatorChar, mStr, separatorChar, sStr );
  } else {
    sprintf( outTime, "   %2s%s%2s   ", mStr, separatorChar, sStr );
  }

  takeMuxSemaphore();
  setFontStyle( &sprite, myTimerFontStyle );
  uint16_t fontHeight = sprite.fontHeight();
  uint16_t textWidth = tft.textWidth( outTime );
  clearLastTimer();
  sprite.drawString( outTime, posx, posy );
  giveMuxSemaphore();

  timerBox.width  = textWidth  > timerBox.width  ? textWidth  : timerBox.width;
  timerBox.height = fontHeight > timerBox.height ? fontHeight : timerBox.height;
  timerBox.x      = posx - timerBox.width/2;
  timerBox.y      = posy - timerBox.height/2;

}


void timerSound() {
  for(uint16_t i=100;i<5000;i+=2) {
    M5.Speaker.tone(5000-i, 50);
  }
}

void cookTimerStop() {
  currentTime = 0;
  currentState = CNT_IDLE_ZERO;
  //sprite.fillSprite( TFT_BLACKISH );
  timerSound();
  clearLastTimer();
  //LogoSprite.fillRect( ( TFT_HALFWIDTH - 32 / 2 ), ( TFT_HALFHEIGHT - 32 / 2 ), 32, 32, TFT_BLACK );
  //LogoSprite.drawJpg( atomic_32x32_jpeg, atomic_32x32_jpeg_len );
  takeMuxSemaphore();
  sprite_drawSpriteSheet( LogoSprite, atomic/*, ( TFT_HALFWIDTH - 32 / 2 ), ( TFT_HALFHEIGHT - 32 / 2 )*/ );
  sprite.fillRect( ( TFT_HALFWIDTH - 32 / 2 ), ( TFT_HALFHEIGHT - 32 / 2 ), 32, 32, TFT_BLACK );
  sprite.pushImage( ( TFT_HALFWIDTH - 32 / 2 ), ( TFT_HALFHEIGHT - 32 / 2 ), 32, 32, (const uint16_t*)LogoSprite.frameBuffer(1) );
  sprite.pushSprite( 0, 0/*, TFT_BLACK */);
  giveMuxSemaphore();
  displayRtcDate();
}


void cookTimerSetup() {
  //sprite.fillSprite( TFT_BLACKISH );
  timerSound();
  clearLastTimer();
  //LogoSprite.fillRect( ( TFT_HALFWIDTH - 32 / 2 ), ( TFT_HALFHEIGHT - 32 / 2 ), 32, 32, TFT_BLACK );
  //LogoSprite.drawJpg( hourglass_jpeg, hourglass_jpeg_len );
  takeMuxSemaphore();
  sprite_drawSpriteSheet( LogoSprite, hourglass/*, ( TFT_HALFWIDTH - 32 / 2 ), ( TFT_HALFHEIGHT - 32 / 2 )*/ );
  sprite.fillRect( ( TFT_HALFWIDTH - 32 / 2 ), ( TFT_HALFHEIGHT - 32 / 2 ), 32, 32, TFT_BLACK );
  sprite.pushImage( ( TFT_HALFWIDTH - 32 / 2 ), ( TFT_HALFHEIGHT - 32 / 2 ), 32, 32, (const uint16_t*)LogoSprite.frameBuffer(1) );
  sprite.pushSprite( 0, 0/*, TFT_BLACK */);
  giveMuxSemaphore();

  millistart = millis();
  millioffset = 0;//millistart - (round(millistart / 1000) * 1000);
  Serial.println("Millistart " + String(millistart));
  Serial.println("Millioffset " + String(millioffset));
}



void pokeCounterAccel() {
  currentTime = maxTime;
  potdelay--;
  if(potdelay<=1) {
    potdelay = 1;
    potincrement++;
  }
  if(potincrement>60 || String(hStr)!="00") {
    potincrement = 60;
  }
  delay(potdelay);
}


void cookTimerloop() {

  switch(CookTimerButtonPos) {
    case BUTTON_RIGHT:
      if(maxTime+potincrement<=36000) {
        maxTime +=potincrement;
        pokeCounterAccel();
      }
      millistart = millis();
    break;
    case BUTTON_LEFT:
      if(maxTime>potincrement && maxTime-potincrement>0) {
        maxTime -=potincrement;
        pokeCounterAccel();
      }
      millistart = millis();
    break;
    case BUTTON_CLICK:
      millistart = millis();

      switch(currentState) {
        case CNT_IDLE_MAX:
          currentState = CNT_DECREASING;
          vTaskDelay(100);
          start = now();
        break;

        case CNT_DECREASING:
         //TODO: PAUSE/RESET
        //break;
        case CNT_IDLE_ZERO:
          //sprite.clearBuffer();
          //sprite.sendBuffer();
          vTaskDelay(100);
          currentTime = maxTime;
          currentState = CNT_IDLE_MAX;
        break;
      }

    break;
    case BUTTON_NEUTRAL:
      // Serial.println(pos); // uncomment this when calibrating
      if(currentState==CNT_DECREASING) {

          time_t counterOffset = now() - start;
          time_t tmpTime = maxTime - counterOffset;

          if(currentTime!=tmpTime) {
            millistart = millis();
            //millioffset = millistart - (round(millistart / 1000) * 1000);
            currentTime = tmpTime;
          }

          Serial.print(maxTime);
          Serial.print("\t");
          Serial.print(counterOffset);
          Serial.print("\t");
          Serial.println(currentTime);

          if(counterOffset>=maxTime) {
            // ring, ring
            currentTime = 0;
            //showTimer();
            currentState = CNT_IDLE_ZERO;
            drawTimer( sprite.width()/2, RTCDateYPos );

            do {
              for(uint16_t i=100;i<5000;i+=4) {
                M5.Speaker.tone(i, 30);
                vTaskDelay(1);
              }
              delay(500);
              //showClock();
            } while(CookTimerButtonPos == BUTTON_NEUTRAL);

            currentState = CNT_IDLE_MAX;
            currentTime = maxTime;

            delay(1000);
          }
      }

      // reset acceleration
      potincrement = POTINCREMENT;
      potdelay = POTDELAY;
    break;
  }

  if(currentState==CNT_DECREASING || millis()-millistart<30000) {
    drawTimer( sprite.width()/2, RTCDateYPos );
  }

}


/*
void showClock() {
  sprite.clearBuffer();
  sprite.setFontMode(1);

  sprintf(hStr, "%02d", hour());
  sprintf(mStr, "%02d", minute());
  sprintf(sStr, "%02d", second());

  sprite.setFont(u8g2_font_inb30_mn);
  sprite.drawString(0,30, hStr );
  sprite.drawString(54,30, mStr );

  sprite.setFont(u8g2_font_t0_22b_mf);
  sprite.drawString(48,15, ":" );
  //sprite.drawString(105,10, "m" );
  //sprite.drawString(112,30, "s" );
  sprite.drawString(104,30, sStr );

  sprite.setFont(u8g2_font_t0_22b_mf);
  //sprite.drawString(46,28, ":" );

  sprite.sendBuffer();
}*/
