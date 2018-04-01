#ifndef WALLCOLLIDABLE__H
#define WALLCOLLIDABLE__H
#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <cmath>

#include "subjectSprite.h"
#include "smartSprite.h"

class WallCollidable {
public:
  WallCollidable(const std::string&);
  WallCollidable(const WallCollidable&);

  const SubjectSprite* getWallCollidable() const { return &collidableSprite; }

  const std::string& getName() const { return collidableSprite.getName(); }
  float getX() const { return collidableSprite.getX(); }
  float getY() const { return collidableSprite.getY(); }
  const Image* getImage() const { 
    return collidableSprite.getImage();
  }
  int getScaledWidth()  const { 
    return collidableSprite.getScaledWidth();
  } 
  int getScaledHeight()  const { 
    return collidableSprite.getScaledHeight();
  } 
  const SDL_Surface* getSurface() const { 
    return collidableSprite.getSurface();
  }

  // Get sprite info (pos, velo, etc.)
  SubjectSprite& getSpriteInfo() {
      return collidableSprite;
  }

  // Collision reaction.
  void collisionDetected();


  // Momentum direction.
  float getMomentumVelocityX() const;
  float getMomentumVelocityY() const;

private:
  enum WallCollidableState { NORMAL, BOUNCE };
  
  SubjectSprite collidableSprite;
  // State machine.
  WallCollidableState current_state;
  // Bounce Timer.
  Uint32 bounce_timer;
  // Previous coordinates.
  float previous_x; 
  float previous_y;

};
#endif
