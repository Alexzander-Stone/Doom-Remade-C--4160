#include "wallCollidable.h"
#include "gamedata.h"


WallCollidable::WallCollidable( const std::string& name) :
  collidableSprite(name),
  current_state(NORMAL),
  bounce_timer(0),
  previous_x(0),
  previous_y(0)
{ }

// Check to see if the object is within the left/right and top/bottom range
// of the object. This will determine where to place the object.
// Detach the collision object once the collidableSprite is in a valid state (outside
// the wall).
void WallCollidable::collisionDetected(){
  // Sort the vector of collidars to determine which object is closest to the
  // player. Each object will be solved in ascending order.
  collidableSprite.sort();

  // Keep track of which wall the collidableSprite encountered.
  bool xFinished = false;			
  float momentumX = getMomentumVelocityX();
  float momentumY = getMomentumVelocityY();
  auto it = collidableSprite.getColliders().begin();
  while( it != collidableSprite.getColliders().end() ) { 
    float collision_obj_x = (*it)->getX();
    float collision_obj_y = (*it)->getY();
    float currentX = collidableSprite.getX();
    float currentY = collidableSprite.getY();
    float currentIncrement = 0;
   
    // Momentum direction determines which direction to move the collidableSprite in response 
    // to the collision. Essentially, the projection matrix. 
    if( (*it)->getName() == "Wall/Horizontal") {
      while ( currentY + currentIncrement <= collision_obj_y + (*it)->getScaledHeight() + 1.001 &&
		          currentY + currentIncrement + getScaledHeight() >= collision_obj_y - 1.001 ) {
	      currentIncrement += -momentumY;
      }
      currentY += currentIncrement;
      collidableSprite.setY( currentY );
    }
    else if( (*it)->getName() == "Wall/Vertical" ) {
      while ( currentX + currentIncrement <= collision_obj_x + (*it)->getScaledWidth() + 1.001 && 
	            currentX + getScaledWidth() + currentIncrement >= collision_obj_x - 1.001 ) {
	      currentIncrement += -momentumX ;
      }
      currentX += currentIncrement;
      collidableSprite.setX( currentX );
    }
    if( currentX <= collision_obj_x + (*it)->getScaledWidth() + 1.00001 && 
	      currentX + getScaledWidth() >= collision_obj_x - 1.00001 )
      xFinished = false;
    else
      xFinished = true;

    it++;			
  }
  collidableSprite.getColliders().erase( collidableSprite.getColliders().begin(), collidableSprite.getColliders().end() );

  // Use the direction that the momentum is traveling towards.
  // Bounce collidableSprite back towards opposite direction.
  // Change state of collidableSprite to BOUNCE mode.
  if ( current_state == NORMAL ){
    current_state = BOUNCE;
    bounce_timer = 0;  
    float bounceVelX = collidableSprite.getVelocityX() * Gamedata::getInstance().getXmlFloat(collidableSprite.getName() + "/Bounce/changeVel");
    float bounceVelY = collidableSprite.getVelocityY() * Gamedata::getInstance().getXmlFloat(collidableSprite.getName() + "/Bounce/changeVel");
			
    // WallCollidable has hit a horizontal wall. Otherwise, vertical wall.
    if( xFinished == false) {	    
      bounceVelY *= -1;
    }
    else {
      bounceVelX *= -1;
    }
    collidableSprite.setVelocityX( bounceVelX );   
    collidableSprite.setVelocityY( bounceVelY );
  }   
  // Still decrement speed whenever running into wall.
  else{
    float slowVelX = collidableSprite.getVelocityX() * Gamedata::getInstance().getXmlFloat(collidableSprite.getName() + "/Bounce/changeVel");
    float slowVelY = collidableSprite.getVelocityY() * Gamedata::getInstance().getXmlFloat(collidableSprite.getName() + "/Bounce/changeVel");
			
    collidableSprite.setVelocityX( slowVelX );   
    collidableSprite.setVelocityY( slowVelY );
  }
}

// Determine the x value of the collidableSprite's momentum.
float WallCollidable::getMomentumVelocityX() const {
  float deltaX = collidableSprite.getX() - previous_x;
  float absoluteDeltaX = fabs( collidableSprite.getX()  - previous_x );
  float absoluteDeltaY = fabs(  collidableSprite.getY() - previous_y );
  float result = ( deltaX ) / (absoluteDeltaX + absoluteDeltaY);

  return result;
}

// Determine the y value of the collidableSprite's momentum.
float WallCollidable::getMomentumVelocityY() const {
  float deltaY = collidableSprite.getY()  - previous_y;
  float absoluteDeltaX = fabs( collidableSprite.getX() - previous_x );
  float absoluteDeltaY = fabs( collidableSprite.getY() - previous_y );
  float result = ( deltaY ) / (absoluteDeltaX + absoluteDeltaY);

  return result;
}
