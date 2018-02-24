#include "EffectBase.h"



class FireFly
{
  public:
    FireFly();
    void initialize(uint32_t s_color, double s_initialSpeed);
    bool stepFireFly();
    void applyToString();
  private:
    int r, g, b;
    double x, y, z;
    double mx, my, mz;
    double flySpeed;
    int straightFly;
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

void FireFly::applyToString() {
  int i, j;

  double curx, cury, curz;
  uint32_t currentColor;
  for (j=0; j<CUBE_Z; j++) {
    for (i=0; i<CUBE_X*CUBE_Y; i++) {
      currentColor = strip.getPixelColor(i+j*CUBE_X*CUBE_Y);
      cury = (double)(i/CUBE_X);
      if ((i/CUBE_X)%2==0) {
        curx = (double)(i%CUBE_X);
      } else {
        curx = ((double)(CUBE_X-1)) - (double)(i%CUBE_X);
      }
      curz = (double)j;
  
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
        
        int pixelNum;
        if ((j%2)==0) pixelNum = i+j*CUBE_X*CUBE_Y;
        else pixelNum = (CUBE_X*CUBE_Y-1-i)+j*CUBE_X*CUBE_Y;
        strip.setPixelColor(pixelNum, strip.Color(n_r, n_g, n_b));
      }
    }
  }
 
  
}

class FireFlyBox
{
  public:
    FireFlyBox();
    void boxStep(int time);
    void applyToString();
    void initialize(int s_intensity, int s_numFlies);
   private:
    int intensity;
    int numFlies;
    FireFly fly[12];
};

FireFlyBox::FireFlyBox() {
}

void FireFlyBox::initialize(int s_intensity, int s_numFlies) {
  intensity = s_intensity;
  numFlies = s_numFlies;

  int i;
  for (i=0; i<numFlies; i++) {
    fly[i].initialize(strip.Color(random(20,220), random(20,220), random(20,220)), 0.005*numFlies);
  }
  strip.clear();
  strip.show();
}

void FireFlyBox::applyToString() {
  int i;
  for (i=0; i<CUBE_X*CUBE_Y*CUBE_Z; i++) {
    strip.setPixelColor(i, 0);
  }
  for (i=0; i<numFlies; i++) {
    fly[i].applyToString();
  }
}

void FireFlyBox::boxStep(int time) {
  int i;
  for (i=0; i<numFlies; i++) {
    if (!fly[i].stepFireFly()) {
      
    }
  }
}


