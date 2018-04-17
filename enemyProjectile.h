#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <cmath>

#include "subjectSprite.h"
#include "smartSprite.h"
#include "wallCollidable.h"

class EnemyProjectile {
public:
  EnemyProjectile(const std::string&, const Vector2f& pos);
  EnemyProjectile(const EnemyProjectile&);
  virtual ~EnemyProjectile(){}

  void draw() const { getSpriteInfo()->draw(); }
  void update(Uint32 ticks);

  // Observor setter.
  const std::string& getName() const { return getSpriteInfo()->getName(); }
  float getX() const { return getSpriteInfo()->getX(); }
  float getY() const { return getSpriteInfo()->getY(); }

  // Translation options.
  void up();

  // Stop movement of collidableSprite.
  void stop();

  // Attack towards the current direction of the player.
  void attackPlayer() const;

private:
  
  int maxVelocity;
  int amtToIncreaseVelocity;
  int worldWidth;
  int worldHeight;
  // Rotation values.
  float x_fov, y_fov;
  int theta, rotation_radius; 
  // Observer- player position.
  Vector2f playerPos; 
};
