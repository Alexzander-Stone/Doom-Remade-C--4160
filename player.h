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
  float getXFov() const { return x_fov; }
  float getYFov() const { return y_fov; }

  // Translation options.
  void right();
  void left();
  void up();
  void down();

  // Rotation Options.
  void rotateLeft();
  void rotateRight();

  // Getter for rotation value (theta)
  const int& getRotation() const{
    return theta;
  };

  // Stop movement of collidableSprite.
  void stop();

  // Observer pattern functions.
  void attach( Enemy* e ){ observers.push_back(e); }
  void detach( Enemy* e );

  

private: 
  int maxVelocity;
  int amtToIncreaseVelocity;
  int worldWidth;
  int worldHeight;
  // Rotation values.
  float x_fov, y_fov;
  int theta, rotation_radius; 
  // Observer pattern.
  std::list<Enemy*> observers;
};
#endif
