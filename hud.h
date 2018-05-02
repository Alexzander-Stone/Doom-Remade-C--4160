#include <string>
#include <chrono>
#include "gamedata.h"
#include "sprite.h"
#include "multisprite.h"
#include "player.h"

class Hud {
  public:
    Hud();
    Hud(Player* p);
    Hud(const Hud&);
    ~Hud();

    Hud& operator=(const Hud&);
    
    void draw() ;
    void update(Uint32 ticks);
    void setPlayer(Player* p);
    std::vector<Sprite*>& getShells() { return shells; }

    void toggleActive() { active = !active; }
    bool getActive() const { return active; }
    void toggleEnding() { ending = !ending; }
    bool getEnding() const { return ending; }
  
  private:
    // Required for drawing.
    SDL_Renderer * const renderer;
    // Time management.
    std::chrono::high_resolution_clock::time_point time;
    double endTime;

    Sprite background;
    MultiSprite doom_head;
    std::vector<Sprite*> shells;
    float screen_ratio;
    bool active;
    bool ending;
    Player* player;
};
