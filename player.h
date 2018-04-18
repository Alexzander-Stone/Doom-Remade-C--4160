#ifndef PLAYER__H
#define PLAYER__H
#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <cmath>

#include "subjectSprite.h"
#include "smartSprite.h"
#include "wallCollidable.h"
#include "enemy.h"

class Player : public WallCollidable {
public:
  Player(const std::string&);
  Player(const Player&);

  virtual ~Player() {}

  void draw() const { getSpriteInfo()->draw(); }
  void update(Uint32 ticks);

  const std::string& getName() const { return getSpriteInfo()->getName(); }
  float getX() const { return getSpriteInfo()->getX(); }
  float getY() const { return getSpriteInfo()->getY(); }
  float getPlaneX() const { return plane_x; }
  float getPlaneY() const { return plane_y; } 

  // Movement options.
  void stop();
  void rotateLeft();
  void rotateRight();

  // Observer pattern functions.
  void attach( Enemy* e ){ observers.push_back(e); }
  void detach( Enemy* e );

  

private: 
  int maxVelocity;
  int worldWidth;
  int worldHeight;
  // Observer pattern.
  std::list<Enemy*> observers;
  // Camera plane.
  float plane_x, plane_y;
};
#endif
