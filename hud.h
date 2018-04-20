#include <string>
#include "gamedata.h"
#include "sprite.h"
#include "multisprite.h"
#include "player.h"

class Hud {
  public:
    Hud();
    Hud(Player* p);
    Hud(const Hud&);
    ~Hud(){}

    Hud& operator=(const Hud&);
    
    void draw() const;
    void update(Uint32 ticks);
    void setPlayer(Player* p);
    std::vector<Sprite*>& getShells() { return shells; }

    void toggleActive() { active = !active; }
    bool getActive() const { return active; }
  
  private:
    Sprite background;
    MultiSprite doom_head;
    std::vector<Sprite*> shells;
    float screen_ratio;
    bool active;
    Player* player;
};
