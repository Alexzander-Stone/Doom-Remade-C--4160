#include <SDL2/SDL.h>
#include <string>

class Living{

  public:
    Living();
    void dealDamage(int n);
    virtual ~Living(){};

  private:
    int health;

};
