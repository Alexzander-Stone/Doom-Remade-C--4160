#include "wallCollidable.h"
#include "gamedata.h"

WallCollidable::WallCollidable( const std::string& name) :
  collidableSprite(name),
  current_state(NORMAL),
  amtToIncreaseVelocity(Gamedata::getInstance().getXmlInt(name+ "/incSpeed")),
  bounce_timer(0),
  previous_x(0),
  previous_y(0),
  x_fov(Gamedata::getInstance().getXmlFloat(name + "/xFovStart")),
  y_fov(Gamedata::getInstance().getXmlFloat(name + "/yFovStart")),
  theta(Gamedata::getInstance().getXmlInt(name + "/directionStart")),
  rotation_radius(Gamedata::getInstance().getXmlInt(name + "/rotationRadius")),
  bulletName( Gamedata::getInstance().getXmlStr(name+"/bullet") ),
  bullets(),
  freeAmmo(),
  bulletInterval( Gamedata::getInstance().getXmlInt(bulletName+"/interval") ),
  bulletSpeed( Gamedata::getInstance().getXmlInt(bulletName+"/speed")),
  timeSinceLastFrame(0)
{ 
  bullets.reserve(Gamedata::getInstance().getXmlInt(bulletName+"/ammo"));
  freeAmmo.reserve(Gamedata::getInstance().getXmlInt(bulletName+"/ammo"));
}

WallCollidable::~WallCollidable(){
  for( Bullet* b : bullets ){
    delete b;
  }
  for( Bullet* b : freeAmmo ){
    delete b;
  }
  bullets.clear();
  freeAmmo.clear();
}

// Needed for shooting projectiles/bullets.
void WallCollidable::draw() const{
  for( const Bullet* bullet : bullets )
    bullet->draw();
}

void WallCollidable::update(Uint32 ticks){
  timeSinceLastFrame += ticks;
  for( Bullet* bullet : bullets )
    bullet->update(ticks);
}

// Check to see if the object is within the left/right and top/bottom range
// of the object. This will determine where to place the object.
// Detach the collision object once the collidableSprite is in a valid state (outside
// the wall).
void WallCollidable::collisionDetected(){
  // Sort the vector of collidars to determine which object is closest to the
  // player. Each object will be solved in ascending order.
  if(collidableSprite.getColliders().size() > 1)
  {
    collidableSprite.sort(collidableSprite.getPosition());
  }
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

// Use y_fov and x_fov to determine how diaggonal movement works.
// Vertical goes from x_fov = 1 (top) to x_fov = -1 (bottom).
// Horizontal goes form y_fov = 1 (left) to y_fov = -1 (right).
void WallCollidable::left() { 
  getSpriteInfo()->setVelocityX( getSpriteInfo()->getVelocityX() - amtToIncreaseVelocity * -y_fov );
  getSpriteInfo()->setVelocityY( getSpriteInfo()->getVelocityY() - amtToIncreaseVelocity * x_fov );
} 
void WallCollidable::right()  { 
  getSpriteInfo()->setVelocityX( getSpriteInfo()->getVelocityX() + amtToIncreaseVelocity * -y_fov );
  getSpriteInfo()->setVelocityY( getSpriteInfo()->getVelocityY() + amtToIncreaseVelocity * x_fov );
} 
void WallCollidable::up()    { 
  getSpriteInfo()->setVelocityX( getSpriteInfo()->getVelocityX() + amtToIncreaseVelocity * x_fov );
  getSpriteInfo()->setVelocityY( getSpriteInfo()->getVelocityY() + amtToIncreaseVelocity * y_fov );
} 
void WallCollidable::down()  { 
  getSpriteInfo()->setVelocityX( getSpriteInfo()->getVelocityX() - amtToIncreaseVelocity * x_fov );
  getSpriteInfo()->setVelocityY( getSpriteInfo()->getVelocityY() - amtToIncreaseVelocity * y_fov );
}

// When determining the vector, make sure to normalize it.
void WallCollidable::rotateRight() {
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
}
void WallCollidable::rotateLeft() {
    // Wrap the theta around when reaching 360.
    theta += Gamedata::getInstance().getXmlInt(getSpriteInfo()->getName() + 
             "/thetaIncrement");
    if(theta > 359) {
    	theta -= 360;
    }
    
    float tempFovX = x_fov;
    x_fov = x_fov * cos(-Gamedata::getInstance().getXmlFloat(getSpriteInfo()->getName() + "/thetaIncrement")/10.0f)
            - y_fov * sin(-Gamedata::getInstance().getXmlFloat(getSpriteInfo()->getName() + "/thetaIncrement")/10.0f);
    y_fov = tempFovX * sin(-Gamedata::getInstance().getXmlFloat(getSpriteInfo()->getName() + "/thetaIncrement")/10.0f)
            + y_fov * cos(-Gamedata::getInstance().getXmlFloat(getSpriteInfo()->getName() + "/thetaIncrement")/10.0f);
}

void WallCollidable::shoot(){
  if( timeSinceLastFrame  < bulletInterval ) return;
  // Create new bullet if freeAmmo is empty.
  if(freeAmmo.empty() && bullets.size() < static_cast<unsigned int>(Gamedata::getInstance().getXmlInt(bulletName+"/ammo")) ){
    Bullet* newBullet = new Bullet(bulletName);
    newBullet->setPosition( getSpriteInfo()->getPosition() + 
                      Vector2f(getSpriteInfo()->getScaledWidth()/2, 
                      getSpriteInfo()->getScaledHeight()/2) );
    newBullet->setVelocity( Vector2f(x_fov * bulletSpeed, y_fov * bulletSpeed)  );
    bullets.push_back( newBullet );
  }
  // Use a bullet available from the freeAmmo vector.
  else if (bullets.size() < static_cast<unsigned int>(Gamedata::getInstance().getXmlInt(bulletName+"/ammo")) ) {
    auto itr = freeAmmo.end()-1;
    (*itr)->setPosition( getSpriteInfo()->getPosition() + 
                      Vector2f(getSpriteInfo()->getScaledWidth()/2, 
                      getSpriteInfo()->getScaledHeight()/2) );
    (*itr)->setVelocity( Vector2f(x_fov * bulletSpeed, y_fov * bulletSpeed)  );
    bullets.push_back( *itr );
    freeAmmo.erase(itr);
  }

  timeSinceLastFrame = 0;
}

void WallCollidable::checkBulletCollision(Drawable* col){
  auto b = bullets.begin();
  while( b != bullets.end() ){
    if((*b)->checkCollision(col)){
      freeAmmo.push_back(*b);
      b = bullets.erase(b);
    }
    else
      b++;
  }
}
