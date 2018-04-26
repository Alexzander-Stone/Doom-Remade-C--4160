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
  player(NULL),
  currentStrategy(0),
  collision( false ),
  makeVideo( false ),
  currentSprite(1),
  hud()
{
  // Objects that can collide with walls.
  collidables.reserve(10);
  
  // Player object.
  player = new Player("DoomGuy");
  collidables.push_back(player);
  // Hud reference for ammo count.
  hud.setPlayer(player);

  // Enemies, attach to observor in collidables[0].
  Vector2f placeholderPlayerPos(20, 20);
  collidables.push_back( new Enemy("Pinkie", placeholderPlayerPos) );
  player->attach( static_cast<Enemy*>( *(collidables.end() - 1) ) );

  // Wall sprites.
  int wallCount = Gamedata::getInstance().getXmlInt("Wall/numberOfWalls");
  sprites.reserve( wallCount*2 );
  int w = player->getSpriteInfo()->getScaledWidth();
  int h = player->getSpriteInfo()->getScaledHeight();

  // Boxed Arena Walls
  for(int i = 0; i < Gamedata::getInstance().getXmlInt("arena/size"); i++) {
    Vector2f spritePos(Gamedata::getInstance().getXmlInt("Wall/Horizontal/x")*i, 0*i);
    sprites.push_back( new SmartSprite("Wall/Horizontal", placeholderPlayerPos, w, h, spritePos) );
    spritePos = Vector2f( Gamedata::getInstance().getXmlInt("Wall/Horizontal/x")*i, 
                          Gamedata::getInstance().getXmlInt("Wall/Horizontal/x")*Gamedata::getInstance().getXmlInt("arena/size"));
    sprites.push_back( new SmartSprite("Wall/Horizontal", placeholderPlayerPos, w, h, spritePos) );
    spritePos = Vector2f(0*i, Gamedata::getInstance().getXmlInt("Wall/Vertical/y")*i);
    sprites.push_back( new SmartSprite("Wall/Vertical", placeholderPlayerPos, w, h, spritePos) );
    spritePos = Vector2f( Gamedata::getInstance().getXmlInt("Wall/Vertical/x")*Gamedata::getInstance().getXmlInt("arena/size"), 
                          i*Gamedata::getInstance().getXmlInt("Wall/Vertical/y"));
    sprites.push_back( new SmartSprite("Wall/Vertical", placeholderPlayerPos, w, h, spritePos) );
  }

  // Collision strategies ( rect, pixel, distance(midpoint) ).
  strategies.push_back( new RectangularCollisionStrategy );
  strategies.push_back( new PerPixelCollisionStrategy );
  strategies.push_back( new MidPointCollisionStrategy );

  Viewport::getInstance().setObjectToTrack(player->getSpriteInfo());
  std::cout << "Loading complete" << std::endl;
}

void Engine::draw() const {
  // Clear render.
  SDL_RenderClear(renderer);
  // Draw background.
  world.draw(); 

  // Loop through all the vertical stripes of the collidables[0]'s view (x's) based on 
  // the screen width/height. This will calculate the rays using a grid system.
  float planeX = player->getPlaneX(); 
  float planeY = player->getPlaneY();
  // These variables are used for scaling
  int side = 0;
  int drawTop = 0;
  int drawBottom = 0;
  // Boolean to optimize performance for rays hitting the same sprite. Saves
  // cycles by only updating surface if new sprite has been hit.
  std::vector<SmartSprite*>::const_iterator spriteCollided ;
  bool newSpriteCollided = true;

  Uint32* textPixels; 
  const SDL_Surface* textSurface; 

  int viewWidth = Gamedata::getInstance().getXmlInt("view/width");
  int viewHeight = Gamedata::getInstance().getXmlInt("view/height");
  int raycastWidth = Gamedata::getInstance().getXmlInt("raycastResolution/width");
  int raycastHeight = Gamedata::getInstance().getXmlInt("raycastResolution/height");
  

  
  for( int vertPixelX = 0; 
       vertPixelX < viewWidth; 
       vertPixelX++ 
  ) {

    // Scale the screen based on the original raycast resolution.	
    if(vertPixelX % (viewWidth / raycastWidth) == 0)
    {
      // Current X-coor in camera (-1 to 1).
      float cameraX = 2.0f * vertPixelX / viewWidth - 1;
      float rayDirX = player->getXFov() + planeX * cameraX;
      float rayDirY = player->getYFov() + planeY * cameraX;


      // Lengths of the ray from collidables[0]X and playerY to first
      // increment of the ray (x and y), and from one ray coordinates 
      // step to the next.
      float posX = player->getX() + player->getSpriteInfo()->getScaledWidth()/2 ;
      float posY = player->getY() + player->getSpriteInfo()->getScaledHeight()/2;

      // Use a grid system to test raycasting potential.
      // TODO: ints create better view when not on wall, floats create better
      // view when on wall. Determine how to fix.
      float gridX = posX;
      float gridY = posY;
   
      // Total length of the coordinate to the first wall encountered.
      float lengthDistX = 0;
      float lengthDistY = 0;

      // Amount to increment the length when attempting to find the 
      // collided wall's x and y.
      float deltaDistX = rayDirX != 0?fabs(1/rayDirX):0;
      float deltaDistY = rayDirY != 0?fabs(1/rayDirY):0;
      float wallDistance = 0;

      // Direction to move the ray's x and y coordinates when attempting 
      // to find a "hit" (1 or -1).
      float incrementX;
      float incrementY;

      // The value that the ray hit (Wall) and side that it hit.
      int rayHit = 0; 

      // Determine which way to send the increments. Negative values will head towards 
      // the left of the viewer's plane while positive values go right.
      // Need to offset initial length based on rotation of user.
      if(rayDirX < 0){
        incrementX = -1;
        lengthDistX = (posX - gridX) * deltaDistX;
      }
      else{
        incrementX = 1;
        lengthDistX = (gridX - posX + 1.0f) * deltaDistX;
      }
      if(rayDirY < 0){
        incrementY = -1;
        lengthDistY = (posY - gridY) * deltaDistY;
      }
      else{
        incrementY = 1;
        lengthDistY = (gridY - posY + 1.0f) * deltaDistY;
      }

      // Loop DDA until wall has been hit. Increment a single planeRay coordinate until 
      // it reaches past the other coordinate. Can be used to determine what part of the tile
      // the ray has hit. Plane ray x/y are the length while the mapx
      Sprite raySprite("Ray");
      while (rayHit == 0)
      {
        if(lengthDistX < lengthDistY){
          lengthDistX += deltaDistX;
          gridX += incrementX;
          side = 0;
        }
        else{
          lengthDistY += deltaDistY;
          gridY += incrementY;
          side = 1;
        }

        // Check for collision with a wall object.
        raySprite.setX(gridX);
        raySprite.setY(gridY);

        std::vector<SmartSprite*>::const_iterator spriteIt = sprites.begin();
        while( spriteIt != sprites.end() && rayHit == 0){
          if( strategies[currentStrategy]->execute( raySprite, **spriteIt) ){
            rayHit = 1; 
            if( spriteCollided == spriteIt ){
              newSpriteCollided = false;
            }
            else
              newSpriteCollided = true;
              
            spriteCollided = spriteIt;
          }
          else
            ++spriteIt;
        }
      }

      if(rayHit == 1)
      {
        // Find the total distance to the wall from the current vertPixelX.
        // This will be used to determine the length of the line drawn for 
        //the current vertPixelX.
        // Don't need to check for 0 for each of the functions since the 
        // side it hit's will tell us which value has been incrementing, 
        // and thus which is NOT 0.
        if(side == 0){
          wallDistance = ( gridX - posX + (1 - incrementX) / 2 ) / rayDirX;
        }
        else{
          wallDistance = ( gridY - posY + (1 - incrementY) / 2 ) / rayDirY;
        }	
        int vertLineLength = viewHeight * 10 / (wallDistance);

        // Find starting and ending pixel to draw to.
        drawTop = -vertLineLength / 2 + viewHeight / 2;
        if (drawTop < 0)
          drawTop = 0;

        drawBottom = vertLineLength / 2 + viewHeight / 2;
        if (drawBottom >= viewHeight)
          drawBottom = viewHeight - 1;

        // X location where wall was hit by ray.
        float wallRayX = 0;
        if(side == 0)
          wallRayX = posY + wallDistance * rayDirY;
        else
          wallRayX = posX + wallDistance * rayDirX;
        wallRayX -= floor(wallRayX);

        // Find the coordinate on the image of the sprite.
        int textureX = wallRayX * float((*spriteCollided)->getScaledWidth());
        if(side == 0 && rayDirX > 0) textureX = (*spriteCollided)->getScaledWidth() - textureX - 1;
        if(side == 1 && rayDirY > 0) textureX = (*spriteCollided)->getScaledWidth() - textureX - 1;

        // TextPixels contains the pixels of the obj that collided with the
        // ray.
        if(newSpriteCollided == true){
          textSurface = (*spriteCollided)->getSurface();
          textPixels = static_cast<Uint32 *>(textSurface->pixels);
        }
        Uint8 red, green, blue, alpha;
        unsigned pixel;
        for( int vertPixelY = drawTop; vertPixelY < drawBottom; vertPixelY++ )
        {
          // Scale the screen based on the original raycast resolution.	
          if( (vertPixelY % (viewHeight / raycastHeight)) == 0)
          {
            int d = vertPixelY * 256 - viewHeight * 128 + vertLineLength * 128;
            int textureY = ((d * ((*spriteCollided)->getScaledHeight()) / vertLineLength)) / 256;
          
            pixel = textPixels[(textureY * (*spriteCollided)->getScaledWidth()) + textureX];
            SDL_GetRGBA(pixel, textSurface->format, 
                      &red, &green, &blue, &alpha);


            SDL_SetRenderDrawColor(renderer, red, green, blue, alpha);
            SDL_RenderDrawPoint(renderer, vertPixelX, vertPixelY);

          }
        }

        //SDL_SetRenderDrawColor(renderer, side==0?255:128, 0, 0, 255);
        //SDL_RenderDrawLine(renderer, vertPixelX, drawTop, vertPixelX, drawBottom);   
      }
    }
    else {
      //SDL_SetRenderDrawColor(renderer, side==0?255:128, 0, 0, 255);
      //SDL_RenderDrawLine(renderer, vertPixelX, drawTop, vertPixelX, drawBottom);
    }  
  } 

  // Draw all sprites in container.
  for( auto& it : sprites )
  {
    it->draw();
  }
  for( auto& it : collidables )
  {
    it->draw();
  }

  if(hud.getActive() == true)
    hud.draw();
  viewport.draw();

  // Draw the texture/pixels of walls and sprites.
  SDL_RenderPresent(renderer);
}

// Collision Detection.
void Engine::checkForCollisions(){
    std::vector<WallCollidable*>::iterator currItr = collidables.begin();
    while( currItr != collidables.end() ){
      // Check until all collisions have been removed.
	    // Search through all the sprites to determine if collision has occurred.
      bool collisionDetected = true;
      while( collisionDetected == true ) {

        // Check against the Walls in the level.
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
/*
        // Check against other collidable objects.
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
    // Check bullets to see if they have collided with 
    // enemies/players and walls.
    std::vector<WallCollidable*>::iterator currGun = collidables.begin();
    while( currGun != collidables.end() ){
      auto currTarget = collidables.begin();
      // Enemy/Player.
      while( currTarget != collidables.end() ) {
        if(currGun != currTarget)
          (*currGun)->checkBulletCollision((*currTarget)->getSpriteInfo());
        currTarget++;
      }
      // Walls.
      auto currWall = sprites.begin();
      while( currWall != sprites.end() ) {
        (*currGun)->checkBulletCollision(*currWall);
        currWall++;
      }
      currGun++;
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
  hud.update(ticks);
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
        if (keystate[SDL_SCANCODE_LSHIFT]) {
          player->shoot();
        }
        if ( keystate[SDL_SCANCODE_P] ) {
          if ( clock.isPaused() ) clock.unpause();
          else clock.pause();
        }
	      if (keystate[SDL_SCANCODE_F1]) {
          std::cout << "Initiating hud" << std::endl;
          hud.toggleActive();
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
        player->left();
      }
      if (keystate[SDL_SCANCODE_D]) {
        player->right();
      }
      if (keystate[SDL_SCANCODE_W]) {
        player->up();
      }
      if (keystate[SDL_SCANCODE_S]) {
        player->down();
      }
      if(keystate[SDL_SCANCODE_LEFT]){
        player->rotateLeft();
      }
      if(keystate[SDL_SCANCODE_RIGHT]){
        player->rotateRight();
      }
      
      update(ticks);
      draw();
      if ( makeVideo ) {
        frameGen.makeFrame();
      }
    }
  }
}
