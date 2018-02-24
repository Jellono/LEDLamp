#include "EffectBase.h"


class OceanBox : public EffectBase
{
  public:
    OceanBox();
    void effectStep(double timeStep);
    void applyToStrip(Adafruit_NeoPixel &strip);
    void initialize(int s_brightness, int s_numElements, int s_controlParameter);
    void adjustBrightness(int newBrightness);
   private:
    void drawColumn(int x, int y, double height, Adafruit_NeoPixel &strip);
    void randomizeParameters();
    double amplitude;
    double curAmplitude;
    double xAngleStep;
    double yAngleStep;
    double startAngle;
    unsigned long startTime;
    double rampUpTime;
    double rampDownTime;
};

OceanBox::OceanBox() {
}


void OceanBox::randomizeParameters() {
  amplitude = (double)random(100) / 50.0;
  xAngleStep = PI/((double)random(4,13));
  yAngleStep = PI/((double)random(4,13));
  startTime = millis();
  rampUpTime = double(random(1, 10));
  rampDownTime = double(random(3, 16));
  
  if (random(2)==1) xAngleStep = xAngleStep * -1.0;
  if (random(2)==1) yAngleStep = yAngleStep * -1.0;
  if (random(10)==0) {
    xAngleStep = xAngleStep/5;
    yAngleStep = yAngleStep/5;
  }
}

void OceanBox::adjustBrightness(int newBrightness) {
  if (newBrightness!=brightness) {
    brightness = newBrightness;
  }
}

void OceanBox::initialize(int s_brightness, int s_numElements, int s_controlParameter) {
  brightness = s_brightness;
  numElements = s_numElements;
  controlParameter = s_controlParameter;
  startAngle = 0.0;
  startTime = millis();
  curAmplitude = 0.0;
  randomizeParameters();
}

void OceanBox::drawColumn(int x, int y, double height, Adafruit_NeoPixel &strip)
{
  int z = 0;
  int pixelNum;
  double proximity = 0.0;
  int red=0, green=0, blue=0;
     
  for (z=0; z<CUBE_Z; z++) {
    red = 0;
    green = 0;
    blue = 0;
    
    pixelNum = xyzToStrip(x, y, z);
        
    if ( height>(double)z ) {
      blue = 60;
    } 
    proximity = abs((double)z-height);
    double adjProximity;
    if (proximity<1.0) {
      adjProximity = (1.0-proximity)*(1.0-proximity);
      green = (int)(adjProximity*150.0);
      blue = blue + (int)((adjProximity)*190.0);
    }
    if (blue>255) blue = 255;
    strip.setPixelColor(pixelNum, strip.Color(green*brightness/DIAL_MAX, red*brightness/DIAL_MAX, blue*brightness/DIAL_MAX));
  }
}

void OceanBox::applyToStrip(Adafruit_NeoPixel &strip) {
  int i;
  for (i=0; i<(CUBE_X*CUBE_Y*CUBE_Z); i++) {
    strip.setPixelColor(i, 0);
  }
  
  int x, y;
  double currentAngle, height;
  for (y=0; y<CUBE_Y; y++) {
    currentAngle = startAngle + yAngleStep*y;
    for (x=0; x<CUBE_X; x++) {
      
      height = 1.0 + curAmplitude * sin(currentAngle + xAngleStep*x);
      drawColumn(x, y, height, strip);
    }
  }
}

void OceanBox::effectStep(double timeStep) {
  startAngle = startAngle + 0.1;
  
  curAmplitude = 0.0;
  double elapsedTime = ((double)millis()-(double)startTime)/1000.0;
  
  if (elapsedTime>0.0) {
    if (elapsedTime<rampUpTime) {
      curAmplitude = (elapsedTime/rampUpTime)*amplitude;
    } else if (elapsedTime<(rampUpTime+rampDownTime)) {
      curAmplitude = (1.0 - (elapsedTime-rampUpTime)/rampDownTime)*amplitude;
    } else {
      randomizeParameters();
      startTime = millis() + random(4,12)*1000;
    }
  }
}


