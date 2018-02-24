#include "EffectBase.h"

#define MAX_FLIES 8

class FireFly
{
  public:
    FireFly();
    void initialize(uint32_t s_color, double s_initialSpeed);
    bool stepFireFly();
    void applyToStrip(Adafruit_NeoPixel &strip, EffectBase *baseClass);
  private:
    byte r, g, b;
    float x, y, z;
    float mx, my, mz;
    float flySpeed;
    bool straightFly;
};

FireFly::FireFly() {
}

void FireFly::initialize(uint32_t s_color, double s_initialSpeed) {
  r = (0x00FF0000 & s_color)>>16;
  g = (0x0000FF00 & s_color)>>8;
  b = (0x000000FF & s_color);

  flySpeed = s_initialSpeed;
  x = (double)random(4);
  y = (double)random(4);
  z = (double)random(4);
  
  mx = ((double)random(1,10))*flySpeed;
  my = ((double)random(1,10))*flySpeed;
  mz = ((double)random(1,10))*flySpeed;
  straightFly = random(2);
  if (straightFly) {
    int flyDirection = random(3);
    if (flyDirection==0) {
      my = 0.0;
      mz = 0.0;
    } else if (flyDirection==1) {
      mx = 0.0;
      mz = 0.0;
    } else if (flyDirection==2) {
      mx = 0.0;
      my = 0.0;
    }
  }
}

bool FireFly::stepFireFly() {
  x = x+mx;
  y = y+my;
  z = z+mz;
  int wallHit = 0;
  double xdir=1.0, ydir=1.0, zdir=1.0;
  
  if (x>((double)(CUBE_X-1))) {
    mx = -1.0 * ((double)random(1,10))*flySpeed;
    wallHit = 1;
    xdir = -1.0;
    x = 3.0;
  } else if (x<0.0) {
    mx = ((double)random(1,10))*flySpeed;
    wallHit = 1;
    x = 0.0;
  }
  
  if (y>((double)(CUBE_Y-1))) {
    my = -1.0 * ((double)random(1,10))*flySpeed;
    wallHit = 1;
    ydir = -1.0;
    y = 3.0;
  } else if (y<0.0) {
    my = ((double)random(1,10))*flySpeed;
    wallHit = 1;
    y = 0.0;
  }
  
  if (z>((double)(CUBE_Z-1))) {
    mz = -1.0 * ((double)random(1,10))*flySpeed;
    wallHit = 1;
    zdir = -1.0;
    z = 3.0;
  } else if (z<0.0) {
    mz = ((double)random(1,10))*flySpeed;
    wallHit = 1;
    z = 0.0;
  }

  if (straightFly && (random(500)==1)) {
    wallHit = 1;
  }

  if (wallHit && straightFly) {
    int flyDirection = random(3);
    if (flyDirection==0) {
      mx = xdir * ((double)random(1,10))*flySpeed;
      my = 0.0;
      mz = 0.0;
    } else if (flyDirection==1) {
      mx = 0.0;
      my = ydir * ((double)random(1,10))*flySpeed;
      mz = 0.0;
    } else if (flyDirection==2) {
      mx = 0.0;
      my = 0.0;
      mz = zdir * ((double)random(1,10))*flySpeed;
    }
  }
  
  return true;
}

void FireFly::applyToStrip(Adafruit_NeoPixel &strip, EffectBase *baseClass) {
  int i, j, k;

  double curx, cury, curz;
  uint32_t currentColor;
  for (k=0; k<CUBE_Z; k++) {
    for (j=0; j<CUBE_Y; j++) {
      for (i=0; i<CUBE_X; i++) {
        int stripIndex = baseClass->xyzToStrip(i, j, k);
//        int stripIndex = i + j*CUBE_X + k*CUBE_X*CUBE_Y;
        currentColor = strip.getPixelColor(stripIndex);
        curx = (double)i;
        cury = (double)j;
        curz = (double)k;
    
        // brightness will be scaled by distance
        int addBrightness;
        int n_r, n_g, n_b;
        int a_r, a_g, a_b;
        double distance = sqrt((curx-x)*(curx-x)+(cury-y)*(cury-y)+(curz-z)*(curz-z));
        if (distance<1.0) {     
          addBrightness = (int)(100.0 * (1.0-distance)*(1.0-distance));
          a_r = r * addBrightness / 100;
          a_g = g * addBrightness / 100;
          a_b = b * addBrightness / 100;
          n_r = ((0x00FF0000 & currentColor)>>16) + a_r;
          n_g = ((0x0000FF00 & currentColor)>>8) + a_g;
          n_b = ((0x000000FF & currentColor)) + a_b;
          if (n_r>255) n_r = 255;
          if (n_g>255) n_g = 255;
          if (n_b>255) n_b = 255;
            
          strip.setPixelColor(stripIndex, strip.Color(n_r, n_g, n_b));
        }
      }
    }
  }
 
  
}

class FireFlyBox : public EffectBase
{
  public:
    FireFlyBox();
    void effectStep(double timeStep);
    void applyToStrip(Adafruit_NeoPixel &strip);
    void initialize(int s_brightness, int s_numElements, int s_controlParameter);
    void adjustBrightness(int newBrightness);
   private:
    int numFlies;
    FireFly fly[MAX_FLIES];
};

FireFlyBox::FireFlyBox() {
}

void FireFlyBox::adjustBrightness(int newBrightness) {
  if (newBrightness!=brightness) {
    brightness = newBrightness;  
    numFlies = (brightness*MAX_FLIES)/DIAL_MAX;
    for (int i=0; i<numFlies; i++) {
      fly[i].initialize(GenerateNearlyWhite(), 0.005*numFlies);
    }
  }
}

void FireFlyBox::initialize(int s_brightness, int s_numElements, int s_controlParameter) {
  brightness = s_brightness;  
  numFlies = (brightness*MAX_FLIES)/DIAL_MAX;
  numElements = s_numElements;
  controlParameter = s_controlParameter;
  
  if (numFlies>MAX_FLIES) numFlies = MAX_FLIES;

  int i;
  for (i=0; i<numFlies; i++) {
    fly[i].initialize(GenerateNearlyWhite(), 0.005*numFlies);
  }
}

void FireFlyBox::applyToStrip(Adafruit_NeoPixel &strip) {
  int i;
  for (i=0; i<CUBE_X*CUBE_Y*CUBE_Z; i++) {
    strip.setPixelColor(i, 0);
  }
  for (i=0; i<numFlies; i++) {
    fly[i].applyToStrip(strip, this);
  }
}

void FireFlyBox::effectStep(double timeStep) {
  int i;
  for (i=0; i<numFlies; i++) {
    if (!fly[i].stepFireFly()) {
      
    }
  }
}


