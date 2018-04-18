#ifndef ENEMY__H
#define ENEMY__H
#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <cmath>

#include "subjectSprite.h"
#include "smartSprite.h"
#include "wallCollidable.h"
#include "enemyProjectile.h"

class Enemy : public WallCollidable {
public:
  Enemy(const std::string&, const Vector2f& pos);
  Enemy(const Enemy&);
  virtual ~Enemy(){}

  void draw() const;
  void update(Uint32 ticks);

  // Observor setter.
  void setPlayerPos(const Vector2f& p) { playerPos = p; }

  const std::string& getName() const { return getSpriteInfo()->getName(); }
  float getX() const { return getSpriteInfo()->getX(); }
  float getY() const { return getSpriteInfo()->getY(); }

  // Translation options.
  void up();

  // Stop movement of collidableSprite.
  void stop();

  // Attack towards the current direction of the player.
  void attackPlayer() const;
  void shoot();

private:
  enum EnemyAttackState { NORMAL, ATTACK };
  EnemyAttackState current_state;
  
  int maxVelocity;
  int amtToIncreaseVelocity;
  int worldWidth;
  int worldHeight;
  // Observer- player position.
  Vector2f playerPos; 
};
#endif
