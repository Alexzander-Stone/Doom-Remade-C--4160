#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <random>
#include <iomanip>
#include "smartSprite.h"
#include "multisprite.h"
#include "gamedata.h"
#include "engine.h"
#include "frameGenerator.h"
#include "player.h"
#include "wallCollidable.h"
#include "collisionStrategy.h"
#include "vector2f.h"

Engine::~Engine() { 
  for( Drawable* drawings : sprites )
  {
      delete drawings;
  }
  for( CollisionStrategy* strategy : strategies )
  {
      delete strategy;
  }
  for( WallCollidable* collide : collidables )
  {
      delete collide;
  }
  std::cout << "Terminating program" << std::endl;
}

Engine::Engine() :
  rc( RenderContext::getInstance() ),
  io( IoMod::getInstance() ),
  clock( Clock::getInstance() ),
  renderer( rc->getRenderer() ),
  world("back", Gamedata::getInstance().getXmlInt("back/factor") ),
  viewport( Viewport::getInstance() ),
  sprites(),
  strategies(),
  collidables(),
  currentStrategy(0),
  collision( false ),
  makeVideo( false ),
  currentSprite(1)
{
  // Objects that can collide with walls.
  collidables.reserve(10);
  collidables.push_back(new Player("DoomGuy"));

  // Enemies, attach to observor in collidables[0].
  Vector2f placeholderPlayerPos(50, 50);
  collidables.push_back( new Enemy("Pinkie", placeholderPlayerPos) );
  static_cast<Player*>(collidables[0])->attach( static_cast<Enemy*>( *(collidables.end() - 1) ) );

  // Wall sprites.
  int wallCount = Gamedata::getInstance().getXmlInt("Wall/numberOfWalls");
  sprites.reserve( wallCount*2 );

  int w = static_cast<Player*>(collidables[0])->getSpriteInfo()->getScaledWidth();
  int h = static_cast<Player*>(collidables[0])->getSpriteInfo()->getScaledHeight();
  for ( int i = 0; i < wallCount; i++ ){
      Vector2f spritePos(50 + i*w, 100);
      sprites.push_back( new SmartSprite("Wall/Horizontal", placeholderPlayerPos, w, h, spritePos) );
  }
  // Boxed Arena Walls
  Vector2f spritePos(100, 500);
  Vector2f spritePos2(100, 500);
  Vector2f spritePos3(100, 1012);
  Vector2f spritePos4(612, 500);
  sprites.push_back( new SmartSprite("Wall/Horizontal", placeholderPlayerPos, w, h, spritePos) );
  sprites.push_back( new SmartSprite("Wall/Vertical", placeholderPlayerPos, w, h, spritePos2) );
  sprites.push_back( new SmartSprite("Wall/Horizontal", placeholderPlayerPos, w, h, spritePos3) );
  sprites.push_back( new SmartSprite("Wall/Vertical", placeholderPlayerPos, w, h, spritePos4) );

  // Collision strategies ( rect, pixel, distance(midpoint) ).
  strategies.push_back( new RectangularCollisionStrategy );
  strategies.push_back( new PerPixelCollisionStrategy );
  strategies.push_back( new MidPointCollisionStrategy );

  Viewport::getInstance().setObjectToTrack(collidables[0]->getSpriteInfo());
  std::cout << "Loading complete" << std::endl;
}

void Engine::draw() const {
  world.draw();

  // Draw all sprites in container.
  for( auto& it : sprites )
  {
    it->draw();
  }
  for( auto& it : collidables )
  {
    it->draw();
  }
  viewport.draw();

  // Screen height and width for drawing.
  int height = Gamedata::getInstance().getXmlInt("view/height");
  int width = Gamedata::getInstance().getXmlInt("view/width");
  // Draw FPS.
  std::stringstream fpsStream;
  fpsStream << clock.getFps();
  string fpsCounter = "FPS: " + fpsStream.str();
  IoMod::getInstance().
      writeText(fpsCounter, width - 100, 0);
  // Draw name.
  IoMod::getInstance().
      writeText("Alexzander Stone", 0, height - Gamedata::getInstance().getXmlInt("font/size"));

  SDL_RenderPresent(renderer);
}

// Collision Detection.
void Engine::checkForCollisions(){
    std::vector<WallCollidable*>::iterator currItr = collidables.begin();
    while( currItr != collidables.end() ){
      bool collisionDetected = true;

      // Check until all collisions have been removed.
      while( collisionDetected == true ) {
	  // Search through all the sprites to determine if collision has occurred.
	  auto spriteIt = sprites.begin();
	      collisionDetected = false;
	      int currentSprite = 0;
	  while( spriteIt != sprites.end() ){
	      // Check for collision between collidables[0] and object.
	      if( strategies[currentStrategy]->execute( *(*currItr)->getSpriteInfo(), **spriteIt) ){
		(*currItr)->getSpriteInfo()->attach( sprites[currentSprite] );
		collisionDetected = true;
	      }
	      ++spriteIt;
	      ++currentSprite;
	  }
/* NOT IMPLEMENTED YET
	  auto colIt = collidables.begin();
	  while( colIt != collidables.end() ){
	      // Check for collision between collidables[0] and object.
	      if( (*colIt)->getSpriteInfo() == (*currItr)->getSpriteInfo() )
		; // Ignore self image.
	      else if( strategies[currentStrategy]->execute( *(*currItr)->getSpriteInfo(), *(*colIt)->getSpriteInfo() ) ){
		(*currItr)->getSpriteInfo()->attach( sprites[currentSprite] );
		collisionDetected = true;
	      }
	      ++colIt;
	      ++currentSprite;
	  }
*/
	  if( collisionDetected == true){
	     (*currItr)->collisionDetected();
	  }
      }
      currItr++;
    }
}

void Engine::update(Uint32 ticks) {
  for(auto& it : collidables)
  {
    it->update(ticks);
  }
  checkForCollisions();
  world.update();
  viewport.update(); // always update viewport last
}

void Engine::play() {
  SDL_Event event;
  const Uint8* keystate;
  bool done = false;
  Uint32 ticks = clock.getElapsedTicks();
  FrameGenerator frameGen;

  while ( !done ) {
    // The next loop polls for events, guarding against key bounce:
    while ( SDL_PollEvent(&event) ) {
      keystate = SDL_GetKeyboardState(NULL);
      if (event.type ==  SDL_QUIT) { done = true; break; }
      if(event.type == SDL_KEYDOWN) {
        if (keystate[SDL_SCANCODE_ESCAPE]) {
          done = true;
          break;
        }
        if ( keystate[SDL_SCANCODE_P] ) {
          if ( clock.isPaused() ) clock.unpause();
          else clock.pause();
        }
        if (keystate[SDL_SCANCODE_F4] && !makeVideo) {
          std::cout << "Initiating frame capture" << std::endl;
          makeVideo = true;
        }
        else if (keystate[SDL_SCANCODE_F4] && makeVideo) {
          std::cout << "Terminating frame capture" << std::endl;
          makeVideo = false;
        }
      }
    }

    // In this section of the event loop we allow key bounce:

    ticks = clock.getElapsedTicks();
    if ( ticks > 0 ) {
      clock.incrFrame();
      if (keystate[SDL_SCANCODE_A]) {
        static_cast<Player*>(collidables[0])->left();
      }
      if (keystate[SDL_SCANCODE_D]) {
        static_cast<Player*>(collidables[0])->right();
      }
      if (keystate[SDL_SCANCODE_W]) {
        static_cast<Player*>(collidables[0])->up();
      }
      if (keystate[SDL_SCANCODE_S]) {
        static_cast<Player*>(collidables[0])->down();
      }
      if(keystate[SDL_SCANCODE_LEFT]){
        static_cast<Player*>(collidables[0])->rotateLeft();
      }
      if(keystate[SDL_SCANCODE_RIGHT]){
        static_cast<Player*>(collidables[0])->rotateRight();
      }
      
      update(ticks);
      draw();
      if ( makeVideo ) {
        frameGen.makeFrame();
      }
    }
  }
}
