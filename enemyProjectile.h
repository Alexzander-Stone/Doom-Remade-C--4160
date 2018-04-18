#include <SDL2/SDL.h>
#include <string>

#include "multisprite.h"

class EnemyProjectile : public MultiSprite{
public:
  EnemyProjectile(const std::string&, const float&, const float&);
  EnemyProjectile(const EnemyProjectile&);
  virtual ~EnemyProjectile(){}

  void update(Uint32 ticks);

  // Translation options.
  void up();

private:
  
  int amtToIncreaseVelocity;
  float x_fov, y_fov;
};
