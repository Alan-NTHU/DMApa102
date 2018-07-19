/*

  This is a example of using DMA - SPI with apa102
  shows the diffence time blocked by FastLED and DmaSpi library.

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
unsigned long startMicros, frameTime, results;
byte buffer[DMASIZE];

DMAMEM byte address[DMASIZE];

#define TSPEED 16000000 // 16Mhz , can edit this for differen results
SPISettings dotstar(TSPEED , MSBFIRST, SPI_MODE0);
ActiveLowChipSelect cs(0, dotstar);
// we dont need the cs here, but using the SPI_settings
DmaSpi::Transfer trx(address, DMASIZE, nullptr, 0 , &cs);

#include <FastLED.h>
// from https://github.com/FastLED/FastLED
CRGB leds[NUM_LEDS];

void setup() {
  Serial.println(F("Hello, this is DMApa102"));
  SPI.begin();
  DMASPI0.begin();
  // put your setup code here, to run once:

  FastLED.addLeds<APA102, 11, 13, BGR, DATA_RATE_MHZ(16)>(leds, NUM_LEDS);

  fill_solid( &(leds[0]), NUM_LEDS , CRGB(10, 10, 10) );//take some tests here;
  FastLED.show();
  delay(1000);
  fill_solid( &(leds[0]), NUM_LEDS , CRGB(0, 0, 0) );//take some tests here;
  FastLED.show();
  delay(1000);


}


void loop() {
  // put your main code here, to run repeatedly:

  fill_solid( &(leds[0]), NUM_LEDS , CRGB(50, 50, 50) );
  startMicros = nowMicros;
  FastLED.show();
  results = nowMicros - startMicros;
  Serial.print(F("a SPI_frame with 80 LEDs cost "));
  Serial.print(results);
  Serial.println(F(" us "));
  //time used
  delay(500);
  fill_solid( &(leds[0]), NUM_LEDS , CRGB(0, 0, 0) );
  FastLED.show();

  delay(500);
  fill_solid( &(leds[0]), NUM_LEDS , CRGB(50, 50, 50) );

  SPI.begin();
  DMASPI0.start();

  startMicros = nowMicros;
  DMAshow(); // launch the DMASPI.
  results = nowMicros - startMicros;


  Serial.print(F("a DMA_frame with 80 LEDs cost "));
  Serial.print(results);
  Serial.println(F(" us "));
  //time used
  delay(500);
  fill_solid( &(leds[0]), NUM_LEDS , CRGB(0, 0, 0) );
  DMAshow();
  delay(500);

  while (true) {} // hang program.
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
  //nope, not in this example

  // wait, incase the last DMA launch isn't complete yet.
  while (trx.busy()) {}

  memcpy(address, buffer, DMASIZE);
  DMASPI0.registerTransfer(trx);
}
