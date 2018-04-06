#include <string>
#include "gamedata.h"
#include "sprite.h"
#include "multisprite.h"

class Hud {
  public:
    Hud();
    
    void draw() const;
    void update(Uint32 ticks);

    void toggleActive() { active = !active; }
    bool getActive() const { return active; }
  
  private:
    Sprite background;
    MultiSprite doom_head;
    float screen_ratio;
    bool active;
};
