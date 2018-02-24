#include "EffectBase.h"


class ColorLayers : public EffectBase
{
  public:
    ColorLayers();
    void effectStep(double timeStep);
    void applyToStrip(Adafruit_NeoPixel &strip);
    void initialize(int s_brightness);
  private:
    double colorPos;
};

ColorLayers::ColorLayers() : EffectBase() {
}

void ColorLayers::initialize(int s_brightness) {
  colorPos = 0.0;
  EffectBase::initialize(s_brightness, 0, 0);
}

void ColorLayers::effectStep(double timeStep) {
  colorPos = colorPos + timeStep;
}

void ColorLayers::applyToStrip(Adafruit_NeoPixel &strip) {
  uint32_t layerColors[CUBE_Z];
  int i, j;
  for (i=0; i<CUBE_Z; i++) {
    layerColors[i] = Wheel(((int)colorPos + i*20)%256, EffectBase::brightness);
    if (colorPos>255.0) colorPos = 0.0;
    for (j=0; j<CUBE_X*CUBE_Y; j++) {
      strip.setPixelColor(j+i*CUBE_X*CUBE_Y, layerColors[i]);
    }
  }
}

