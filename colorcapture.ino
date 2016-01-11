// Code adapted from adafruit's colorview example for their
// TCS34725 RGB Color sensor module

#include <Wire.h> // Two-wire (I2C protocol) library
#include "Adafruit_TCS34725.h" // RGB Color sensor library
#include <Adafruit_NeoPixel.h> // Adafruit RGB LED library

// The Flora maps the RGB LED to pin 8 
#define PIN 8

// Create a NeoPixel "strip" which, for this case is only 1 pixel long. 
// You can drive full strips, but will need to externally hook those up
// to one of the other available Flora pins
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, PIN, NEO_GRB + NEO_KHZ800);
 
// our RGB -> eye-recognized gamma color
byte gammatable[256];
// If the RGB color sensor isn't found at boot, run a cycling rainbow demo
int useSensor = 1;

// Create a reference to the RGB Color sensor. 
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_4X);

void setup() {
  // Set up the serial port to print color information too
  Serial.begin(9600);
  Serial.println("Flora color sensor demo");

  if (tcs.begin()) {
    Serial.println("Found sensor");
    useSensor = 1;
  } else {
    Serial.println("No TCS34725 found ... Going to RAINBOW MODE");
    useSensor = 0;
  }
  // Initialize the NeoPixel and start it up with show.
  strip.begin();
  strip.show();
   
  // thanks PhilB for this gamma table!
  // it helps convert RGB colors to what humans see
  for (int i=0; i<256; i++) {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;
      
    gammatable[i] = x;      
  }
}

void loop() {
  uint16_t clear, red, green, blue;

  if(useSensor == 1) {
    tcs.setInterrupt(false);  // turn on LED and trigger a reading
    
    delay(25);  // takes ~25ms to read 
    
    tcs.getRawData(&red, &green, &blue, &clear);
      
    Serial.print("C:\t"); Serial.print(clear);
    Serial.print("\tR:\t"); Serial.print(red);
    Serial.print("\tG:\t"); Serial.print(green);
    Serial.print("\tB:\t"); Serial.print(blue);
  
    // Figure out some basic hex code for visualization
    uint32_t sum = clear;
    float r, g, b;
    r = red; r /= sum;
    g = green; g /= sum;
    b = blue; b /= sum;
    r *= 255; g *= 255; b *= 255;
    Serial.print("\t");
    Serial.print((int)r, HEX); Serial.print((int)g, HEX); Serial.print((int)b, HEX);
    Serial.println();
  
    strip.setPixelColor(0, strip.Color(gammatable[(int)r], gammatable[(int)g], gammatable[(int)b]));
    strip.show(); 
  } else {
    rainbowCycle(20);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;
 
  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}
 
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}
