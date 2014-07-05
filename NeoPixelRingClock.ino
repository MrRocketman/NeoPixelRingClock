#include <Adafruit_NeoPixel.h>

#define NUMBER_OF_PIXELS_IN_RING 16
#define BRIGHTNESS 32 // From 0...255 // Make sure your power supply can handle this
#define TOP_LED 1 // A positive number from 0...(NUMBER_OF_PIXELS_IN_RING - 1)
#define DIRECTION -1 // 1 or -1
#define NUMBER_OF_LEDS_PER_CLOCK_HAND 2

#define NEOPIXEL_PIN 6
// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMBER_OF_PIXELS_IN_RING, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

#define PIXELS_PER_HOUR (NUMBER_OF_PIXELS_IN_RING / 12.0)
#define PIXELS_PER_MINUTE (NUMBER_OF_PIXELS_IN_RING / 60.0)
#define PIXELS_PER_SECOND (NUMBER_OF_PIXELS_IN_RING / 60.0)

int hourPixelBrightnesses[NUMBER_OF_LEDS_PER_CLOCK_HAND];
int hourPixelIndexes[NUMBER_OF_LEDS_PER_CLOCK_HAND];
int minutePixelBrightnesses[NUMBER_OF_LEDS_PER_CLOCK_HAND];
int minutePixelIndexes[NUMBER_OF_LEDS_PER_CLOCK_HAND];
int secondPixelBrightnesses[NUMBER_OF_LEDS_PER_CLOCK_HAND];
int secondPixelIndexes[NUMBER_OF_LEDS_PER_CLOCK_HAND];

int startHour = 1;
int startMinute = 24;
int startSecond = 15;

int directionOffset = 0;
int topLEDOffset = 0;

void determineHourPixels();
void determineMinutePixels();
void determineSecondPixels();

void setup() 
{
  Serial.begin(9600);
  
  if(DIRECTION == -1)
  {
    directionOffset = NUMBER_OF_PIXELS_IN_RING;
  }
  topLEDOffset = TOP_LED * DIRECTION;
  
  resetPixels();
  
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() 
{
    showTime();
}

void showTime()
{
  determineHourPixels();
  determineMinutePixels();
  determineSecondPixels();
  
  uint8_t red, green, blue;
  // Set all empty pixels to empty
  for(int i = 0; i < NUMBER_OF_PIXELS_IN_RING; i ++)
  {
    // Default to no color
    red = 0;
    green = 0;
    blue = 0;
    
    for(int i2 = 0; i2 < NUMBER_OF_LEDS_PER_CLOCK_HAND; i2 ++)
    {
      // Set the red if there is an hour pixel with the current index
      if(hourPixelIndexes[i2] == i)
      {
        red = hourPixelBrightnesses[i2];
      }
      // Set the green if there is an minute pixel with the current index
      if(minutePixelIndexes[i2] == i)
      {
        green = minutePixelBrightnesses[i2];
      }
      // Set the blue if there is an second pixel with the current index
      if(secondPixelIndexes[i2] == i)
      {
        blue = secondPixelBrightnesses[i2];
      }
    }
    
    strip.setPixelColor(i, strip.Color(red, green, blue));
  }
  
  strip.show();
  
  resetPixels();
}

void resetPixels()
{
  memset(hourPixelIndexes, -1, 2);
  memset(hourPixelBrightnesses, -1, 2);
  memset(minutePixelIndexes, -1, 2);
  memset(minutePixelBrightnesses, -1, 2);
  memset(secondPixelIndexes, -1, 2);
  memset(secondPixelBrightnesses, -1, 2);
}

void determineHourPixels()
{
  determineClockHandPixelsForTime(floatHour(), PIXELS_PER_HOUR, hourPixelBrightnesses, hourPixelIndexes); 
}

void determineMinutePixels()
{
  determineClockHandPixelsForTime(floatMinute(), PIXELS_PER_MINUTE, minutePixelBrightnesses, minutePixelIndexes); 
}

void determineSecondPixels()
{
  determineClockHandPixelsForTime(floatSecond(), PIXELS_PER_SECOND, secondPixelBrightnesses, secondPixelIndexes); 
}

void determineClockHandPixelsForTime(float time, float pixelsPerUnit, int *brightnessArray, int *pixelIndexesArray)
{ 
  //NUMBER_OF_LEDS_PER_CLOCK_HAND
  
  time = time * pixelsPerUnit;
  brightnessArray[0] = (time - (int)time) * BRIGHTNESS;
  pixelIndexesArray[0] = directionOffset - (topLEDOffset + ((int)time + 1)) * -DIRECTION;
  if(pixelIndexesArray[0] > NUMBER_OF_PIXELS_IN_RING - 1)
  {
    pixelIndexesArray[0] %= NUMBER_OF_PIXELS_IN_RING;
  }
  
  brightnessArray[1] = ((int)time + 1 - time) * BRIGHTNESS;
  pixelIndexesArray[1] = directionOffset - (topLEDOffset + ((int)time))  * -DIRECTION;
  if(pixelIndexesArray[1] > NUMBER_OF_PIXELS_IN_RING - 1)
  {
    pixelIndexesArray[1] %= NUMBER_OF_PIXELS_IN_RING;
  }
}

float floatSecond()
{
  float time = fmod((millis() / 1000.0) + startSecond, 60);
  
  return time;
}

int second()
{
  int time = ((millis() / 1000) + startSecond) % 60;
  
  return time;
}

float floatMinute()
{
  float time = fmod((millis() / 1000.0) / 60 + startMinute, 60);
  
  return time;
}

int minute()
{
  int time = ((millis() / 1000) / 60 + startMinute) % 60;
  
  return time;
}

float floatHour()
{
  float time = fmod((millis() / 1000.0) / 60 / 12 + startHour, 12);
  
  return time;
}

int hour()
{
  int time = ((millis() / 1000) / 60 / 12 + startHour) % 12;  
  return time;
}




// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) 
{
  for(uint16_t i=0; i<strip.numPixels(); i++) 
  {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

// Fill the dots one after the other with a color
void inverseColorWipe(uint32_t c, uint8_t wait) 
{
  for(uint16_t i=strip.numPixels() - 1; i>0; i--) 
  {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void rainbow(uint8_t wait) 
{
  uint16_t i, j;

  for(j=0; j<256; j++) 
  {
    for(i=0; i<strip.numPixels(); i++) 
    {
      strip.setPixelColor(i, Wheel((i+j) & BRIGHTNESS));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) 
{
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++)
    {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & BRIGHTNESS));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to BRIGHTNESS to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, BRIGHTNESS - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(BRIGHTNESS - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, BRIGHTNESS - WheelPos * 3);
  }
}

