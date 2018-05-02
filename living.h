#include <SDL2/SDL.h>
#include <string>

class Living{

  public:
    Living();
    virtual ~Living(){};
    
    bool getAlive();

    void update();
    void dealDamage(int n);
  
  private:
    int health;
    bool alive;

};
