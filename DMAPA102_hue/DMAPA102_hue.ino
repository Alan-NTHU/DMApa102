/*

  This is a example of using DMA - SPI with apa102
  doing hue_drawing, uses FastLED and DmaSpi library.

  using pin 11 & 13 as SPIport - Dout / Clock for apa102

  this works on Teensy 3.6, tested
  and should work on Teensy 3.5 / LC, testes with different DMAPA102 code

  Please give some likes if you like the works thank you.
  National Tsing Hua University_Glow Performance Club (NTHU_GPC)
  https://www.facebook.com/nthugpc/
  Panda'Spectrum
  https://www.facebook.com/PandaSpectrum/
  2018/JUL/19

*/

#define NUM_LEDS 80



#include <DmaSpi.h>
// from https://github.com/crteensy/DmaSpi
#define DMASIZE NUM_LEDS*4 + 4 +(NUM_LEDS/4) // (add start&end frame)
// 4 bytes per pixel, and extra 4 start bytes, and some end pushing bytes.
// start bytes are 4 (0x00)s, while endbyte are all (0xFF)s.
elapsedMicros nowMicros;
elapsedMillis nowMillis;
unsigned long targetMicros, frameTime;
byte buffer[DMASIZE];

DMAMEM byte address[DMASIZE];
#define TSPEED 16000000 // 16Mhz
SPISettings dotstar(TSPEED , MSBFIRST, SPI_MODE0);
ActiveLowChipSelect cs(0, dotstar);
// we dont need the cs here, but using the SPI_settings
DmaSpi::Transfer trx(address, DMASIZE, nullptr, 0 , &cs);
bool FPSchecker = 0;

unsigned int fps = 60; // modify this, if you wish to try another FPS
// or set to 0, to run on full speed
float frame = 0;

#include <FastLED.h>
// from https://github.com/FastLED/FastLED
byte hue = 1;
CRGB leds[NUM_LEDS];

void setup() {
  Serial.println(F("Hello, this is DMApa102"));
  SPI.begin();
  DMASPI0.begin();
  DMASPI0.start();
  // put your setup code here, to run once:
  FPSchecker = (fps > 0);
  if (FPSchecker)  {
    frameTime = (1000000 / fps); //microseconds to wait for next frame.
  }

  fill_solid( &(leds[0]), NUM_LEDS , CRGB(10, 10, 10) );//take some tests here;
  DMAshow();
  delay(1000);
  fill_solid( &(leds[0]), NUM_LEDS , CRGB(0, 0, 0) );//take some tests here;
  DMAshow();
  delay(1000);

  targetMicros = nowMicros; // follow up the time so it wont wait so long for FPS
}


void loop() {
  // put your main code here, to run repeatedly:

  for (byte n = 0; n <= NUM_LEDS; n++) {
    leds[n] = CHSV(hue, 255, 40);
  }
hue +=1;

  DMAshow(); // launch the DMASPI.
  

  if (nowMillis / 1000 >= 2) { // reports real FPS every 2 seconds
    Serial.println(F(" frames per second :"));
    Serial.print(frame);
    Serial.print(F(" frames in "));
    Serial.print(2000);
    Serial.print(F("ms\t FPS = "));
    Serial.println((frame / 2));
    Serial.println("");
    frame = 0;
    nowMillis = 0;
  }
}

inline void DMAshow() {

  memset(&buffer, 0xFF, DMASIZE); //this also sets the end frames, and globalbright of every pixel

  int dmaoffset;
  byte ploc;
  // start frames of apa102,
  buffer[0] = 0x00;
  buffer[1] = 0x00;
  buffer[2] = 0x00;
  buffer[3] = 0x00;

  for (int n = NUM_LEDS ; n >= 1 ; n--) // leave the 0-3 unchanged 0x00;
  {
    dmaoffset = n * 4;
    ploc = n - 1;
    //buffer[dmaoffset] = .... ;
    /*this is already 0xFF as memset above, or you
      can use buffer[dmaoffset] = 111abcde; with abcde = 1-31 to decide this pixel brightness
      not this uses a slower PWM, and shows unaceptable flickering if using with Persistence of Vision Projects.
    */
    buffer[dmaoffset + 1] =  (leds[ploc].b); // do some corrections here if you wish
    buffer[dmaoffset + 2] =  (leds[ploc].g); // do some corrections here if you wish
    buffer[dmaoffset + 3] =  (leds[ploc].r); // do some corrections here if you wish
  }

  //this controlls the FPS, if set.
  if (FPSchecker) {
    while (nowMicros < targetMicros) {
      //
      delayMicroseconds(1);
    }
    targetMicros = nowMicros  + frameTime;
  }

  // wait, incase the last DMA launch isn't complete yet.
  while (trx.busy()) {}

  memcpy(address, buffer, DMASIZE);
  DMASPI0.registerTransfer(trx);
  frame++; //count a frame line.
}
