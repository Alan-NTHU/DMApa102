# DMApa102
Will need a Teensy and following library:

DMASPI: https://github.com/crteensy/DmaSpi

FastLED: https://github.com/FastLED/FastLED

Using DMASPI with FastLED to push APA102 LEDs.
Possible to combine with FastLED existed examples, but may need some works.

the DMAPA102_hue example shows how the function of DMAshow and using elaspedMicros to make the framing stable.
if frame per second is more than 60, it is probably not very pleasant to human eyes while static.

the DMAPA102_microsearned example shows the time blocked in microseconds, you can change the SPIsettings clock to see differences.
notice that with DMASPI you will still need to wait the SPI transfer to complete to show next frame, but you can do other different things while transfering.

  Credits.   
  National Tsing Hua University_Glow Performance Club (NTHU_GPC): https://www.facebook.com/nthugpc/
  Panda'Spectrum:   https://www.facebook.com/PandaSpectrum/
  2018/JUL/19
