#include "player.h"
#include "gamedata.h"


Player::Player( const std::string& name) :
  WallCollidable(name), // Pass parent constructor the name.
  maxVelocity(Gamedata::getInstance().getXmlInt(name + "/maxSpeed")),
  amtToIncreaseVelocity(Gamedata::getInstance().getXmlInt(name+ "/incSpeed")),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height")),
  x_fov(Gamedata::getInstance().getXmlInt(name + "/xFovStart")),
  y_fov(Gamedata::getInstance().getXmlInt(name + "/yFovStart")),
  theta(Gamedata::getInstance().getXmlInt(name + "/thetaStart")),
  rotation_radius(Gamedata::getInstance().getXmlInt(name + "/rotationRadius")),
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

// Use y_fov and x_fov to determine how diaggonal movement works.
// Vertical goes from x_fov = 1 (top) to x_fov = -1 (bottom).
// Horizontal goes form y_fov = 1 (left) to y_fov = -1 (right).
void Player::right() { 
  getSpriteInfo()->setVelocityX( getSpriteInfo()->getVelocityX() - amtToIncreaseVelocity * -y_fov );
  getSpriteInfo()->setVelocityY( getSpriteInfo()->getVelocityY() - amtToIncreaseVelocity * x_fov );
} 
void Player::left()  { 
  getSpriteInfo()->setVelocityX( getSpriteInfo()->getVelocityX() + amtToIncreaseVelocity * -y_fov );
  getSpriteInfo()->setVelocityY( getSpriteInfo()->getVelocityY() + amtToIncreaseVelocity * x_fov );
} 
void Player::up()    { 
  getSpriteInfo()->setVelocityX( getSpriteInfo()->getVelocityX() + amtToIncreaseVelocity * x_fov );
  getSpriteInfo()->setVelocityY( getSpriteInfo()->getVelocityY() + amtToIncreaseVelocity * y_fov );
} 
void Player::down()  { 
  getSpriteInfo()->setVelocityX( getSpriteInfo()->getVelocityX() - amtToIncreaseVelocity * x_fov );
  getSpriteInfo()->setVelocityY( getSpriteInfo()->getVelocityY() - amtToIncreaseVelocity * y_fov );
}

// When determining the vector, make sure to normalize it.
void Player::rotateLeft() {
    // Wrap the theta around when reaching -1.
    theta -= Gamedata::getInstance().getXmlInt(getSpriteInfo()->getName() + 
             "/thetaIncrement");
    if(theta < 0) {
      theta += 360;
    }
    
    // Check to see if the values are negative, need to preserve the negative if so.
    float tempFovX = x_fov;
    x_fov = x_fov * cos(Gamedata::getInstance().getXmlFloat(getSpriteInfo()->getName() + "/thetaIncrement")/10.0f)
	      - y_fov * sin(Gamedata::getInstance().getXmlFloat(getSpriteInfo()->getName() + "/thetaIncrement")/10.0f); 
    y_fov = tempFovX * sin(Gamedata::getInstance().getXmlFloat(getSpriteInfo()->getName() + "/thetaIncrement")/10.0f)
	      + y_fov * cos(Gamedata::getInstance().getXmlFloat(getSpriteInfo()->getName() + "/thetaIncrement")/10.0f); 
    
    // Check to see if the values are negative, need to preserve the negative if so.
    float tempPlaneX = plane_x;
    plane_x = plane_x * cos(Gamedata::getInstance().getXmlFloat(getSpriteInfo()->getName() + "/thetaIncrement")/10.0f)
	      - plane_y * sin(Gamedata::getInstance().getXmlFloat(getSpriteInfo()->getName() + "/thetaIncrement")/10.0f); 
    plane_y = tempPlaneX * sin(Gamedata::getInstance().getXmlFloat(getSpriteInfo()->getName() + "/thetaIncrement")/10.0f)
	      + plane_y * cos(Gamedata::getInstance().getXmlFloat(getSpriteInfo()->getName() + "/thetaIncrement")/10.0f); 
}
void Player::rotateRight() {
    // Wrap the theta around when reaching 360.
    theta += Gamedata::getInstance().getXmlInt(getSpriteInfo()->getName() + 
             "/thetaIncrement");
    if(theta > 359) {
    	theta -= 360;
    }

    // Check to see if the values are negative, need to preserve the negative if so.
    float tempFovX = x_fov;
    x_fov = x_fov * cos(-Gamedata::getInstance().getXmlFloat(getSpriteInfo()->getName() + "/thetaIncrement")/10.0f)
	      - y_fov * sin(-Gamedata::getInstance().getXmlFloat(getSpriteInfo()->getName() + "/thetaIncrement")/10.0f); 
    y_fov = tempFovX * sin(-Gamedata::getInstance().getXmlFloat(getSpriteInfo()->getName() + "/thetaIncrement")/10.0f)
	      + y_fov * cos(-Gamedata::getInstance().getXmlInt(getSpriteInfo()->getName() + "/thetaIncrement")/10.0f); 
    
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


