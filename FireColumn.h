#include "EffectBase.h"





class FireColumn 
{
  public:
    FireColumn();
    void fadeStep(int rowNumber, int columnNumber, EffectBase *baseClass, Adafruit_NeoPixel &thisStrip);
    void initialize(int s_intensityRange, int s_stepRange);
  private:
    int intensityRange;
    uint32_t startColor;
    uint32_t endColor;
    int numSteps;
    int currentStep;
    int stepRange;
};

FireColumn::FireColumn() {
}

void FireColumn::initialize(int s_intensityRange, int s_stepRange) {
  intensityRange = s_intensityRange;
  startColor = GenerateFireColor(random(intensityRange));
  endColor = GenerateFireColor(random(intensityRange));

  stepRange = s_stepRange;
  currentStep = 0;
  numSteps = random(stepRange) + 1;
}


void FireColumn::fadeStep(int rowNumber, int columnNumber, EffectBase *baseClass, Adafruit_NeoPixel &thisStrip) {
  int r1, g1, b1;
  int r2, g2, b2;
  int nr, ng, nb;
  r1 = (startColor&0x00FF0000)>>16;
  g1 = (startColor&0x0000FF00)>>8;
  b1 = (startColor&0x000000FF);
  r2 = (endColor&0x00FF0000)>>16;
  g2 = (endColor&0x0000FF00)>>8;
  b2 = (endColor&0x000000FF);

  uint32_t currentColor[CUBE_Z];
  int layer;
  
  if (currentStep<numSteps) {
    nr = (r2-r1)*currentStep/numSteps + r1;
    ng = (g2-g1)*currentStep/numSteps + g1;
    nb = (b2-b1)*currentStep/numSteps + b1;
    currentColor[0] = thisStrip.Color( nr, ng, nb);
    for (layer=1; layer<CUBE_Z; layer++) {
      currentColor[layer] = thisStrip.Color( nr/(3*layer), ng/(3*layer), nb/(3*layer));
    }
    currentStep = currentStep + 1;
  } else {
    currentColor[0] = endColor;
    for (layer=1; layer<CUBE_Z; layer++) {
      currentColor[layer] = thisStrip.Color( r2/(3*layer), g2/(3*layer), b2/(3*layer));
    }
    startColor = endColor;
    endColor = GenerateFireColor(random(intensityRange));
    currentStep = 0;
    numSteps = random(stepRange) + 1;
  }

  for (layer=0; layer<CUBE_Z; layer++) {
    int stripIndex = baseClass->xyzToStrip(rowNumber, columnNumber, layer);
    thisStrip.setPixelColor(stripIndex, currentColor[layer]);
  }
}


class FireBox : public EffectBase
{
  public:
    FireBox();
    void effectStep(double timeStep);
    void applyToStrip(Adafruit_NeoPixel &strip);
    void initialize(int s_brightness, int s_numElements, int s_controlParameter);
    void adjustBrightness(int newBrightness);
   private:
    FireColumn column[CUBE_X][CUBE_Y];
};


FireBox::FireBox() {
}

void FireBox::initialize(int s_brightness, int s_numElements, int s_controlParameter) {
  brightness = s_brightness;
  if (brightness>DIAL_MAX) brightness = DIAL_MAX;
  if (brightness<5) brightness = 5;
  numElements = s_numElements;
  controlParameter = s_controlParameter;
  
  int i, j;
  double center_x, center_y;
  center_x = (double)(CUBE_X - 1) / 2.0;
  center_y = (double)(CUBE_Y - 1) / 2.0;
  double curX, curY;
  double distanceToCenter;
  double intensityAdjustment;
  double maxDistance = sqrt(center_x*center_x + center_y*center_y);
  int newIntensity;
  int timeConstant;
  for (i=0; i<CUBE_X; i++) {
    for (j=0; j<CUBE_Y; j++) {
      curX = (double)(i);
      curY = (double)(j);
      
      distanceToCenter = sqrt((curX-center_x)*(curX-center_x) + (curY-center_y)*(curY-center_y));
      if (distanceToCenter<1.0) distanceToCenter = 0.0;
      intensityAdjustment = 1.0 - (distanceToCenter/maxDistance);
      
      newIntensity = ((int)(intensityAdjustment*215.0 + 40.0))*brightness/DIAL_MAX;
      timeConstant = 500 + (int)(500.0*intensityAdjustment);
      column[i][j].initialize(newIntensity, 40 + timeConstant/brightness);
    }
  }
}


void FireBox::adjustBrightness(int newBrightness) {
  if (newBrightness<5) newBrightness = 5;
  if (newBrightness!=brightness) {
    initialize(newBrightness, 0, 0);
  }
}


void FireBox::effectStep(double timeStep) {
}


void FireBox::applyToStrip(Adafruit_NeoPixel &strip) {
  int i, j;
  for (i=0; i<CUBE_X; i++) {
    for (j=0; j<CUBE_Y; j++) {
      column[i][j].fadeStep(i, j, this, strip);
    }
  }
}


