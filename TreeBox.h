#include "EffectBase.h"


class TreeBox
{
  public:
    TreeBox();
    void initialize(int s_intensity);
    void stepTree();
    void applyTree();
  private:
    int intensity;
    int season;
    double trunkHeight;
    double oldTrunkHeight;
    double canopyWidth;
    double oldCanopyWidth;
    double timeInSeason;          
};

TreeBox::TreeBox() {
}

void TreeBox::initialize(int s_intensity) {
  intensity = s_intensity;
  stripOfLEDs.clear();
  stripOfLEDs.show();
  season = 0;
  oldTrunkHeight = 0.0;
  trunkHeight = 0.0;
  oldCanopyWidth = 0.0;
  canopyWidth = 0.0;
  timeInSeason = 0.0;
}


void TreeBox::stepTree() {
  
  if (season==0) {
    trunkHeight = oldTrunkHeight + timeInSeason;
  } else if (season==1) {
  } else if (season==2) {
  } else {
  }
  
  
  timeInSeason = timeInSeason + 0.001;
  if (timeInSeason>1.0) {
    season = season + 1;
    if (season>3) season = 0;
  }
}


void TreeBox::applyTree() {
  int i;
  for (i=0; i<64; i++) {
    stripOfLEDs.setPixelColor(i, 0);
  }
  
}


