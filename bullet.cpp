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
  return strategy->execute(*sprite, *this); 
}
