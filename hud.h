#include <string>
#include "gamedata.h"
#include "sprite.h"

class Hud {
  public:
    Hud();
    
    void draw() const;
    void update();
  
  private:
    Sprite background;
};
