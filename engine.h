#include <vector>
#include <SDL2/SDL.h>
#include "ioMod.h"
#include "renderContext.h"
#include "clock.h"
#include "world.h"
#include "viewport.h"
#include "enemy.h"
#include "hud.h"
#include "sound.h"
#include "healthBar.h"

class Player;
class CollisionStrategy;
class SmartSprite;
class SubjectSprite;

class Engine {
public:
  Engine ();
  ~Engine ();
  bool play();

private:
  const RenderContext* rc;
  const IoMod& io;
  Clock& clock;

  SDL_Renderer * const renderer;
  World world;
  Viewport& viewport;

  // Collision variables.
  std::vector<SmartSprite*> sprites;
  std::vector<CollisionStrategy*> strategies;
  // Enemy list
  std::vector<WallCollidable*> collidables;
  // Player reference
  Player* player;

  // Collisions
  int currentStrategy;
  bool collision;
  bool makeVideo;
  int currentSprite;
  Hud hud;

  // Sound.
  SDLSound sound;
  // Health
  HealthBar* enemyHealth;
  HealthBar* playerHealth;

  // Floor sprite.
  Sprite* floor;
  Uint32* floorPixels;
  const SDL_Surface* floorSurface;
  // Enemy sprites.
  int* depthBuffer;
  // Pixels to be drawn, save to buffer (faster than per pixel methods)
  SDL_Texture* texture_buffer;
  Uint32* pixels_to_draw;
  void draw();
  void update(Uint32);

  Engine(const Engine&);
  Engine& operator=(const Engine&);
  void printScales() const;

  void checkForCollisions();
};
