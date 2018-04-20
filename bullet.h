#include <iostream>
#include "sprite.h"
#include "gamedata.h"
#include "collisionStrategy.h"
#include "drawable.h"


class Bullet : public Sprite {
public:
  explicit Bullet(const string& name) :
    Sprite(name), 
    strategy(nullptr),
    distance(0), 
    maxDistance(Gamedata::getInstance().getXmlInt(name+"/distance")), 
    tooFar(false) 
  { strategy = new RectangularCollisionStrategy; }

  Bullet(const Bullet& bul):
    Sprite(bul.getName()), 
    strategy(nullptr),
    distance(0), 
    maxDistance(Gamedata::getInstance().getXmlInt(bul.getName() +"/distance")), 
    tooFar(false)
  {
    strategy = new RectangularCollisionStrategy;
  }
  ~Bullet(){ delete strategy; }

  Bullet& operator=(const Bullet& bul);

  virtual void update(Uint32 ticks);
  bool goneTooFar() const { return tooFar; }
  bool checkCollision(const Drawable*) const;
  void reset() {
    tooFar = false;
    distance = 0;
  }

private:
  CollisionStrategy* strategy;
  float distance;
  float maxDistance;
  bool tooFar;
};
