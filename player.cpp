#include "player.h"
#include "gamedata.h"


Player::Player( const std::string& name) :
  WallCollidable(name), // Pass parent constructor the name.
  maxVelocity(Gamedata::getInstance().getXmlInt(name + "/maxSpeed")),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height")),
  observers(),
  plane_x(0),
  plane_y(.66)
{ }

void Player::stop() {
  // Momentum, slow down speed each tick.
  getSpriteInfo()->setVelocity( 
    Vector2f(
	      Gamedata::getInstance().getXmlFloat(getName() + 
        "/momentumSlowdown") * getSpriteInfo()->getVelocityX(), 
        Gamedata::getInstance().getXmlFloat(getName() + 
        "/momentumSlowdown") * getSpriteInfo()->getVelocityY()
    ) 
  );
}

// When determining the vector, make sure to normalize it.
void Player::rotateLeft() {
    WallCollidable::rotateLeft();
   
    // Check to see if the values are negative, need to preserve the negative if so.
    float tempPlaneX = plane_x;
    plane_x = plane_x * cos(Gamedata::getInstance().getXmlFloat(getSpriteInfo()->getName() + "/thetaIncrement")/10.0f)
	      - plane_y * sin(Gamedata::getInstance().getXmlFloat(getSpriteInfo()->getName() + "/thetaIncrement")/10.0f); 
    plane_y = tempPlaneX * sin(Gamedata::getInstance().getXmlFloat(getSpriteInfo()->getName() + "/thetaIncrement")/10.0f)
	      + plane_y * cos(Gamedata::getInstance().getXmlFloat(getSpriteInfo()->getName() + "/thetaIncrement")/10.0f); 
}
void Player::rotateRight() {   
    WallCollidable::rotateLeft();

    // Update the camera plane.
    float tempPlaneX = plane_x;
    plane_x = plane_x * cos(-Gamedata::getInstance().getXmlFloat(getSpriteInfo()->getName() + "/thetaIncrement")/10.0f)
	      - plane_y * sin(-Gamedata::getInstance().getXmlFloat(getSpriteInfo()->getName() + "/thetaIncrement")/10.0f); 
    plane_y = tempPlaneX * sin(-Gamedata::getInstance().getXmlFloat(getSpriteInfo()->getName() + "/thetaIncrement")/10.0f)
	      + plane_y * cos(-Gamedata::getInstance().getXmlFloat(getSpriteInfo()->getName() + "/thetaIncrement")/10.0f);
}

void Player::detach( Enemy* e ){
  std::list<Enemy*>::iterator itr = observers.begin();
  while( itr != observers.end() )
  {
    if( *itr == e ){
      itr = observers.erase(itr);
      return;
    }
    ++itr;
  }
}

void Player::update(Uint32 ticks) {
  // Bouncing timer when colliding with wall.
  if( getState() == 1 && getBounceTimer() >= static_cast<Uint32>( 10 * Gamedata::getInstance().getXmlInt("period") ) 
      && 
      ( fabs(getSpriteInfo()->getVelocityX() ) +fabs(getSpriteInfo()->getVelocityY()) > Gamedata::getInstance().getXmlInt(getName() + "/bounceSpeedRequirement") ) 
    ) 
  {
    setState(0); 
  }
  else if ( getState() == 1 ){
    setBounceTimer(ticks);
  }
  
  setPreviousY(getSpriteInfo()->getY());
  setPreviousX(getSpriteInfo()->getX());
  getSpriteInfo()->update(ticks);
  stop();

  // Update observers with new x and y coordinates.
  std::list<Enemy*>::iterator itr = observers.begin();
  while( itr != observers.end() )
  {
    (*itr)->setPlayerPos( getSpriteInfo()->getPosition() );
    itr++;
  } 
}


