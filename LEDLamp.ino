#define SMALLEST_CODESIZE
#include <ky-040.h>

#include <Adafruit_NeoPixel.h>
#include <math.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif


// *** uncomment the following based on the hardware ***
#define BOX_WITH_SINGLE_DIAL
//#define BOX_WITH_TWO_BUTTONS
//#define BOX_WITH_FOUR_BUTTONS
#define BOX_WITH_NEW_LEDS
//#define BOX_TYPE_180_ROTATION 

#define LED_OUTPUT_PIN  6

#define CUBE_X  5
#define CUBE_Y  5
#define CUBE_Z  5

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
#ifdef BOX_WITH_NEW_LEDS
  Adafruit_NeoPixel stripOfLEDs = Adafruit_NeoPixel(CUBE_X*CUBE_Y*CUBE_Z, LED_OUTPUT_PIN, NEO_RGB + NEO_KHZ800);
#else
  Adafruit_NeoPixel stripOfLEDs = Adafruit_NeoPixel(CUBE_X*CUBE_Y*CUBE_Z, LED_OUTPUT_PIN, NEO_GRB + NEO_KHZ800);
#endif

#ifdef BOX_WITH_SINGLE_DIAL  
  #define ENCODER_CLK1         2      // This pin must have a minimum 0.47 uF capacitor
  #define ENCODER_DT1          4      // data pin
  #define ENCODER_SW1          5      // switch pin (active LOW)
  #define MAX_ROTARIES1        2      // this example defines two rotaries for this encoder
  ky040 encoder1(ENCODER_CLK1, ENCODER_DT1, ENCODER_SW1, MAX_ROTARIES1 );
#endif

#ifdef BOX_WITH_TWO_BUTTONS  
  #define BUTTON_1_PIN          3
  #define BUTTON_2_PIN          2
  bool button1State;
  bool button2State;
#endif 

#ifdef BOX_WITH_FOUR_BUTTONS  
  #define BUTTON_1_PIN          2
  #define BUTTON_2_PIN          3
  #define BUTTON_3_PIN          4
  #define BUTTON_4_PIN          5
  bool button1State;
  bool button2State;
  bool button3State;
  bool button4State;
  unsigned long  timeButton1Pressed;
  unsigned long  timeButton3Pressed;
  int brightnessStart;
#endif 

#define DIAL_MIN  1
#define DIAL_MAX  50


#include "EffectBase.h"

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<stripOfLEDs.numPixels(); i++) {
    stripOfLEDs.setPixelColor(i, c);
    stripOfLEDs.show();
  }
}


uint32_t getRandomTwoChannelColor()
{
  int channel = random(3);
  if (channel==0) {
    return stripOfLEDs.Color(random(256), random(256), 0);
  } else if (channel==1) {
    return stripOfLEDs.Color(random(256), 0, random(256));
  } else {
    return stripOfLEDs.Color(0, random(256), random(256));
  }
}

uint32_t GenerateFireColor(int range) {
  int divider = random(5)+2;
  return stripOfLEDs.Color(range/divider, range, 0);
}

uint32_t GenerateNearlyWhite() {
  return stripOfLEDs.Color(random(20,220), random(20,220), random(20,220));
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos, int brightness) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return stripOfLEDs.Color((255 - WheelPos * 3)*brightness/DIAL_MAX, 0, (WheelPos * 3)*brightness/DIAL_MAX);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return stripOfLEDs.Color(0, (WheelPos * 3)*brightness/DIAL_MAX, (255 - WheelPos * 3)*brightness/DIAL_MAX);
  }
  WheelPos -= 170;
  return stripOfLEDs.Color((WheelPos * 3)*brightness/DIAL_MAX, (255 - WheelPos * 3)*brightness/DIAL_MAX, 0);
}


void testPlaneWipe(uint32_t color, int delayAmount) {
  int x, y, z, stripIndex, i;
  EffectBase baseBox;
  baseBox.initialize(100, 0, 0);
  for (x=0; x<CUBE_X; x++) {
    for (y=0; y<CUBE_Y; y++) {
      for (z=0; z<CUBE_Z; z++) {
        stripIndex = baseBox.xyzToStrip(x, y, z);
        stripOfLEDs.setPixelColor(stripIndex, color);
      }
    }
    stripOfLEDs.show();
    delay(delayAmount);
    for(i=0; i<stripOfLEDs.numPixels(); i++) {
      stripOfLEDs.setPixelColor(i, stripOfLEDs.Color(0,0,0));
    }
  }

  for (y=0; y<CUBE_Y; y++) {
    for (x=0; x<CUBE_X; x++) {
      for (z=0; z<CUBE_Z; z++) {
        stripIndex = baseBox.xyzToStrip(x, y, z);
        stripOfLEDs.setPixelColor(stripIndex, color);
      }
    }
    stripOfLEDs.show();
    delay(delayAmount);
    for(i=0; i<stripOfLEDs.numPixels(); i++) {
      stripOfLEDs.setPixelColor(i, stripOfLEDs.Color(0,0,0));
    }
  }
  
  for (z=0; z<CUBE_Z; z++) {
    for (y=0; y<CUBE_Y; y++) {
      for (x=0; x<CUBE_X; x++) {
        stripIndex = baseBox.xyzToStrip(x, y, z);
        stripOfLEDs.setPixelColor(stripIndex, color);
      }
    }
    stripOfLEDs.show();
    delay(delayAmount);
    for(i=0; i<stripOfLEDs.numPixels(); i++) {
      stripOfLEDs.setPixelColor(i, stripOfLEDs.Color(0,0,0));
    }
  }
}

#include "FireColumn.h"
#include "CirclingBox.h"
#include "ColorLayers.h"
//#include "FireFly.h"
#include "OceanBox.h"
#include "TreeBox.h"

//Colorbands
//Stars 
//Fireworks
//cyclone

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.


int lampMode;
unsigned long modeStartTime;
unsigned long lastChangeTime;
int dialBrightness;


CirclingBox spriteBox;
ColorLayers colorLayerBox;
OceanBox waveBox;
FireBox fireBox;
#define NUM_LIGHT_BOX_EFFECTS  4
EffectBase *lightBoxEffect[NUM_LIGHT_BOX_EFFECTS];

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code

  lampMode = -1;
  dialBrightness = DIAL_MAX;
  modeStartTime = millis();

  stripOfLEDs.begin();
  stripOfLEDs.show(); // Initialize all pixels to 'off'

  int i; 
  for(i=0; i<stripOfLEDs.numPixels(); i++) {
    stripOfLEDs.setPixelColor(i, stripOfLEDs.Color(0,0,0));
  }


  //Wipe through box
  testPlaneWipe(stripOfLEDs.Color(255,0,0), 25);
  testPlaneWipe(stripOfLEDs.Color(0,255,0), 25);
  testPlaneWipe(stripOfLEDs.Color(0,0,255), 25);
  testPlaneWipe(stripOfLEDs.Color(255,255,255), 25);

  #ifdef BOX_WITH_TWO_BUTTONS
    pinMode(BUTTON_1_PIN, INPUT_PULLUP);
    pinMode(BUTTON_2_PIN, INPUT_PULLUP);
    button1State = false;
    button2State = false;
  #endif

  #ifdef BOX_WITH_FOUR_BUTTONS
    pinMode(BUTTON_1_PIN, INPUT_PULLUP);
    pinMode(BUTTON_2_PIN, INPUT_PULLUP);
    pinMode(BUTTON_3_PIN, INPUT_PULLUP);
    pinMode(BUTTON_4_PIN, INPUT_PULLUP);
    button1State = false;
    button2State = false;
    button3State = false;
    button4State = false;
  #endif

  
  #ifdef BOX_WITH_ONE_DIAL 
    encoder1.AddRotaryCounter(1, DIAL_MAX, 0, DIAL_MAX, 1, false );
    encoder1.SetRotary(1);
  #endif 
  
  
  fireBox.initialize(dialBrightness, 0, 0);
  waveBox.initialize(dialBrightness, 0, 0);
//  flyBox.initialize(dialBrightness, 0, 0);
  colorLayerBox.initialize(dialBrightness, 0, 0);
  spriteBox.initialize(dialBrightness, 0, 0);
  
  lightBoxEffect[0] = &fireBox;
  lightBoxEffect[1] = &waveBox;
  lightBoxEffect[2] = &colorLayerBox;
  lightBoxEffect[3] = &spriteBox;
  
  lampMode = 0;
 
}



void loop() {
  
  if (lampMode<NUM_LIGHT_BOX_EFFECTS) {
    lightBoxEffect[lampMode]->adjustBrightness(dialBrightness);
    lightBoxEffect[lampMode]->effectStep(1.0);
    lightBoxEffect[lampMode]->applyToStrip(stripOfLEDs);
    stripOfLEDs.show();
  }

#ifdef BOX_WITH_ONE_DIAL
  if (encoder1.SwitchPressed()) {
    lampMode++;
    modeStartTime = millis();
    lastChangeTime = modeStartTime;
     
    encoder1.SetChanged();
    if (lampMode<0) lampMode = NUM_LIGHT_BOX_EFFECTS-1;
    if (lampMode==NUM_LIGHT_BOX_EFFECTS) lampMode = 0;
  }

  if (encoder1.HasRotaryValueChanged()) {
    dialBrightness = encoder1.GetRotaryValue(1);
  }
#endif

#ifdef BOX_WITH_TWO_BUTTONS
  if (!digitalRead(BUTTON_1_PIN)) {
    if (button1State==false) {
      button1State = true;
      lampMode++;
      modeStartTime = millis();
      lastChangeTime = modeStartTime;
      if (lampMode<0) lampMode = NUM_LIGHT_BOX_EFFECTS-1;
      if (lampMode==NUM_LIGHT_BOX_EFFECTS) lampMode = 0;
    }
  } else {
    button1State = false;
  }
  if (!digitalRead(BUTTON_2_PIN)) {
    if (button2State==false) {
      button2State = true;
      dialBrightness -= DIAL_MAX/8;
      if (dialBrightness<DIAL_MIN) dialBrightness = DIAL_MAX;
    }
  } else {
    button2State = false;
  }
#endif

  unsigned long currentTime = millis();

#ifdef BOX_WITH_FOUR_BUTTONS
  if (!digitalRead(BUTTON_2_PIN)) {
    if (button2State==false) {
      button2State = true;
      lampMode++;
      modeStartTime = millis();
      lastChangeTime = modeStartTime;
      if (lampMode<0) lampMode = NUM_LIGHT_BOX_EFFECTS-1;
      if (lampMode==NUM_LIGHT_BOX_EFFECTS) lampMode = 0;
    }
  } else {
    button2State = false;
  }
  if (!digitalRead(BUTTON_4_PIN)) {
    if (button4State==false) {
      button4State = true;
      lampMode--;
      modeStartTime = millis();
      lastChangeTime = modeStartTime;
      if (lampMode<0) lampMode = NUM_LIGHT_BOX_EFFECTS-1;
      if (lampMode==NUM_LIGHT_BOX_EFFECTS) lampMode = 0;
    }
  } else {
    button4State = false;
  }

  if (!digitalRead(BUTTON_1_PIN)) {
    if (button1State==false) {
      timeButton1Pressed = millis();
      brightnessStart = dialBrightness;
      button1State = true;
    } else {
      int timeDiff = currentTime - timeButton1Pressed;
      dialBrightness = brightnessStart + timeDiff/40;
      if (dialBrightness>DIAL_MAX) dialBrightness = DIAL_MAX;
    }
  } else {
    button1State = false;
  }

  if (!digitalRead(BUTTON_3_PIN)) {
    if (button3State==false) {
      timeButton3Pressed = millis();
      brightnessStart = dialBrightness;
      button3State = true;
    } else {
      int timeDiff = currentTime - timeButton3Pressed;
      dialBrightness = brightnessStart - timeDiff/40;
      if (dialBrightness<DIAL_MIN) dialBrightness = DIAL_MIN;
    }
  } else {
    button3State = false;
  }

#endif

  // Fade out fire effect
  if ((currentTime - lastChangeTime)>432000) {
    lastChangeTime = currentTime;
    if (lampMode==0) {
      dialBrightness = dialBrightness - 1;
    } 
  }

    

}



