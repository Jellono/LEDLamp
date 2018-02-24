#include "EffectBase.h"


class ColorLayers : public EffectBase
{
  public:
    ColorLayers();
    void effectStep(double timeStep);
    void applyToStrip(Adafruit_NeoPixel &strip);
    void initialize(int s_brightness, int s_numElements, int s_controlParameter);
  private:
    double colorPos;
};

ColorLayers::ColorLayers() : EffectBase() {
}

void ColorLayers::initialize(int s_brightness, int s_numElements, int s_controlParameter) {
  colorPos = 0.0;
  EffectBase::initialize(s_brightness, 0, 0);
}

void ColorLayers::effectStep(double timeStep) {
  colorPos = colorPos + timeStep;
  if (colorPos>255.0) colorPos = 0.0;
}

void ColorLayers::applyToStrip(Adafruit_NeoPixel &strip) {
  uint32_t pixelColor;
  int i, j, k;
  int ring;
  for (k=0; k<CUBE_Z; k++) {
    int stripIndex;
    for (i=0; i<CUBE_X; i++) {
      for (j=0; j<CUBE_Y; j++) {
        ring = 0;
        if ( i>0 && i<(CUBE_X-1) && j>0 && j<(CUBE_Y-1) ) ring++;
        if ( i>1 && i<(CUBE_X-2) && j>1 && j<(CUBE_Y-2) ) ring++;
        pixelColor = Wheel(((int)colorPos + (k+ring*2)*20)%256, EffectBase::brightness);
        strip.setPixelColor(xyzToStrip(i, j, k), pixelColor);
      }
    }
  }
}

