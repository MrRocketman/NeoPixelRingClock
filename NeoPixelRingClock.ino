#include <Adafruit_NeoPixel.h>

#define PIN 6

#define NUMBER_OF_PIXELS_IN_RING 16
#define BRIGHTNESS 127
#define TOP_LED 1 // A positive number from 0...(NUMBER_OF_PIXELS_IN_RING - 1)
#define DIRECTION -1 // 1 or -1

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMBER_OF_PIXELS_IN_RING, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

#define PIXELS_PER_HOUR (NUMBER_OF_PIXELS_IN_RING / 12.0)
#define PIXELS_PER_MINUTE (NUMBER_OF_PIXELS_IN_RING / 60.0)
#define PIXELS_PER_SECOND (NUMBER_OF_PIXELS_IN_RING / 60.0)

int hourPixelBrightnesses[2];
int hourPixelIndexes[2];
int minutePixelBrightnesses[2];
int minutePixelIndexes[2];
int secondPixelBrightnesses[2];
int secondPixelIndexes[2];
//uint32_t hourColor = strip.Color(BRIGHTNESS, 0, 0);

int startHour = 0;
int startMinute = 45;
int startSecond = 25;

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

void resetPixels()
{
  memset(hourPixelIndexes, -1, 2);
  memset(hourPixelBrightnesses, -1, 2);
  memset(minutePixelIndexes, -1, 2);
  memset(minutePixelBrightnesses, -1, 2);
  memset(secondPixelIndexes, -1, 2);
  memset(secondPixelBrightnesses, -1, 2);
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
    
    for(int i2 = 0; i2 < 2; i2 ++)
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

void determineHourPixels()
{
  float mainPixel = floatHour() * PIXELS_PER_HOUR;
  //Serial.print("hours: ");
  //Serial.print(floatHour());
  //Serial.print(" main: ");
  //Serial.println(mainPixel);
  
  hourPixelBrightnesses[0] = (mainPixel - (int)mainPixel) * BRIGHTNESS;
  hourPixelIndexes[0] = directionOffset - (topLEDOffset + ((int)mainPixel + 1)) * -DIRECTION;
  if(hourPixelIndexes[0] > NUMBER_OF_PIXELS_IN_RING - 1)
  {
    hourPixelIndexes[0] %= NUMBER_OF_PIXELS_IN_RING;
  }
  hourPixelBrightnesses[1] = ((int)mainPixel + 1 - mainPixel) * BRIGHTNESS;
  hourPixelIndexes[1] = directionOffset - (topLEDOffset + ((int)mainPixel))  * -DIRECTION;
  if(hourPixelIndexes[1] > NUMBER_OF_PIXELS_IN_RING - 1)
  {
    hourPixelIndexes[1] %= NUMBER_OF_PIXELS_IN_RING;
  }
}

void determineMinutePixels()
{
  float mainPixel = floatMinute() * PIXELS_PER_MINUTE;
  //Serial.print("minute: ");
  //Serial.print(floatMinute());
  //Serial.print(" main: ");
  //Serial.println(mainPixel);
  
  minutePixelBrightnesses[0] = (mainPixel - (int)mainPixel) * BRIGHTNESS;
  minutePixelIndexes[0] = directionOffset - (topLEDOffset + ((int)mainPixel + 1))  * -DIRECTION;
  if(minutePixelIndexes[0] > NUMBER_OF_PIXELS_IN_RING - 1)
  {
    minutePixelIndexes[0] %= NUMBER_OF_PIXELS_IN_RING;
  }
  minutePixelBrightnesses[1] = ((int)mainPixel + 1 - mainPixel) * BRIGHTNESS;
  minutePixelIndexes[1] = directionOffset - (topLEDOffset + ((int)mainPixel))  * -DIRECTION;
  if(minutePixelIndexes[1] > NUMBER_OF_PIXELS_IN_RING - 1)
  {
    minutePixelIndexes[1] %= NUMBER_OF_PIXELS_IN_RING;
  }
}

void determineSecondPixels()
{
  float mainPixel = floatSecond() * PIXELS_PER_SECOND;
  //Serial.print("seconds: ");
  //Serial.print(floatSecond());
  //Serial.print(" main: ");
  //Serial.println(mainPixel);
  
  secondPixelBrightnesses[0] = (mainPixel - (int)mainPixel) * BRIGHTNESS;
  secondPixelIndexes[0] = directionOffset - (topLEDOffset + ((int)mainPixel + 1))  * -DIRECTION;
  if(secondPixelIndexes[0] > NUMBER_OF_PIXELS_IN_RING - 1)
  {
    secondPixelIndexes[0] %= NUMBER_OF_PIXELS_IN_RING;
  }
  secondPixelBrightnesses[1] = ((int)mainPixel + 1 - mainPixel) * BRIGHTNESS;
  secondPixelIndexes[1] = directionOffset - (topLEDOffset + ((int)mainPixel))  * -DIRECTION;
  if(secondPixelIndexes[1] > NUMBER_OF_PIXELS_IN_RING - 1)
  {
    secondPixelIndexes[1] %= NUMBER_OF_PIXELS_IN_RING;
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

