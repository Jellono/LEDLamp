#include "EffectBase.h"

#define MAX_CIRCLING_SPRITES 10

class BoxSprite
{
  public:
    BoxSprite();
    void initialize(uint32_t s_color, float s_spriteAngleSpeed, float s_spriteRadiusSpeed, float s_spriteLayerSpeed, float s_layer);
    bool stepSprite(double timeStep);
    uint32_t getColorAddition(int curx, int cury, int curz, uint32_t currentColor);
  private:
    byte r, g, b;
    float spriteAngleSpeed;
    float spriteRadiusSpeed;
    float spriteLayerSpeed;
    float angle;
    float radius;
    float layer;
    short x, y, z;
};

BoxSprite::BoxSprite() {
}

void BoxSprite::initialize(uint32_t s_color, float s_spriteAngleSpeed, float s_spriteRadiusSpeed, float s_spriteLayerSpeed, float s_layer) {
  r = (byte)((0x00FF0000 & s_color)>>16);
  g = (byte)((0x0000FF00 & s_color)>>8);
  b = (byte)((0x000000FF & s_color));
  spriteAngleSpeed = s_spriteAngleSpeed;
  spriteRadiusSpeed = s_spriteRadiusSpeed;
  spriteLayerSpeed = s_spriteLayerSpeed;
  angle = 0.0;
  radius = 0.0;
  layer = s_layer;
}

double maxRadius = (double)(CUBE_X + CUBE_Y)/2.0 * 0.70;

bool BoxSprite::stepSprite(double timeStep) {
  angle += spriteAngleSpeed*timeStep;
  radius += spriteRadiusSpeed*timeStep;
  layer += spriteLayerSpeed*timeStep;
    
  x = (short)(20.0*(radius*cos(angle)));
  y = (short)(20.0*(radius*sin(angle)));
  z = (short)(20.0*layer);
    
  if (radius<maxRadius && layer<((float)CUBE_Z)) return true;
  else return false;
}

uint32_t BoxSprite::getColorAddition(int curx, int cury, int curz, uint32_t currentColor) {
  
  if (abs((int)layer-curz)>1.0) return currentColor;
  
  int i;
  short fx, fy, fz;
  
  fx = x - (short)(20.0*((float)curx - (float)(CUBE_X-1)/2.0));
  fy = y - (short)(20.0*((float)cury - (float)(CUBE_Y-1)/2.0));
  fz = z - (short)(20*curz);
  fx=fx*fx;
  fy=fy*fy;
  fz=fz*fz;
  i = (fx+fy+fz);
  
  // brightness will be scaled by distance
  int addBrightness;
  int n_r, n_g, n_b;
  int a_r, a_g, a_b;
  double distance = ((double)i/400.0);

  uint32_t newcolor = 0;
  if (distance<1.0) {     
    n_r = ((0x00FF0000 & currentColor)>>16);
    n_g = ((0x0000FF00 & currentColor)>>8);
    n_b = ((0x000000FF & currentColor));

    addBrightness = (int)(100.0 * (1.0-distance));
    a_r = ((int)r) * addBrightness / 100;
    a_g = ((int)g) * addBrightness / 100;
    a_b = ((int)b) * addBrightness / 100;
    n_r = n_r + a_r;
    n_g = n_g + a_g;
    n_b = n_b + a_b;
    if (n_r>255) n_r = 255;
    if (n_g>255) n_g = 255;
    if (n_b>255) n_b = 255;
    newcolor = ((uint32_t)n_r)<<16 | ((uint32_t)n_g)<<8 | n_b;
  } else {
    newcolor = currentColor;
  }
  
  return newcolor;  
}

class CirclingBox : public EffectBase
{
  public:
    CirclingBox();
    void effectStep(double timeStep);
    void applyToStrip(Adafruit_NeoPixel &strip);
    void initialize(int s_brightness, int s_numElements, int s_controlParameter);
    void adjustBrightness(int newBrightness);
   private:
    int numSprites;
    BoxSprite sprite[MAX_CIRCLING_SPRITES];
};

CirclingBox::CirclingBox() : EffectBase() {
}

void CirclingBox::initialize(int s_brightness, int s_numElements, int s_controlParameter) {
  brightness = s_brightness;
  numSprites = (brightness*MAX_CIRCLING_SPRITES)/DIAL_MAX;
  if (numSprites<1) numSprites = 1;
  if (numSprites>MAX_CIRCLING_SPRITES) numSprites = MAX_CIRCLING_SPRITES;
  int i;
  
  for (i=0; i<numSprites; i++) {
    float s_angle, s_radius, s_layer, startLayer;
    s_angle = 0.5 + ((double)random(500))/1000.0;
    s_radius = 0.005 + ((double)random(100))/10000.0;
    s_layer = 0.025 + ((double)random(150))/1000.0;
    startLayer = (float)i * ((float)CUBE_Z / (float)MAX_CIRCLING_SPRITES);
    sprite[i].initialize(getRandomTwoChannelColor(), s_angle, s_radius, s_layer, startLayer);
  }
}


void CirclingBox::adjustBrightness(int newBrightness) {
  if (newBrightness!=brightness) {
    initialize(newBrightness, 0, 0);
  }
}


void CirclingBox::applyToStrip(Adafruit_NeoPixel &strip) {
  int i;
  
  int x, y, z;
  uint32_t currentColor;
  int pixelNum;
  for (z=0; z<CUBE_Z; z++) {
    for (y=0; y<CUBE_Y; y++) {
      for (x=0; x<CUBE_X; x++) {
        pixelNum = xyzToStrip(x, y, z);
        currentColor = 0;
        for (i=0; i<numSprites; i++) {
          currentColor = sprite[i].getColorAddition(x, y, z, currentColor);
        }
        strip.setPixelColor(pixelNum, currentColor);
      }
    }
  } 

  
}

void CirclingBox::effectStep(double timeStep) {
  int i;
  for (i=0; i<numSprites; i++) {
    if (!sprite[i].stepSprite(timeStep)) {
      float s_angle, s_radius, s_layer;
      s_angle = 0.5 + ((double)random(500))/1000.0;
      s_radius = 0.015 + ((double)random(100))/10000.0;
      s_layer = 0.025 + ((double)random(150))/1000.0;
      sprite[i].initialize(getRandomTwoChannelColor(), s_angle, s_radius, s_layer, 0.0);
    }
  } 

}


