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
  virtual ~WallCollidable(){}

  virtual void draw() const = 0;
  virtual void update(Uint32 ticks) = 0;

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
  SubjectSprite* getSpriteInfo() {
      return &collidableSprite;
  }
  // Get sprite info (pos, velo, etc.)
  const SubjectSprite* getSpriteInfo() const {
      return &collidableSprite;
  }

  // Normal = 0, Bounce = 1
  int getState() {
    switch(current_state){
      case NORMAL: return 0;
	           break;
      case BOUNCE: return 1;
		   break;
      default: std::cout << "Error in state change" << std::endl;
	       return -1;
	       break;
    }
  }
  void setState(int newState) {
    switch(newState){
      case 0: current_state = NORMAL;
	      break;
      case 1: current_state = BOUNCE;
	      break;
      default: std::cout << "Error in state change" << std::endl;
	       break;
    }
  }

  Uint32 getBounceTimer() {
    return bounce_timer;
  }
  void setBounceTimer(Uint32 incTime) {
    bounce_timer += incTime;
  }

  float getPreviousX(){
    return previous_x;
  }
  float getPreviousY(){
    return previous_y;
  }
  void setPreviousX(float x){
    previous_x = x;
  }
  void setPreviousY(float y){
    previous_y = y;
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
