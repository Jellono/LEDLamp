#include "EffectBase.h"


class BoxSprite
{
  public:
    BoxSprite();
    void initialize(uint32_t s_color, double s_spriteAngleSpeed, double s_spriteRadiusSpeed, int s_layer);
    bool stepSprite();
    void applyToString();
  private:
    int r, g, b;
    double spriteAngleSpeed;
    double spriteRadiusSpeed;
    int spriteLayer;
    double angle;
    double radius;
};

BoxSprite::BoxSprite() {
}

void BoxSprite::initialize(uint32_t s_color, double s_spriteAngleSpeed, double s_spriteRadiusSpeed, int s_layer) {
  r = (0x00FF0000 & s_color)>>16;
  g = (0x0000FF00 & s_color)>>8;
  b = (0x000000FF & s_color);
  spriteAngleSpeed = s_spriteAngleSpeed;
  spriteRadiusSpeed = s_spriteRadiusSpeed;
  spriteLayer = s_layer;
  angle = 0.0;
  radius = 0.0;
}

bool BoxSprite::stepSprite() {
  angle += spriteAngleSpeed;
  radius += spriteRadiusSpeed;
  
  double maxRadius = (double)(CUBE_X + CUBE_Y)/2.0 * 0.70;
  
  if (radius<2.75) return true;
  else return false;
}

void BoxSprite::applyToString() {
  double x, y;
  x = (radius*cos(angle));
  y = (radius*sin(angle));
  
  int i;
  double curx, cury;
  uint32_t currentColor;
  for (i=0; i<CUBE_X*CUBE_Y; i++) {
    currentColor = strip.getPixelColor(i);
    cury = (double)(i/CUBE_X);
    if ((i/CUBE_X)%2==0) {
      curx = (double)(i%CUBE_X);
    } else {
      curx = (double)(CUBE_X-1) - (double)(i%CUBE_X);
    }
    curx = curx - (double)(CUBE_X-1)/2.0;
    cury = cury - (double)(CUBE_Y-1)/2.0;    

    // brightness will be scaled by distance
    int addBrightness;
    int n_r, n_g, n_b;
    int a_r, a_g, a_b;
    double distance = sqrt((curx-x)*(curx-x)+(cury-y)*(cury-y));
    if (distance<1.0) {     
      addBrightness = (int)(100.0 * (1.0-distance));
      a_r = r * addBrightness / 100;
      a_g = g * addBrightness / 100;
      a_b = b * addBrightness / 100;
      n_r = ((0x00FF0000 & currentColor)>>16) + a_r;
      n_g = ((0x0000FF00 & currentColor)>>8) + a_g;
      n_b = ((0x000000FF & currentColor)) + a_b;
      if (n_r>255) n_r = 255;
      if (n_g>255) n_g = 255;
      if (n_b>255) n_b = 255;
      
      int pixelNum;
      if ((spriteLayer%2)==0) pixelNum = i+spriteLayer*CUBE_X*CUBE_Y;
      else pixelNum = (CUBE_X*CUBE_Y-1-i)+spriteLayer*CUBE_X*CUBE_Y;
      strip.setPixelColor(pixelNum, strip.Color(n_r, n_g, n_b));
    }
  }
 
  
}

class CirclingBox
{
  public:
    CirclingBox();
    void circleStep(int time);
    void applyToString();
    void initialize(int s_intensity, int s_circleSpeed, int s_numSprites);
   private:
    int intensity;
    int circleSpeed;
    int nextSpriteLayer = 0;
    int numSprites;
    BoxSprite sprite[12];
};

CirclingBox::CirclingBox() {
}

void CirclingBox::initialize(int s_intensity, int s_circleSpeed, int s_numSprites) {
  intensity = s_intensity;
  circleSpeed = s_circleSpeed;
  nextSpriteLayer = 0;
  numSprites = s_numSprites;
  int i;
  for (i=0; i<numSprites; i++) {
    sprite[i].initialize(getRandomTwoChannelColor(), ((double)random(1000))/10000.0, ((double)random(100))/100000.0, nextSpriteLayer);
    nextSpriteLayer += 1;
    if (nextSpriteLayer>(CUBE_Z-1)) nextSpriteLayer = 0;
  }
  strip.clear();
  strip.show();
}

void CirclingBox::applyToString() {
  int i;
  for (i=0; i<(CUBE_X*CUBE_Y*CUBE_Z); i++) {
    strip.setPixelColor(i, 0);
  }
  for (i=0; i<numSprites; i++) {
    sprite[i].applyToString();
  }
}

void CirclingBox::circleStep(int time) {
  int i;
  for (i=0; i<numSprites; i++) {
    if (!sprite[i].stepSprite()) {
      sprite[i].initialize(getRandomTwoChannelColor(), ((double)random(1000))/10000.0, ((double)random(100))/100000.0, nextSpriteLayer);
      nextSpriteLayer += 1;
      if (nextSpriteLayer>(CUBE_Z-1)) nextSpriteLayer = 0;
    }
  }
}


