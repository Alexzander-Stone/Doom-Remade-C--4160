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
#include "collisionStrategy.h"

Engine::~Engine() { 
  delete player;
  for( Drawable* drawings : sprites )
  {
      delete drawings;
  }
  for( CollisionStrategy* strategy : strategies )
  {
      delete strategy;
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
  player(new Player("SpinningStar")),
  sprites(),
  strategies(),
  currentStrategy(0),
  collision( false ),
  makeVideo( false ),
  currentSprite(1)
{
  // Wall sprites.
  int wallCount = Gamedata::getInstance().getXmlInt("Wall/numberOfWalls");
  sprites.reserve( wallCount );

  Vector2f pos = player->getSpriteInfo().getPosition();
  int w = player->getScaledWidth();
  int h = player->getScaledHeight();
  for ( int i = 0; i < wallCount; i++ ){
      sprites.push_back( new SmartSprite("YellowStar", pos, w, h) );
      //player->getSpriteInfo().attach( sprites[i] );
  }

  // Collision strategies ( rect, pixel, distance(midpoint) ).
  strategies.push_back( new RectangularCollisionStrategy );
  strategies.push_back( new PerPixelCollisionStrategy );
  strategies.push_back( new MidPointCollisionStrategy );

  Viewport::getInstance().setObjectToTrack(player->getPlayer());
  std::cout << "Loading complete" << std::endl;
}

void Engine::draw() const {
  world.draw();

  player->draw();

  viewport.draw();
  SDL_RenderPresent(renderer);
}

// Collision Detection.
void Engine::checkForCollisions(){
    auto it = sprites.begin();

    // Search through all the sprites to determine if collision has occurred.
    while( it != sprites.end() ){
        // Check for collision between player and object.
        if( strategies[currentStrategy]->execute(player->getSpriteInfo(), **it) ){
            // Collision has been detected, bounce off wall.
        }
        ++it;
    }
}

void Engine::update(Uint32 ticks) {
  checkForCollisions();
  player->update(ticks);
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
        static_cast<Player*>(player)->left();
      }
      if (keystate[SDL_SCANCODE_D]) {
        static_cast<Player*>(player)->right();
      }
      if (keystate[SDL_SCANCODE_W]) {
        static_cast<Player*>(player)->up();
      }
      if (keystate[SDL_SCANCODE_S]) {
        static_cast<Player*>(player)->down();
      }
      if(keystate[SDL_SCANCODE_LEFT]){
        static_cast<Player*>(player)->rotateLeft();
      }
      if(keystate[SDL_SCANCODE_RIGHT]){
        static_cast<Player*>(player)->rotateRight();
      }
      
      draw();
      update(ticks);
      if ( makeVideo ) {
        frameGen.makeFrame();
      }
    }
  }
}
