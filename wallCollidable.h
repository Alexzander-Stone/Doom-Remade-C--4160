#ifndef WALLCOLLIDABLE__H
#define WALLCOLLIDABLE__H
#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <cmath>

#include "subjectSprite.h"
#include "smartSprite.h"
#include "bullet.h"


class WallCollidable {
public:
  WallCollidable(const std::string&);
  WallCollidable(const WallCollidable&);
  virtual ~WallCollidable();

  virtual void draw() const;
  virtual void update(Uint32);
  virtual void shoot();

  const Image* getImage() const { return collidableSprite.getImage(); }
  int getScaledWidth()  const { return collidableSprite.getScaledWidth(); } 
  int getScaledHeight()  const { return collidableSprite.getScaledHeight(); } 
  const SDL_Surface* getSurface() const { return collidableSprite.getSurface(); }

  SubjectSprite* getSpriteInfo() { return &collidableSprite; }
  const SubjectSprite* getSpriteInfo() const { return &collidableSprite; }

  Uint32 getBounceTimer() { return bounce_timer; }
  void setBounceTimer(Uint32 incTime) { bounce_timer += incTime; }

  float getPreviousX() const{ return previous_x; }
  float getPreviousY() const{ return previous_y; }
  void setPreviousX(float x){ previous_x = x; }
  void setPreviousY(float y){ previous_y = y; }

  void collisionDetected();

  float getMomentumVelocityX() const;
  float getMomentumVelocityY() const;
  float getIncrementalVel() const { return amtToIncreaseVelocity; }
  void setIncrementalVel(float inc) { amtToIncreaseVelocity = inc; }

  float getXFov() const{ return x_fov; }
  float getYFov() const{ return y_fov; }

  float getRotation() const{ return theta; }
  void setRotation(int t) { theta = t; }

  void checkBulletCollision(WallCollidable*);

  // Movement options.
  virtual void right();
  virtual void left();
  virtual void up();
  virtual void down();

  virtual void rotateLeft();
  virtual void rotateRight();
  
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

private:
  enum WallCollidableState { NORMAL, BOUNCE };
  
  SubjectSprite collidableSprite;
  // State machine.
  WallCollidableState current_state;
  // Movement.
  int amtToIncreaseVelocity; 
  // Bounce Timer.
  Uint32 bounce_timer;
  // Previous coordinates.
  float previous_x; 
  float previous_y;
  // Direction vector.
  float x_fov, y_fov;
  int theta, rotation_radius;
  // Shooting
  std::string bulletName;
  std::vector<Bullet*> bullets;
  std::vector<Bullet*> freeAmmo;
  int bulletInterval;
  int bulletSpeed;
  int timeSinceLastFrame;
};
#endif
