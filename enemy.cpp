#include "enemy.h"
#include "gamedata.h"

Enemy::Enemy( const std::string& name, const Vector2f& pos) :
  WallCollidable(name), // Pass parent constructor the name.
  current_state(NORMAL),
  maxVelocity(Gamedata::getInstance().getXmlInt(name + "/maxSpeed")),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height")),
  playerPos(pos)
{ }

void Enemy::draw() const{
  WallCollidable::draw();
  getSpriteInfo()->draw();
}

// Slow down velocity each tick.
void Enemy::stop() {
  getSpriteInfo()->setVelocity( 
    Vector2f(
	Gamedata::getInstance().getXmlFloat(getName() + 
        "/momentumSlowdown") * getSpriteInfo()->getVelocityX(), 
        Gamedata::getInstance().getXmlFloat(getName() + 
        "/momentumSlowdown") * getSpriteInfo()->getVelocityY()
    ) 
  );
}

void Enemy::update(Uint32 ticks) {
  // Shooting.
  WallCollidable::update(ticks);

  // Bouncing timer when colliding with wall.
  // Can bounce when traveling above the bounce speed.
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

  // Use player's position/angle to determine which way to rotate the enemy.
  // Tie this to tick rate in future.
  float deltaX = playerPos[0] - getSpriteInfo()->getX() - getXFov();
  float deltaY = playerPos[1] - getSpriteInfo()->getY() - getYFov();
  float angleBetweenVectors = atan2(deltaY, deltaX) * 57.2958 + 180;
  
  deltaX = getPreviousX() - getSpriteInfo()->getX() - getXFov();
  deltaY = getPreviousY() - getSpriteInfo()->getY() - getYFov();
  float tempTheta = atan2(deltaY, deltaX) * 57.2958 + 180;
  
  float leftRotCheck = angleBetweenVectors - tempTheta;
  float rightRotCheck = tempTheta - angleBetweenVectors;
  if(leftRotCheck < 0)
    leftRotCheck += 360; 
  if(rightRotCheck < 0)
    rightRotCheck += 360;
  if(rightRotCheck - 180 < 5 && rightRotCheck - 180 > -5 && leftRotCheck - 180 < 5 && leftRotCheck - 180 > -5)
    ;
  else if(rightRotCheck > leftRotCheck){
    rotateLeft();
  }
  else if(rightRotCheck < leftRotCheck){
    rotateRight();
  }

  // Shoot projectile at player if they are within range.
  if(current_state == NORMAL && hypot(playerPos[0]-getSpriteInfo()->getX(), playerPos[1] - getSpriteInfo()->getY()) < Gamedata::getInstance().getXmlInt(getName()+"/attackDistance"))
  {
    current_state = ATTACK;
    setIncrementalVel(0);
  }
  // Continue shooting at player until they exit range.
  else if(current_state == ATTACK && hypot(playerPos[0]-getSpriteInfo()->getX(), playerPos[1] - getSpriteInfo()->getY()) < Gamedata::getInstance().getXmlInt(getName()+"/attackDistance")) {
    shoot();  
  }
  // Player has left the range of the enemy, begin walking towards the player
  // again.
  else if(current_state == ATTACK) {
    current_state = NORMAL;
    setIncrementalVel( Gamedata::getInstance().getXmlInt(getName() + "/incSpeed") );
  }
  WallCollidable::up();  

  setPreviousY(getSpriteInfo()->getY());
  setPreviousX(getSpriteInfo()->getX());
  getSpriteInfo()->update(ticks);

  stop();
}

