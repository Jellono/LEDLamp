#ifndef EFFECT_BASE_H

class EffectBase
{
  public:
    EffectBase();
    virtual void effectStep(double timeStep);
    virtual void applyToStrip(Adafruit_NeoPixel &strip);
    virtual void initialize(int s_brightness, int s_numElements, int s_controlParameter);
    virtual void adjustBrightness(int newBrightness);
    int xyzToStrip(int x, int y, int z);
  protected:
    int brightness;
    int numElements;
    int controlParameter;
};

EffectBase::EffectBase()
{
  brightness = 0;
  numElements = 0;
  controlParameter = 0;
}

void EffectBase::effectStep(double timeStep)
{
}

void EffectBase::applyToStrip(Adafruit_NeoPixel &strip)
{
  strip.show(); 
}

void EffectBase::initialize(int s_brightness, int s_numElements, int s_controlParameter)
{
  brightness = s_brightness;
  numElements = s_numElements;
  controlParameter = s_controlParameter;
}

void EffectBase::adjustBrightness(int newBrightness)
{
  brightness = newBrightness;
}

int EffectBase::xyzToStrip(int x, int y, int z)
{
  int stripIndex = 0;
 
#ifdef BOX_TYPE_180_ROTATION 
  if ((z%2)==0) {
    if ((y%2)==0) {
      stripIndex = x + y*CUBE_X + z*CUBE_X*CUBE_Y;
    } else {
      stripIndex = ((CUBE_X-1)-x) + y*CUBE_X + z*CUBE_X*CUBE_Y;
    }
  } else {
    if ((y%2)==0) {
      stripIndex = ((z+1)*CUBE_X*CUBE_Y-1) - x - y*CUBE_X;
    } else {
      stripIndex = ((z+1)*CUBE_X*CUBE_Y-1) - ((CUBE_X-1)-x) - y*CUBE_X;
    }
  }
#else
  if ((z%2)==0) {
    if ((y%2)==0) {
      stripIndex = x + y*CUBE_X + z*CUBE_X*CUBE_Y;
    } else {
      stripIndex = ((CUBE_X-1)-x) + y*CUBE_X + z*CUBE_X*CUBE_Y;
    }
  } else {
    if ((x%2)==0) {
      stripIndex = ((z+1)*CUBE_X*CUBE_Y-1) - y - x*CUBE_Y;
    } else {
      stripIndex = ((z+1)*CUBE_X*CUBE_Y-1) - ((CUBE_Y-1)-y) - x*CUBE_Y;
    }
  }
#endif
  
  return stripIndex;
}

#define EFFECT_BASE_H
#endif
