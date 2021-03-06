// extended from time_loop from https://github.com/zeroeth/time_loop 
#include "application.h"

// This #include statement was automatically added by the Spark IDE.
#include "neopixel/neopixel.h"

#define PIXEL_COUNT 12
#define PIXEL_PIN A7
#define PIXEL_TYPE WS2812B

// From adaFruit NEOPIXEL goggles example: Gamma correction improves appearance of midrange colors
const uint8_t gamma[] = {
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,
      1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,
      3,  3,  4,  4,  4,  4,  5,  5,  5,  5,  5,  6,  6,  6,  6,  7,
      7,  7,  8,  8,  8,  9,  9,  9, 10, 10, 10, 11, 11, 11, 12, 12,
     13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20,
     20, 21, 21, 22, 22, 23, 24, 24, 25, 25, 26, 27, 27, 28, 29, 29,
     30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 38, 38, 39, 40, 41, 42,
     42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
     58, 59, 60, 61, 62, 63, 64, 65, 66, 68, 69, 70, 71, 72, 73, 75,
     76, 77, 78, 80, 81, 82, 84, 85, 86, 88, 89, 90, 92, 93, 94, 96,
     97, 99,100,102,103,105,106,108,109,111,112,114,115,117,119,120,
    122,124,125,127,129,130,132,134,136,137,139,141,143,145,146,148,
    150,152,154,156,158,160,162,164,166,168,170,172,174,176,178,180,
    182,184,186,188,191,193,195,197,199,202,204,206,209,211,213,215,
    218,220,223,225,227,230,232,235,237,240,242,245,247,250,252,255
};

//  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12
const uint8_t LEDmap[] = {
    8, 9, 10, 11, 0, 1, 2, 3, 4, 5, 6, 7
};

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
//               note: if not specified, D2 is selected for you.
// Parameter 3 = pixel type [ WS2812, WS2812B, WS2811, TM1803 ]
//               note: if not specified, WS2812B is selected for you.
//               note: RGB order is automatically applied to WS2811,
//                     WS2812/WS2812B/TM1803 is GRB order.
//
// 800 KHz bitstream 800 KHz bitstream (most NeoPixel products ...
//                         ... WS2812 (6-pin part)/WS2812B (4-pin part) )
//
// 400 KHz bitstream (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//                   (Radio Shack Tri-Color LED Strip - TM1803 driver
//                    NOTE: RS Tri-Color LED's are grouped in sets of 3)

//Adafruit_NeoPixel strip = Adafruit_NeoPixel(12, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

uint32_t milli_color       = strip.Color ( 1,  1,  1);
uint32_t second_color      = strip.Color ( 0,  0,  20);
uint32_t second_color_weak = strip.Color ( 0,  0,  8);
uint32_t minute_color      = strip.Color ( 20, 0, 0);
uint32_t hour_color        = strip.Color (  0, 20,  0);

uint32_t off_color    = strip.Color (  1,  1,  1);
uint32_t off_color_strong    = strip.Color (  0,  1,  0);


/* CLOCK */
class ClockPositions
{
 public:
  uint8_t milli;
  uint8_t second;
  uint8_t minute;
  uint8_t hour;

  ClockPositions ();
  void update    ();
};


ClockPositions::ClockPositions()
{
  milli = second = minute = hour = 0;
  //milli = 0;
  //second = 0; //Time.second();
  //minute = 5; //Time.minute();
  //hour = 2; Time.hour();
  //DateTime(__DATE__, __TIME__);
}


void ClockPositions::update()
{
  milli  = map ((millis() %  1000), 0,  1000, 0, PIXEL_COUNT);
  second = map (Time.second(), 0, 60, 0, PIXEL_COUNT);
  minute = map (Time.minute(), 0, 60, 0, PIXEL_COUNT);
  hour   = map (Time.hour(), 0,  12, 0, PIXEL_COUNT);
}



/* CLOCK VIEW */

class ClockSegments
{
 public:
  ClockPositions    &positions;
  Adafruit_NeoPixel &strip;

  ClockSegments (Adafruit_NeoPixel&, ClockPositions&);
  void draw  ();
  void clear ();
  void add_color (uint8_t position, uint32_t color);
  uint32_t blend (uint32_t color1, uint32_t color2);
};


ClockSegments::ClockSegments (Adafruit_NeoPixel& n_strip, ClockPositions& n_positions): strip (n_strip), positions (n_positions)
{
}


void ClockSegments::draw()
{
  clear();

  add_color (positions.minute   % PIXEL_COUNT,  minute_color);
  add_color (positions.hour     % PIXEL_COUNT,  hour_color  );
//  add_color ((positions.hour+1) % PIXEL_COUNT,  hour_color  );

  add_color (positions.second     % PIXEL_COUNT, second_color);
  add_color ((positions.second-1) % PIXEL_COUNT, second_color_weak);
//  add_color ((positions.second+2) % PIXEL_COUNT, second_color);

  add_color (positions.milli     % PIXEL_COUNT,  milli_color);
  add_color ((positions.milli+1) % PIXEL_COUNT,  milli_color);
  add_color ((positions.milli+2) % PIXEL_COUNT,  milli_color);

  strip.show ();
}


void ClockSegments::add_color (uint8_t position, uint32_t color)
{
  uint32_t blended_color = blend (strip.getPixelColor (LEDmap[position]), color);

  /* Gamma mapping */
  uint8_t r,b,g;

  r = (uint8_t)(blended_color >> 16),
  g = (uint8_t)(blended_color >>  8),
  b = (uint8_t)(blended_color >>  0);

  strip.setPixelColor (LEDmap[position], blended_color);
}


uint32_t ClockSegments::blend (uint32_t color1, uint32_t color2)
{
  uint8_t r1,g1,b1;
  uint8_t r2,g2,b2;
  uint8_t r3,g3,b3;

  r1 = (uint8_t)(color1 >> 16),
  g1 = (uint8_t)(color1 >>  8),
  b1 = (uint8_t)(color1 >>  0);

  r2 = (uint8_t)(color2 >> 16),
  g2 = (uint8_t)(color2 >>  8),
  b2 = (uint8_t)(color2 >>  0);


  return strip.Color (constrain (r1+r2, 0, 255), constrain (g1+g2, 0, 255), constrain (b1+b2, 0, 255));
}


void ClockSegments::clear ()
{
  for(uint16_t i=0; i<strip.numPixels (); i++) {
    if ((i == 0) || (i == 3) || (i == 6) || (i == 9)) { 
      strip.setPixelColor (LEDmap[i], off_color_strong); 
    } else {
    strip.setPixelColor (LEDmap[i], off_color);
    }
  }
}


/* APP */
ClockPositions positions;
ClockSegments  segments(strip, positions);

void setup ()
{
  Time.zone(+1); // We are in Norway
  strip.begin ();
  strip.show (); // Initialize all pixels to 'off'
  
  // Dim on board LED - it is way too bright
  RGB.control(true);
  RGB.color(20, 0, 0);
  }


void loop ()
{
  // Check if we have summer or winter (normal) DST time
  if (IsDst(Time.day(), Time.month(), Time.weekday())) {
    //We ard saving daylight - It is Summertime
    Time.zone(+2);
  } else {
    //We are NOT saving daylight - It is Wintertime and this is the normal
    Time.zone(+1);
  }
  positions.update ();
  segments.draw ();
}

bool IsDst(uint8_t day, uint8_t month, uint8_t dow)
{
  if (month < 3 || month > 10)  return false; 
  if (month > 3 && month < 10)  return true; 

  uint8_t previousSunday = day - dow;

  if (month == 3) return previousSunday >= 25;
  if (month == 10) return previousSunday < 25;

  return false; // this line never gonna happend
}

