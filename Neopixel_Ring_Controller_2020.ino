#include <Wire.h>

// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// Released under the GPLv3 license to match the rest of the
// Adafruit NeoPixel library

#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        1 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 16 // Popular NeoPixel ring size

#define DELAYVAL 50 // Time (in milliseconds) to pause between pixels

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
boolean showPattern  = true;
boolean patternDir = true;
int     patternSpeed = 25;
int     patternColor = 4;
int     patternPos = 0;

void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(255);

  pixels.clear(); // Set all pixel colors to 'off'
  for(int i=0; i<NUMPIXELS; i++) { // For each pixel...

    // Here we're using a moderately color that compensates for the slightly 
    // blueish tint if we set them to all the same color:
    pixels.setPixelColor(i, 70, 60, 50);

  }
  pixels.show();   // Send the updated pixel colors to the hardware.

  Wire.begin(44);               // join i2c bus with address #44
  Wire.onReceive(receiveEvent); // register event

}

void loop() {

  if (showPattern) {
    int p = patternPos % NUMPIXELS;
    int r = ((patternColor & 1) == 1) ? 128 : 0;
    int g = ((patternColor & 2) == 2) ? 128 : 0;
    int b = ((patternColor & 4) == 4) ? 128 : 0;
    pixels.clear(); // Set all pixel colors to 'off'
    for(int i=0; i<NUMPIXELS; i++) { // For each pixel...
  
      if (i == p) {
        pixels.setPixelColor(i, r, g, b);
      } else {
        pixels.setPixelColor(i, 0, 0, 0);
      }
    }
    pixels.show();   // Send the updated pixel colors to the hardware.
    patternPos++;
    delay(patternSpeed); 
  } else {
    delay(10); 
  }
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
  int buffer[howMany];
  int idx = 0;
  while( Wire.available() > 0 ) // loop through all but the last
  {
    char c = Wire.read(); // receive byte as a character
    buffer[idx] = c;
    idx++;
  }

  if (howMany > 0) {
    pixels.clear(); // Set all pixel colors to 'off'

    if (howMany == 1) {
      showPattern = false;
      for (int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(buffer[0], buffer[0], buffer[0]));      
      }
    } else if (howMany == 3) {
      showPattern = false;
      for (int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(buffer[0], buffer[1], buffer[2]));      
      }
    } else if (howMany == 2) {
      showPattern = true;
      patternColor = buffer[0];
      patternSpeed = buffer[1];
    }
    pixels.show();   // Send the updated pixel colors to the hardware.
  }
  delay(DELAYVAL); // Pause before next pass through loop

}
