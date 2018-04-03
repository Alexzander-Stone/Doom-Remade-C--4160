#ifndef ENEMY__H
#define ENEMY__H
#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <cmath>

#include "subjectSprite.h"
#include "smartSprite.h"
#include "wallCollidable.h"

class Enemy : public WallCollidable {
public:
  Enemy(const std::string&);
  Enemy(const Enemy&);

  void draw() { getSpriteInfo()->draw(); }
  void update(Uint32 ticks);

  const std::string& getName() const { return getSpriteInfo()->getName(); }
  float getX() const { return getSpriteInfo()->getX(); }
  float getY() const { return getSpriteInfo()->getY(); }

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

private:
  
  int maxVelocity;
  int amtToIncreaseVelocity;
  int worldWidth;
  int worldHeight;
  // Rotation values.
  float x_fov, y_fov;
  int theta, rotation_radius; 
};
#endif
