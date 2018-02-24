#include <ky-040.h>

#include <Adafruit_NeoPixel.h>
#include <math.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6
#define CUBE_X  5
#define CUBE_Y  5
#define CUBE_Z  5
#define BUTTON_1  5
#define BUTTON_2  7
#define DIAL_PIN_A 2
#define DIAL_PIN_B 4

#define ENCODER_CLK1         2      // This pin must have a minimum 0.47 uF capacitor
#define ENCODER_DT1          4      // data pin
#define ENCODER_SW1          5      // switch pin (active LOW)
#define MAX_ROTARIES1        2      // this example defines two rotaries for this encoder
ky040 encoder1(ENCODER_CLK1, ENCODER_DT1, ENCODER_SW1, MAX_ROTARIES1 );


#define DIAL_MIN  1
#define DIAL_MAX  50


// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(CUBE_X*CUBE_Y*CUBE_Z, PIN, NEO_RGB + NEO_KHZ800);

#include "EffectBase.h"
#include "FireColumn.h"
#include "CirclingBox.h"
#include "ColorLayers.h"
#include "FireFly.h"
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


int button1State;
int button2State;
int lampMode;
int timedFadeIntensity;
unsigned long modeStartTime;
unsigned long lastChangeTime;
int solidBrightness;
int dialBrightness;

int encoderPosCount = 0; 
int pinALast;  
int aVal;
boolean bCW;


// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
  }
}


uint32_t getRandomTwoChannelColor()
{
  int channel = random(3);
  if (channel==0) {
    return strip.Color(random(256), random(256), 0);
  } else if (channel==1) {
    return strip.Color(random(256), 0, random(256));
  } else {
    return strip.Color(0, random(256), random(256));
  }
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos, int brightness) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color((255 - WheelPos * 3)*brightness/DIAL_MAX, 0, (WheelPos * 3)*brightness/DIAL_MAX);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, (WheelPos * 3)*brightness/DIAL_MAX, (255 - WheelPos * 3)*brightness/DIAL_MAX);
  }
  WheelPos -= 170;
  return strip.Color((WheelPos * 3)*brightness/DIAL_MAX, (255 - WheelPos * 3)*brightness/DIAL_MAX, 0);
}



void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code

  lampMode = -1;
  button1State = 0;
  button2State = 0;
  timedFadeIntensity = 70;
  dialBrightness = DIAL_MAX/2;
  solidBrightness = dialBrightness/10;
  int solidColorDone = 0;
  modeStartTime = millis();
  pinMode(BUTTON_1, INPUT);
  pinMode(BUTTON_2, INPUT);
  pinMode(DIAL_PIN_A, INPUT);
  pinMode(DIAL_PIN_B, INPUT);

  pinALast = digitalRead(DIAL_PIN_A);   
   
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  int i; 
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0,0,0));
  }

  encoder1.AddRotaryCounter(1, DIAL_MAX, 0, DIAL_MAX/2, 1, false );
  encoder1.SetRotary(1);
  
  Serial.begin(115200) ;   // write out the values. Make sure the Terminal is also set to 9600
  while ( ! Serial ) ;

}




CirclingBox spriteBox;
ColorLayers colorLayerBox;
FireFlyBox flyBox;
OceanBox waveBox;
FireBox fireBox;

void loop() {
  
  //readDial();

  if (lampMode==-1) {
    fireBox.initialize(100, 1);
    waveBox.initialize(5);
    flyBox.initialize(1, 5);
    colorLayerBox.initialize(solidBrightness);
    spriteBox.initialize(1, 1, 2*solidBrightness);
    lampMode = 0;
  }

  if (lampMode==0) {
    fireBox.fireStep(1);
    fireBox.applyToString();
    strip.show();
  } else if (lampMode==1) {
    spriteBox.circleStep(1.0);
    spriteBox.applyToString();
    strip.show();
  } else if (lampMode==2) {
    colorLayerBox.adjustBrightness(dialBrightness);
    colorLayerBox.effectStep(0.1);
    colorLayerBox.applyToStrip(strip);
    strip.show();
  } else if (lampMode==3) {
    flyBox.boxStep(1);
    flyBox.applyToString();
    strip.show();
  } else if (lampMode==4) {
    waveBox.waveStep(1);
    waveBox.applyToString();
    strip.show();
  }
  
  if (encoder1.SwitchPressed()) {
//    if (button1State==0) {
//      button1State = 1;
      lampMode++;
      modeStartTime = millis();
      lastChangeTime = modeStartTime;
      timedFadeIntensity = dialBrightness*2;
      
/*      if (lampMode==1) {
        spriteBox.initialize(1, 1, 2*solidBrightness);
      }
      if (lampMode==2) {
        colorLayerBox.initialize(solidBrightness);
      }
      if (lampMode==3) {
        flyBox.initialize(5, solidBrightness);
      } 
      if (lampMode==4) {
        waveBox.initialize(5);
      }
*/
//    } else {
//      button1State = 0;
//    }

    encoder1.SetChanged();
  }

  if (encoder1.HasRotaryValueChanged()) {
    dialBrightness = encoder1.GetRotaryValue(1);
    Serial.print("Encoder 1 ROTARY_ID1: "); Serial.println(encoder1.GetRotaryValue(1));

    solidBrightness = dialBrightness / 10;
    if (solidBrightness<1) solidBrightness = 1;
    if (lampMode==0) {
      timedFadeIntensity = dialBrightness*2;
      if (timedFadeIntensity<4) timedFadeIntensity = 4;
      fireBox.initialize(timedFadeIntensity, 1);
    } else if (lampMode==1) {
      spriteBox.initialize(1, 1, 2*solidBrightness);
    } else if (lampMode==2) {
//      colorLayerBox.adjustBrightness(solidBrightness);
    } else if (lampMode==3) {
      flyBox.initialize(5, solidBrightness);
    } else if (lampMode==4) {
      waveBox.initialize(solidBrightness);
    }
  }

/*
  if (digitalRead(BUTTON_2)==0) {
    if (button2State==0) {
      button2State = 1;
      if (lampMode>0) {
        solidBrightness = solidBrightness - 1;
        if (solidBrightness==0) solidBrightness = 5;
      }
      if (lampMode==0) {
        timedFadeIntensity = timedFadeIntensity - 10;
        if (timedFadeIntensity < 1) timedFadeIntensity = 100;
        fireBox.initialize(timedFadeIntensity, 1);
      } else if (lampMode==1) {
        spriteBox.initialize(1, 1, 2*solidBrightness);
      } else if (lampMode==2) {
        colorLayerBox.adjustBrightness(solidBrightness);
      } else if (lampMode==3) {
        flyBox.initialize(5, solidBrightness);
      } else if (lampMode==4) {
        waveBox.initialize(solidBrightness);
      }
    }
  } else {
    button2State = 0;
  }
*/

  unsigned long currentTime = millis();
  if ((currentTime - lastChangeTime)>216000) {
    lastChangeTime = currentTime;
    if (lampMode==0) {
      timedFadeIntensity = timedFadeIntensity - 1;
      if (timedFadeIntensity < 4) timedFadeIntensity = 4;
      fireBox.initialize(timedFadeIntensity, 1);
    } 
  }
    
  if (lampMode<0) lampMode = 4;
  if (lampMode>4) lampMode = 0;

}



