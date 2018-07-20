#include <iostream>
#include <cmath>
#include "bullet.h"
#include "collisionStrategy.h"

Bullet& Bullet::operator=(const Bullet& bul){
  if(this == &bul) return *this;
  delete strategy;
  strategy = new RectangularCollisionStrategy;
  return *this;
}

void Bullet::update(Uint32 ticks) { 
  Sprite::update(ticks);
}

bool Bullet::checkCollision( const Drawable* sprite) const{
    // Enemy collision, large sprite.
  if(sprite->getName() == "Pinkie"){
    Sprite bulletCollision("BulletCollision");
    bulletCollision.setPosition(getPosition() + Vector2f(getScaledWidth()/2 - bulletCollision.getScaledWidth()/2, getScaledHeight()/2 - bulletCollision.getScaledHeight()/2));

    Sprite objectToHit("BulletCollision");
    objectToHit.setPosition(sprite->getPosition());
    return strategy->execute(objectToHit, bulletCollision); 
  }
  else {  // Walls and player.
    Sprite bulletCollision("BulletCollision");
    bulletCollision.setPosition(getPosition() + Vector2f(getScaledWidth()/2 - bulletCollision.getScaledWidth()/2, getScaledHeight()/2 - bulletCollision.getScaledHeight()/2));
    return strategy->execute(*sprite, bulletCollision); 
  }
}
