#include "EffectBase.h"



uint32_t GenerateFireColor(int range) {
  int divider = random(5)+2;
  return strip.Color(range/divider, range, 0);
}



class FireColumn
{
  public:
    FireColumn();
    void fadeStep(int columnNumber);
    void applyToColumn(int columnNumber);
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


void FireColumn::fadeStep(int columnNumber) {
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
    currentColor[0] = strip.Color( nr, ng, nb);
    for (layer=1; layer<CUBE_Z; layer++) {
      currentColor[layer] = strip.Color( nr/(3*layer), ng/(3*layer), nb/(3*layer));
    }
    currentStep = currentStep + 1;
  } else {
    currentColor[0] = endColor;
    for (layer=1; layer<CUBE_Z; layer++) {
      currentColor[layer] = strip.Color( r2/(3*layer), g2/(3*layer), b2/(3*layer));
    }
    startColor = endColor;
    endColor = GenerateFireColor(random(intensityRange));
    currentStep = 0;
    numSteps = random(stepRange) + 1;
  }

  for (layer=0; layer<CUBE_Z; layer++) {
    if ((layer%2)==0) {
      strip.setPixelColor(columnNumber + (CUBE_X*CUBE_Y)*layer, currentColor[layer]);
    } else {
      strip.setPixelColor(((CUBE_X*CUBE_Y)*(layer+1)-1)-columnNumber, currentColor[layer]);
    }
  }
}

void FireColumn::applyToColumn(int columnNumber) {
}


class FireBox
{
  public:
    FireBox();
    void fireStep(int time);
    void applyToString();
    void initialize(int s_intensity, int s_fireSpeed);
   private:
    int intensity;
    int fireSpeed;
    FireColumn column[CUBE_X*CUBE_Y];
};


FireBox::FireBox() {
}

void FireBox::initialize(int s_intensity, int s_fireSpeed) {
  intensity = s_intensity;
  fireSpeed = s_fireSpeed;
  
  int i;
  double center_x, center_y;
  center_x = (double)(CUBE_X - 1) / 2.0;
  center_y = (double)(CUBE_Y - 1) / 2.0;
  double curX, curY;
  double distanceToCenter;
  double intensityAdjustment;
  double maxDistance = sqrt(center_x*center_x + center_y*center_y);
  int newIntensity;
  int timeConstant;
  for (int i=0; i<(CUBE_X*CUBE_Y); i++) {
    curX = (double)(i%CUBE_X);
    curY = (double)(i/CUBE_X);
    
    distanceToCenter = sqrt((curX-center_x)*(curX-center_x) + (curY-center_y)*(curY-center_y));
    if (distanceToCenter<1.0) distanceToCenter = 0.0;
    intensityAdjustment = 1.0 - (distanceToCenter/maxDistance);
    
    newIntensity = ((int)(intensityAdjustment*215.0 + 40.0))*intensity/100;
    timeConstant = 500 + (int)(500.0*intensityAdjustment);
    column[i].initialize(newIntensity, 40 + timeConstant/intensity);
  }
}

void FireBox::fireStep(int time) {
  int i;
  for (i=0; i<(CUBE_X*CUBE_Y); i++) {
    column[i].fadeStep(i);
  }
}


void FireBox::applyToString() {
  int i;
  for (i=0; i<(CUBE_X*CUBE_Y); i++) {
    column[i].applyToColumn(i);
  }
}


