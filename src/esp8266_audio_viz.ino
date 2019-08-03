
#include <FastLED.h>
#include "arduinoFFT.h" // Standard Arduino FFT library
// https://github.com/kosme/arduinoFFT, in IDE, Sketch, Include Library, Manage Library, then search for FFT
arduinoFFT FFT = arduinoFFT();

FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

//#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
#define DATA_PIN D4
//#define CLK_PIN   4
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS 120
#define TEMPERATURE Tungsten100W
CRGB leds[NUM_LEDS];

#define BRIGHTNESS 150
#define FRAMES_PER_SECOND 120
#define LED_DITHER 0 // try 0 to disable flickering

#define SAMPLES 256              //Must be a power of 2
#define SAMPLING_FREQUENCY 10000 //Hz, must be 10000 or less due to ADC conversion time. Determines maximum frequency that can be analysed by the FFT.
#define amplitude 50
unsigned int sampling_period_us;
unsigned long microseconds;
byte peak[] = {0, 0, 0, 0, 0, 0, 0};
double vReal[SAMPLES];
double vImag[SAMPLES];
unsigned long newTime, oldTime;

/////////////////////////////////////////////////////////////////////////

uint8_t val = 0;
int amp[7];
int avg = 0;
uint8_t countB = 0;
uint8_t gHue = 10;

void setup()
{

  delay(3000); // 3 second delay for recovery

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setTemperature(TEMPERATURE);
  //FastLED.setDither(LED_DITHER);
  FastLED.show(); // needed to reset leds to zero
  for (int i = 0; i < 6; i++)
  {
    amp[i] = 0;
  }

  Serial.begin(9600);

  sampling_period_us = round(100000 * (1.0 / SAMPLING_FREQUENCY));
}

void loop()
{

  for (int i = 0; i < SAMPLES; i++)
  {
    newTime = micros() - oldTime;
    oldTime = newTime;
    vReal[i] = analogRead(A0); // A conversion takes about 1mS on an ESP8266
    vImag[i] = 0;
    while (micros() < (newTime + sampling_period_us))
    {
      /* do nothing to wait */
    }
  }
  FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
  for (int i = 2; i < (SAMPLES / 2); i++)
  { // Don't use sample 0 and only first SAMPLES/2 are usable. Each array eleement represents a frequency and its value the amplitude.
    if (vReal[i] > 200)
    { // Add a crude noise filter, 4 x amplitude or more
      if (i <= 5)
      {
        // Serial.println(vReal[2] / amplitude);
        //Serial.println((int)vReal[i]/amplitude);// 125Hz
        if ((int)vReal[2] / amplitude > amp[0])
          amp[0] = (int)vReal[2] / amplitude;
      }
      if (i > 5 && i <= 12)
      {
        //displayBand(1,(int)vReal[i]/amplitude); // 250Hz
        if ((int)vReal[i] / amplitude > amp[1])
          amp[1] = (int)vReal[i] / amplitude;
      }
      if (i > 12 && i <= 32)
      {
        //displayBand(2,(int)vReal[i]/amplitude); // 500Hz
        //Serial.println((int)vReal[i]/amplitude);
        if ((int)vReal[i] / amplitude > amp[2])
          amp[2] = (int)vReal[i] / amplitude;
      }
      if (i > 32 && i <= 62)
      {
        //displayBand(3,(int)vReal[i]/amplitude); // 1000Hz
        if ((int)vReal[i] / amplitude > amp[3])
          amp[3] = (int)vReal[i] / amplitude;
      }
      if (i > 62 && i <= 105)
      {
        //displayBand(4,(int)vReal[i]/amplitude); // 2000Hz
        if ((int)vReal[i] / amplitude > amp[4])
          amp[4] = (int)vReal[i] / amplitude;
      }
      if (i > 105 && i <= 120)
      {
        //displayBand(5,(int)vReal[i]/amplitude); // 4000Hz
        if ((int)vReal[i] / amplitude > amp[5])
          amp[5] = (int)vReal[i] / amplitude;
      }
      if (i > 120 && i <= 146)
      {
        //displayBand(6,(int)vReal[i]/amplitude); // 8000Hz
        if ((int)vReal[i] / amplitude > amp[6])
          amp[6] = (int)vReal[i] / amplitude;
      }
      //Serial.println(i);
    }
  }
  //Normalize amplitudes
  amp[6] *= 4;
  amp[5] *= 4;
  amp[4] *= 2.5;
  //amp[3]*=2;
  //amp[2]*=1.5;
  amp[1] *= 1;
  amp[0] *= 0.75;
  avg = (amp[2] + amp[3] + amp[4]) / 3;

  fadeToBlackBy(leds, NUM_LEDS, 25);

  if (amp[0] > 200)
  {
    amp[0] = 200;
  }

  CRGB clr = CRGB::Violet;
  CRGB clr2 = CRGB::Violet;

  clr2 = clr2.nscale8(amp[0] * 1.2);

  if (amp[4] > 30 || amp[3] > 30)
    clr = clr.nscale8(192);
  avg *= 0.65;
  if (avg > 40)
  {
    avg = 40;
  }

  if (avg > 1 && amp[4] < 60)
    fill_solid(leds + (60 - avg), avg * 2, clr);

  clr = CRGB::White;
  clr.maximizeBrightness();
  if (amp[4] > 30)
    for (int i = 0; i < 10; i++)
      addGlitter(clr);
  clr = CRGB::Black;
  clr.maximizeBrightness();
  if (amp[3] > 30)
    for (int i = 0; i < 10; i++)
      addGlitter(clr);

  if (amp[0] > 50 && amp[4] < 40)
  {
    if (countB == 0)
    {
      fill_solid(leds, 40, clr2);
      fill_solid(leds + 80, 40, clr2);
    }
    else
      countB++;
    if (countB == 2)
      countB = 0;
  }

  FastLED.show();
  for (int i = 0; i < 6; i++)
  {
    amp[i] = 0;
  }
}

void confetti()
{
  // random colored speckles that blink in and fade smoothly
  //fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV(gHue + random8(64), 200, 255);
}

void addGlitter(CRGB clr)
{
  leds[random16(NUM_LEDS)] = clr;
}
