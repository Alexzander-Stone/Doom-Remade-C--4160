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
  theta(Gamedata::getInstance().getXmlInt(name + "/directionStart")),
  rotation_radius(Gamedata::getInstance().getXmlInt(name + "/rotationRadius")),
  observers()
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
  // Add to the current speed of the getSpriteInfo()->
  getSpriteInfo()->setVelocityX( getSpriteInfo()->getVelocityX() + amtToIncreaseVelocity * x_fov );
  getSpriteInfo()->setVelocityY( getSpriteInfo()->getVelocityY() + amtToIncreaseVelocity * y_fov );
} 
void Player::left()  { 
  // Add to the current speed of the getSpriteInfo()->
  getSpriteInfo()->setVelocityX( getSpriteInfo()->getVelocityX() - amtToIncreaseVelocity * x_fov );
  getSpriteInfo()->setVelocityY( getSpriteInfo()->getVelocityY() - amtToIncreaseVelocity * y_fov );
} 
void Player::up()    { 
  // Add to the current speed of the getSpriteInfo()->
  getSpriteInfo()->setVelocityX( getSpriteInfo()->getVelocityX() - amtToIncreaseVelocity * -y_fov );
  getSpriteInfo()->setVelocityY( getSpriteInfo()->getVelocityY() - amtToIncreaseVelocity * x_fov );
} 
void Player::down()  { 
  // Add to the current speed of the getSpriteInfo()->
  getSpriteInfo()->setVelocityX( getSpriteInfo()->getVelocityX() + amtToIncreaseVelocity * -y_fov );
  getSpriteInfo()->setVelocityY( getSpriteInfo()->getVelocityY() + amtToIncreaseVelocity * x_fov );
}

// When determining the vector, make sure to normalize it.
void Player::rotateLeft() {
    // Wrap the theta around when reaching -1.
    theta -= Gamedata::getInstance().getXmlInt(getSpriteInfo()->getName() + 
             "/thetaIncrement");
    if(theta < 0) {
      theta += 360;
    }
    float not_normalized_x= cos(theta * (3.14/180));
    float not_normalized_y = sin(theta * (3.14/180));
    
    // Check to see if the values are negative, need to preserve the negative if so.
    if(not_normalized_x >= 0){
      x_fov = rotation_radius * pow(not_normalized_x, 2);
    }
    else{
      x_fov = -1 * rotation_radius * pow(not_normalized_x, 2);
    }

    if(not_normalized_y >= 0){
      y_fov = rotation_radius * pow(not_normalized_y, 2);
    }
    else{
      y_fov = -1 * rotation_radius * pow(not_normalized_y, 2);
    }
}
void Player::rotateRight() {
    // Wrap the theta around when reaching 360.
    theta += Gamedata::getInstance().getXmlInt(getSpriteInfo()->getName() + 
             "/thetaIncrement");
    if(theta > 359) {
    	theta -= 360;
    }
    float not_normalized_x= cos(theta * (3.14/180));
    float not_normalized_y = sin(theta * (3.14/180));
    

    // Check to see if the values are negative, need to preserve the negative if so.
    if(not_normalized_x >= 0){
      x_fov = rotation_radius * pow(not_normalized_x, 2);
    }
    else{
      x_fov = -1 * rotation_radius * pow(not_normalized_x, 2);
    }

    if(not_normalized_y >= 0){
      y_fov = rotation_radius * pow(not_normalized_y, 2);
    }
    else{
      y_fov = -1 * rotation_radius * pow(not_normalized_y, 2);
    }
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
