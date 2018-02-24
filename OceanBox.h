#include "EffectBase.h"


class OceanBox
{
  public:
    OceanBox();
    void waveStep(int time);
    void applyToString();
    void initialize(int s_intensity);
    void drawColumn(int x, int y, double height);
   private:
    void randomizeParameters();
    int intensity;
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

void OceanBox::initialize(int s_intensity) {
  intensity = s_intensity;
  strip.clear();
  strip.show();
  startAngle = 0.0;
  startTime = millis();
  curAmplitude = 0.0;
  randomizeParameters();
}

void OceanBox::drawColumn(int x, int y, double height)
{
  int z = 0;
  int pixelNum;
  double proximity = 0.0;
  int red=0, green=0, blue=0;
     
  for (z=0; z<CUBE_Z; z++) {
    red = 0;
    green = 0;
    blue = 0;
    
    if ((y%2)==0) pixelNum = x + y*CUBE_X;
    else pixelNum = ((CUBE_X-1)-x) + y*CUBE_X;
    
    if ((z%2)==0) pixelNum = pixelNum + z*CUBE_X*CUBE_Y;
    else pixelNum = (CUBE_X*CUBE_Y-1-pixelNum) + z*CUBE_X*CUBE_Y;
    
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
    strip.setPixelColor(pixelNum, strip.Color(green*intensity/5, red*intensity/5, blue*intensity/5));
  }
}

void OceanBox::applyToString() {
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
      drawColumn(x, y, height);
    }
  }
}

void OceanBox::waveStep(int time) {
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


