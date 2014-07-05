#include <Adafruit_NeoPixel.h>

// TODO: Blink

#define NUMBER_OF_PIXELS_IN_RING 16
#define BRIGHTNESS 64 // From 0...255 *************** Make sure your power supply can handle this * NUMBER_OF_LEDS_PER_CLOCK_HAND ******************
#define TOP_LED 1 // A positive number from 0...(NUMBER_OF_PIXELS_IN_RING - 1)
#define DIRECTION -1 // 1 or -1 // Which direction should the watch move?

#define NUMBER_OF_LEDS_FOR_HOUR_HAND 1
#define NUMBER_OF_LEDS_FOR_MINUTE_HAND 1
#define NUMBER_OF_LEDS_FOR_SECOND_HAND 3
#define NUMBER_OF_LEDS_FOR_MILLISECOND_HAND 3
#define LARGEST_NUMBER_OF_LEDS_FOR_A_HAND 5 // ********* Make sure to set this ************

#define HOUR_HAND_RED 1.0
#define HOUR_HAND_GREEN 0.0
#define HOUR_HAND_BLUE 0.0
#define MINUTE_HAND_RED 0.0
#define MINUTE_HAND_GREEN 1.0
#define MINUTE_HAND_BLUE 0.0
#define SECOND_HAND_RED 0.0
#define SECOND_HAND_GREEN 0.0
#define SECOND_HAND_BLUE 1.0
#define MILLISECOND_HAND_RED 0.5
#define MILLISECOND_HAND_GREEN 0.0
#define MILLISECOND_HAND_BLUE 0.5

// If using type 2, the NUMBER_OF_LEDS_FOR_xxxx_HAND above should be odd numbers
#define HAND_DISPLAY_TYPE 0 // 0 = trailing leds, 1 = leading leds, 2 = leading and trailing leds

#define SHOW_HOUR_HAND 1
#define SHOW_MINUTE_HAND 1
#define SHOW_SECOND_HAND 1
#define SHOW_MILLISECOND_HAND 1

#define BLINK 0

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
#define NEOPIXEL_PIN 6
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMBER_OF_PIXELS_IN_RING, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

#define PIXELS_PER_HOUR (NUMBER_OF_PIXELS_IN_RING / 12.0)
#define PIXELS_PER_MINUTE (NUMBER_OF_PIXELS_IN_RING / 60.0)
#define PIXELS_PER_SECOND (NUMBER_OF_PIXELS_IN_RING / 60.0)
#define PIXELS_PER_MILLISECOND (NUMBER_OF_PIXELS_IN_RING / 1000.0)

int hourPixelBrightnesses[LARGEST_NUMBER_OF_LEDS_FOR_A_HAND];
int hourPixelIndexes[LARGEST_NUMBER_OF_LEDS_FOR_A_HAND];
int minutePixelBrightnesses[LARGEST_NUMBER_OF_LEDS_FOR_A_HAND];
int minutePixelIndexes[LARGEST_NUMBER_OF_LEDS_FOR_A_HAND];
int secondPixelBrightnesses[LARGEST_NUMBER_OF_LEDS_FOR_A_HAND];
int secondPixelIndexes[LARGEST_NUMBER_OF_LEDS_FOR_A_HAND];
int millisecondPixelBrightnesses[LARGEST_NUMBER_OF_LEDS_FOR_A_HAND];
int millisecondPixelIndexes[LARGEST_NUMBER_OF_LEDS_FOR_A_HAND];

int startHour = 4;
int startMinute = 30;
int startSecond = 0;

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
    if(SHOW_MILLISECOND_HAND)
    {
        determineMillisecondPixels();
    }
    if(SHOW_SECOND_HAND)
    {
        determineSecondPixels();
    }
    if(SHOW_MINUTE_HAND)
    {
        determineMinutePixels();
    }
    if(SHOW_HOUR_HAND)
    {
        determineHourPixels();
    }
    
    uint8_t red, green, blue;
    // Set all empty pixels to empty
    for(int i = 0; i < NUMBER_OF_PIXELS_IN_RING; i ++)
    {
        // Default to no color
        red = 0;
        green = 0;
        blue = 0;
        
        for(int i2 = 0; i2 < LARGEST_NUMBER_OF_LEDS_FOR_A_HAND; i2 ++)
        {
            // Set the red if there is an hour pixel with the current index
            if(hourPixelIndexes[i2] == i && SHOW_HOUR_HAND)
            {
                red += hourPixelBrightnesses[i2] * HOUR_HAND_RED;
                green += hourPixelBrightnesses[i2] * HOUR_HAND_GREEN;
                blue += hourPixelBrightnesses[i2] * HOUR_HAND_BLUE;
            }
            // Set the green if there is an minute pixel with the current index
            if(minutePixelIndexes[i2] == i && SHOW_MINUTE_HAND)
            {
                red += minutePixelBrightnesses[i2] * MINUTE_HAND_RED;
                green += minutePixelBrightnesses[i2] * MINUTE_HAND_GREEN;
                blue += minutePixelBrightnesses[i2] * MINUTE_HAND_BLUE;
            }
            // Set the blue if there is an second pixel with the current index
            if(secondPixelIndexes[i2] == i && SHOW_MILLISECOND_HAND)
            {
                red += secondPixelBrightnesses[i2] * SECOND_HAND_RED;
                green += secondPixelBrightnesses[i2] * SECOND_HAND_GREEN;
                blue += secondPixelBrightnesses[i2] * SECOND_HAND_BLUE;
            }
            // Set the blue if there is an millisecond pixel with the current index
            if(millisecondPixelIndexes[i2] == i && SHOW_MILLISECOND_HAND)
            {
                red += millisecondPixelBrightnesses[i2] * MILLISECOND_HAND_RED;
                green += millisecondPixelBrightnesses[i2] * MILLISECOND_HAND_GREEN;
                blue += millisecondPixelBrightnesses[i2] * MILLISECOND_HAND_BLUE;
            }
        }
        
        if(red > 255)
        {
            red = 255;
        }
        if(green > 255)
        {
            green = 255;
        }
        if(blue > 255)
        {
            blue = 255;
        }
        
        strip.setPixelColor(i, strip.Color(red, green, blue));
    }
    
    strip.show();
    
    resetPixels();
}

void resetPixels()
{
    memset(hourPixelIndexes, -1, LARGEST_NUMBER_OF_LEDS_FOR_A_HAND);
    memset(hourPixelBrightnesses, -1, LARGEST_NUMBER_OF_LEDS_FOR_A_HAND);
    memset(minutePixelIndexes, -1, LARGEST_NUMBER_OF_LEDS_FOR_A_HAND);
    memset(minutePixelBrightnesses, -1, LARGEST_NUMBER_OF_LEDS_FOR_A_HAND);
    memset(secondPixelIndexes, -1, LARGEST_NUMBER_OF_LEDS_FOR_A_HAND);
    memset(secondPixelBrightnesses, -1, LARGEST_NUMBER_OF_LEDS_FOR_A_HAND);
    memset(millisecondPixelIndexes, -1, LARGEST_NUMBER_OF_LEDS_FOR_A_HAND);
    memset(millisecondPixelBrightnesses, -1, LARGEST_NUMBER_OF_LEDS_FOR_A_HAND);
}

void determineHourPixels()
{
    determineClockHandPixelsForTime(floatHour(), PIXELS_PER_HOUR, hourPixelBrightnesses, hourPixelIndexes, NUMBER_OF_LEDS_FOR_HOUR_HAND);
}

void determineMinutePixels()
{
    determineClockHandPixelsForTime(floatMinute(), PIXELS_PER_MINUTE, minutePixelBrightnesses, minutePixelIndexes, NUMBER_OF_LEDS_FOR_MINUTE_HAND);
}

void determineSecondPixels()
{
    determineClockHandPixelsForTime(floatSecond(), PIXELS_PER_SECOND, secondPixelBrightnesses, secondPixelIndexes, NUMBER_OF_LEDS_FOR_SECOND_HAND);
}

void determineMillisecondPixels()
{
    determineClockHandPixelsForTime(floatMillisecond(), PIXELS_PER_MILLISECOND, millisecondPixelBrightnesses, millisecondPixelIndexes, NUMBER_OF_LEDS_FOR_MILLISECOND_HAND);
}

void determineClockHandPixelsForTime(float time, float pixelsPerUnit, int *brightnessArray, int *pixelIndexesArray, int numberOfLEDsToUse)
{
    //BLINK
    
    // Traling
    if(HAND_DISPLAY_TYPE == 0)
    {
        ledTrail(time, pixelsPerUnit, brightnessArray, pixelIndexesArray, numberOfLEDsToUse, 0, -1);
    }
    // Leading
    else if(HAND_DISPLAY_TYPE == 1)
    {
        ledTrail(time, pixelsPerUnit, brightnessArray, pixelIndexesArray, numberOfLEDsToUse, 0, 1);
    }
    // Leading And Trailing
    else if(HAND_DISPLAY_TYPE == 2)
    {
        int numberOfPixels = ((numberOfLEDsToUse % 2 == 1) ? ((numberOfLEDsToUse + 1) / 2) : (numberOfLEDsToUse / 2));
        ledTrail(time, pixelsPerUnit, brightnessArray, pixelIndexesArray, numberOfPixels, 0, -1);
        ledTrail(time, pixelsPerUnit, brightnessArray, pixelIndexesArray, numberOfPixels, numberOfPixels - 1, 1);
    }
}

// direction = -1 means trailing, direction = 1 means leading
void ledTrail(float time, float pixelsPerUnit, int *brightnessArray, int *pixelIndexesArray, int pixelsToUse, int arrayOffset, int direction)
{
    float pixelIndexFloat = time * pixelsPerUnit;
    int pixelIndex = (int)(time * pixelsPerUnit);
    
    float percentageBetweenPixels = 0;
    // Goes from 1.0 - 0.0
    if(direction == -1)
    {
        percentageBetweenPixels = (pixelIndex + 1 - pixelIndexFloat);
    }
    // Goes from 0.0 - 1.0
    else
    {
        percentageBetweenPixels = (pixelIndexFloat - pixelIndex);
    }
    
    // The first pixel
    brightnessArray[0] =  BRIGHTNESS;
    pixelIndexesArray[0] = rawIndexToRingIndex(directionOffset - (topLEDOffset + pixelIndex * -DIRECTION));
    
    // The middle pixels
    if(pixelsToUse > 2)
    {
        for(int i = 1; i < pixelsToUse - 1; i ++)
        {
            brightnessArray[arrayOffset + i] = BRIGHTNESS / (2 * i) + percentageBetweenPixels * BRIGHTNESS / (2 * i);
            pixelIndexesArray[arrayOffset + i] = rawIndexToRingIndex(directionOffset - (topLEDOffset + (pixelIndex + (i * direction))) * -DIRECTION);
        }
    }
    
    // The last pixel
    if(pixelsToUse > 2)
    {
        brightnessArray[arrayOffset + pixelsToUse - 1] = percentageBetweenPixels * BRIGHTNESS / (2 * (pixelsToUse - 2));
        pixelIndexesArray[arrayOffset + pixelsToUse - 1] = rawIndexToRingIndex(directionOffset - (topLEDOffset + (pixelIndex + (pixelsToUse - 1) * direction)) * -DIRECTION);
    }
    else if(pixelsToUse > 1)
    {
        brightnessArray[arrayOffset + pixelsToUse - 1] = percentageBetweenPixels * BRIGHTNESS / 2;
        pixelIndexesArray[arrayOffset + pixelsToUse - 1] = rawIndexToRingIndex(directionOffset - (topLEDOffset + (pixelIndex + (pixelsToUse - 1) * direction)) * -DIRECTION);
    }
}

int rawIndexToRingIndex(int rawIndex)
{
    if(rawIndex > NUMBER_OF_PIXELS_IN_RING - 1)
    {
        rawIndex %= NUMBER_OF_PIXELS_IN_RING;
    }
    else if(rawIndex < 0)
    {
        rawIndex += NUMBER_OF_PIXELS_IN_RING;
    }
    
    return rawIndex;
}

float floatMillisecond()
{
    float time = fmod(millis(), 1000);
    
    return time;
}

float floatSecond()
{
    float time = fmod(rawSecond(), 60);
    
    return time;
}

float floatMinute()
{
    float time = fmod(rawMinute(), 60);
    
    return time;
}

float floatHour()
{
    float time = fmod(rawHour(), 12);
    
    return time;
}

float rawMillisecond()
{
    return (float)millis();
}

float rawSecond()
{
    return rawMillisecond() / 1000.0 + startSecond;
}

float rawMinute()
{
    return rawSecond() / 60.0 + startMinute;
}

float rawHour()
{
    return rawMinute() / 60.0 + startHour;
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

