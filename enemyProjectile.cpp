#include "enemyProjectile.h"
#include "gamedata.h"

EnemyProjectile::EnemyProjectile( const std::string& name, const float& xFOV, const float& yFOV) :
  MultiSprite(name),
  amtToIncreaseVelocity(Gamedata::getInstance().getXmlInt(name+ "/incSpeed")),
  x_fov(xFOV),
  y_fov(yFOV)
{ }

// Use y_fov and x_fov to determine how diagonal movement works.
// Vertical goes from x_fov = 1 (top) to x_fov = -1 (bottom).
// Horizontal goes form y_fov = 1 (left) to y_fov = -1 (right).
void EnemyProjectile::up()    { 
  setVelocityX( getVelocityX() - amtToIncreaseVelocity * y_fov );
  setVelocityY( getVelocityY() - amtToIncreaseVelocity * x_fov );
} 

void EnemyProjectile::update(Uint32 ticks) {
  up();  
  update(ticks);
}
