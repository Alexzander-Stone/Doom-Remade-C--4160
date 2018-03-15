#ifndef PLAYER__H
#define PLAYER__H
#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <cmath>

#include "multisprite.h"

class Player {
public:
  Player(const std::string&);
  Player(const Player&);

  void draw() const { player.draw(); }
  void update(Uint32 ticks);
  const MultiSprite* getPlayer() const { return &player; }

  const std::string& getName() const { return player.getName(); }
  int getX() const { return player.getX(); }
  int getY() const { return player.getY(); }
  const Image* getImage() const { 
    return player.getImage();
  }
  int getScaledWidth()  const { 
    return player.getScaledWidth();
  } 
  int getScaledHeight()  const { 
    return player.getScaledHeight();
  } 
  const SDL_Surface* getSurface() const { 
    return player.getSurface();
  }

  // Translation options.
  void right();
  void left();
  void up();
  void down();

  // Rotation Options.
  void rotateLeft();
  void rotateRight();

  // Get sprite info (pos, velo, etc.)
  const MultiSprite& getSpriteInfo() const{
      return player;
  }

  // Getter for rotation value (theta)
  const int& getRotation() const{
    return theta;
  };

  // Collision reaction.
  void collisionDetected();

  // Stop movement of player.
  void stop();

private:
  enum PlayerState { NORMAL, BOUNCE };
  
  MultiSprite player;
  int maxVelocity;
  int amtToIncreaseVelocity;
  int worldWidth;
  int worldHeight;
  // Rotation values.
  double x_fov, y_fov;
  int theta, rotation_radius; 
  // State machine.
  PlayerState current_state;
  // Bounce Timer.
  Uint32 bounce_timer;
};
#endif
