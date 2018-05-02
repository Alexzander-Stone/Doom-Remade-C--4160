#include <iostream>
#include <unistd.h>
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

  delete floor;
  delete [] depthBuffer;

  // Delete textures.
  delete [] pixels_to_draw;
  SDL_DestroyTexture(texture_buffer);

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
  hud(),
  floor(new Sprite("Ceiling")),
  floorPixels(static_cast<Uint32 *>(floor->getSurface()->pixels)),
  floorSurface(floor->getSurface()),
  depthBuffer(new int[Gamedata::getInstance().getXmlInt("view/width")]),
  texture_buffer(nullptr),
  pixels_to_draw(new Uint32[Gamedata::getInstance().getXmlInt("view/width") * Gamedata::getInstance().getXmlInt("view/height")])
{
  // Using an array of ints as a container for the pixels will save cycles 
  // through less method calls for per pixel calculations.
  texture_buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGR888, SDL_TEXTUREACCESS_STREAMING, 
				     Gamedata::getInstance().getXmlInt("view/width"), 
				     Gamedata::getInstance().getXmlInt("view/height"));

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
  int w = player->getSpriteInfo()->getScaledWidth()/2;
  int h = player->getSpriteInfo()->getScaledHeight()/2;

  // Boxed Arena Walls
  for(int i = 0; i < Gamedata::getInstance().getXmlInt("arena/size"); i++) {
    Vector2f spritePos(Gamedata::getInstance().getXmlInt("Wall/Horizontal/x")*i+20, 0*i);
    sprites.push_back( new SmartSprite("Wall/Horizontal", placeholderPlayerPos, w, h, spritePos) );

    spritePos = Vector2f( Gamedata::getInstance().getXmlInt("Wall/Horizontal/x")*i+20, 
                          Gamedata::getInstance().getXmlInt("Wall/Horizontal/x")*Gamedata::getInstance().getXmlInt("arena/size") + 20);
    sprites.push_back( new SmartSprite("Wall/Horizontal", placeholderPlayerPos, w, h, spritePos) );

    spritePos = Vector2f(0*i, Gamedata::getInstance().getXmlInt("Wall/Vertical/y")*i+20);
    sprites.push_back( new SmartSprite("Wall/Vertical", placeholderPlayerPos, w, h, spritePos) );

    spritePos = Vector2f( Gamedata::getInstance().getXmlInt("Wall/Vertical/x")*Gamedata::getInstance().getXmlInt("arena/size") + 20, 
                          i*Gamedata::getInstance().getXmlInt("Wall/Vertical/y")+20);
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
  int viewWidth = Gamedata::getInstance().getXmlInt("view/width");
  int viewHeight = Gamedata::getInstance().getXmlInt("view/height");
  
  // Draw background.
  world.draw(); 

  // Copy the pixels from texture to window.
  SDL_UpdateTexture(texture_buffer, NULL, pixels_to_draw, viewWidth * sizeof(Uint32));

  // Fill out the top half the the pixels with the sky.
  for(int backgroundY = 0; backgroundY < viewHeight/2; backgroundY++)
  {
    for( int backgroundX = 0; backgroundX < viewWidth-1; backgroundX++)
    {
      pixels_to_draw[backgroundX + backgroundY * viewWidth] = 0;
    }
  }
  
  // Loop through all the vertical stripes of the player's view (x's) based on 
  // the screen width/height. This will calculate the rays using a grid system.
  // Camera/plane variables for projection/rendering cycles. 
  float planeX = player->getPlaneX();   
  float planeY = player->getPlaneY();
  
  int side = 0;
  int drawTop = 0;
  int drawBottom = 0;
  float posX = player->getX() + player->getScaledWidth()/2;
  float posY = player->getY() + player->getScaledHeight()/2;
  float directionX = player->getXFov();
  float directionY = player->getYFov();

  // Boolean to optimize performance for rays hitting the same sprite. Saves
  // cycles by only updating surface if new sprite has been hit.
  bool newSpriteCollided = true;
  std::vector<SmartSprite*>::const_iterator spriteCollided;
  const SDL_Surface* textSurface; 
  Uint32* textPixels; 
  
  // Depth buffer will be used here when drawing the sprites.
  // Gives us the ability to scale enemy sprites based on the distance to the
  // player.
  // Cycle through each column of pixels that will be rendered to the screen.
  for( int vertPixelX = 0; vertPixelX < viewWidth; vertPixelX++ ) {
    // Scale the screen based on the original raycast resolution. Effectively
    // duplicate the previous pixels to create illusion of size increase.	
      // Current X-coor in camera (-1 to 1).
      // Current player position, centered.
      // Current "grid" position, made for performance increase.
      // Lengths of the ray from playerX and playerY to first
      // increment of the ray (x and y), and from one ray coordinates 
      // step to the next.
      float cameraX = 2.0f * static_cast<float>(vertPixelX) / static_cast<float>(viewWidth) - 1;
      int gridX = posX;
      int gridY = posY;
      float rayDirX = directionX + planeX * cameraX;
      float rayDirY = directionY + planeY * cameraX;
            
      // Total length of the coordinate to the first wall encountered.
      // Needed for determining the size/length of the wall to draw.
      // Amount to increment the length when attempting to find the 
      // collided wall's x and y.
      float lengthDistX = 0;
      float lengthDistY = 0;
      float deltaDistX = rayDirX != 0?fabs(1/rayDirX):0;
      float deltaDistY = rayDirY != 0?fabs(1/rayDirY):0;
      float wallDistance = 0;

      // Direction to move the ray's x and y coordinates when attempting 
      // to find a "hit" (1 or -1).
      // The value that the ray hit (Wall) and side that it hit.
      float incrementX;
      float incrementY;
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

      // Loop until wall has been hit. Increment a single planeRay/lengthDist coordinate until 
      // it reaches past the other coordinate. Can be used to determine what part of the tile
      // the ray has hit (side). 
      // Once collision with a wall has been found with the single ray, record
      // the sprite it has hit so it can be used to render it's image/pixels.
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
            if( spriteCollided == spriteIt )
              newSpriteCollided = false;
            else
              newSpriteCollided = true;
              
            spriteCollided = spriteIt;
          }
          else
            ++spriteIt;
        }
      }

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
      float vertLineLength = viewHeight  / wallDistance;

      // Find starting and ending pixel to draw to.
      drawTop = -vertLineLength / 2 + viewHeight / 2;
      if (drawTop < 0)
        drawTop = 0;

      drawBottom = vertLineLength / 2 + viewHeight / 2;
      if (drawBottom >= viewHeight)
        drawBottom = viewHeight - 1;

      // X location where wall was hit by ray. Needed for determining which
      // pixel to draw from the wall image that has collided with the ray.
      float wallRayX = 0;
      if(side == 0)
        wallRayX = posY + wallDistance * rayDirY;
      else
        wallRayX = posX + wallDistance * rayDirX;
      wallRayX -= std::floor(wallRayX);

      // Find the x coordinate on the image of the sprite based on wall ray x hit
      // location.
      int textureX = wallRayX * (*spriteCollided)->getScaledWidth();
      if(side == 0 && rayDirX > 0) textureX = (*spriteCollided)->getScaledWidth() - textureX - 1;
      if(side == 1 && rayDirY > 0) textureX = (*spriteCollided)->getScaledWidth() - textureX - 1;

      // TextPixels contains the pixels of the obj that collided with the
      // ray. These pixels will be the array for which we draw from (based on
      // pixel perfect collision strategy).
      if(newSpriteCollided == true){
        textSurface = (*spriteCollided)->getSurface();
        textPixels = static_cast<Uint32 *>(textSurface->pixels);
      }
      
      // Begin rendering the rows of each column to the screen. By using the
      // previously found texture x coordinate, we can determine the y
      // coordinate on the texture using the wall location.
      for( int vertPixelY = drawTop; vertPixelY < drawBottom; vertPixelY++ )
      {
          // Scale the screen based on the original raycast resolution.	
          int d = vertPixelY * 256 - viewHeight * 128 + vertLineLength * 128;
          int textureY = d*(*spriteCollided)->getScaledHeight() / vertLineLength / 256;
	        // Store the new pixel into the array until all pixels have been saved.
          pixels_to_draw[vertPixelX + vertPixelY * viewWidth] = textPixels[(textureY * (*spriteCollided)->getScaledWidth()) + textureX];;
      }

      // Record the distance into the depth buffer for further use.
      // No need for 2d array, only each vertical stripe matters for distance.
      depthBuffer[vertPixelX] = wallDistance;

      // Floor and ceiling casting. The pixels that aren't used in each row
      // will be filled using a ceiling/floor method.
      float floorX, floorY;

      // Find the direction of the wall that has been hit, use this to create
      // the wall and ceiling.  
      if(side == 0 && rayDirX > 0){
        floorX = gridX;
        floorY = gridY + wallRayX;
      }
      else if(side == 0 && rayDirX < 0){
        floorX = gridX + 1.0;
        floorY = gridY + wallRayX;
      }
      else if(side == 1 && rayDirY > 0){
        floorX = gridX + wallRayX;
        floorY = gridY;
      }
      else { // side == 1 && rayDirY < 0 
        floorX = gridX + wallRayX;
        floorY = gridY + 1.0;
      }

      float currDistance;
      float floorToWall = wallDistance; 
      
      // Render floor (and/or ceiling).
      for( int floorRow = drawBottom + 1; floorRow < viewHeight; floorRow++) {
        currDistance = viewHeight / (2.0 * floorRow - viewHeight);
        float linearInterpolation = currDistance / floorToWall;

        float currFloorX = linearInterpolation * floorX + (1.0 - linearInterpolation) * posX;
        float currFloorY = linearInterpolation * floorY + (1.0 - linearInterpolation) * posY;

        int floorTextureX = int( currFloorX * floor->getScaledWidth()) % floor->getScaledWidth();
        int floorTextureY = int( currFloorY * floor->getScaledHeight()) % floor->getScaledHeight();

        // Render pixel from the coordinate of the texture to the screen
        // coordinate. Can repeat for ceilings if wanted.
        pixels_to_draw[vertPixelX + floorRow * viewWidth] = floorPixels[(floorTextureY * floor->getScaledWidth()) + floorTextureX];
      }
  }

  // Use the depthBuffer to begin drawing the enemy sprites. (matrix math
  // hurts). Need to sort the sprites first, to determine which is painted
  // above the others (painters algorithm style).
  // Loop through all player and enemy sprites, while also obtaining their
  // bullets and placing them into the list to be sorted. 
  std::list<const Drawable*> depth_sprite_render;
  std::vector<WallCollidable*>::const_iterator itr = collidables.begin();
  // Skip the player object, but make sure to collect their bullets.
  for(unsigned int i = 0; i < (*itr)->getBulletCount(); i++){
      depth_sprite_render.push_back((*itr)->getBulletSprite(i));
  }
  itr++; // Skip player object.
  while( itr != collidables.end() )
  {
    depth_sprite_render.push_back((*itr)->getSpriteInfo());
    for(unsigned int i = 0; i < (*itr)->getBulletCount(); i++){
      depth_sprite_render.push_back((*itr)->getBulletSprite(i));
    }
    itr++;
  }
  // Begin sorting the sprites based on their distance to the player.
  float playerX = player->getX() + player->getScaledWidth()/2;
  float playerY = player->getY() + player->getScaledHeight()/2;
  depth_sprite_render.sort(
      [playerX, playerY]( const Drawable* lhs, const Drawable* rhs ) -> bool{ 
        return hypot(lhs->getX() + lhs->getScaledWidth()/2 - playerX, lhs->getY() + lhs->getScaledHeight()/2 - playerY) 
               <=
               hypot(rhs->getX() + rhs->getScaledWidth()/2 - playerX, rhs->getY() + rhs->getScaledHeight()/2 - playerY);
      }
  );

  // Begin rendering the sorted sprites.
  // Need to use a inverse camera matrix to render sprites on screen, for more
  // info check out lodev.org/cgtutor/raycasting3.html (helpful formulas).
  for( auto& ptr : depth_sprite_render ) {
    // Find the sprite's coordinates in relation to the camera.
    // Needed to find the enemies 3D coordinates
    float currSpriteX = ptr->getX() - posX;
    float currSpriteY = ptr->getY() - posY;
    if( ptr->getName() != "Pinkie" ){
      currSpriteX += ptr->getScaledWidth()/2;
      currSpriteY += ptr->getScaledHeight()/2;
    }
      

    float inverseCameraMatrix = 1.0f / (planeX * directionY - directionX * planeY);

    float cameraSpriteX = inverseCameraMatrix * ( directionY * currSpriteX - directionX * currSpriteY);
    float cameraSpriteY = inverseCameraMatrix * ( -planeY * currSpriteX + planeX * currSpriteY);

    int spriteScreenX = (viewWidth/2) * (1 + cameraSpriteX / cameraSpriteY);

    int spriteScreenHeight = abs(viewHeight/cameraSpriteY);
    int spriteScreenWidth = abs(viewHeight/cameraSpriteY);
    
    // Vertical pixels to draw to, based on sprite sizes.
    // Can be used to offset sprites.
    int drawTop = -spriteScreenHeight / 2 + viewHeight/2;
    if(drawTop < 0)
      drawTop = 0;
    int drawBottom = spriteScreenHeight /2 + viewHeight/2;
    if(drawBottom >= viewHeight)
      drawBottom = viewHeight - 1;

    // Horizontal pixels to draw to, based on sprite sizes.
    // Can be used to offset sprites.
    int drawLeft = -spriteScreenWidth / 2 + spriteScreenX;
    if(drawLeft < 0)
      drawLeft = 0;
    int drawRight = spriteScreenWidth /2 + spriteScreenX;
    if(drawRight >= viewWidth)
      drawRight = viewWidth -1;

    // Only draw the sprite if it's within the camera plane.
    for( int vertSprite = drawLeft; vertSprite < drawRight; vertSprite++ ) {
      // Within the region/rectangle of the camera/window.
      if( cameraSpriteY > 0 && vertSprite > 0 && vertSprite < viewWidth && cameraSpriteY < depthBuffer[vertSprite] ){
        float textureX = (256 * (vertSprite - (-spriteScreenWidth / 2 + spriteScreenX)) * ptr->getScaledWidth() / spriteScreenWidth) / 256;
        const SDL_Surface* spriteSurface = ptr->getSurface();
        textPixels = static_cast<Uint32 *>(spriteSurface->pixels);
        for( int horizSprite = drawTop; horizSprite < drawBottom; horizSprite++ ) {
          int d = horizSprite * 256 - viewHeight * 128 + spriteScreenHeight * 128;
          int textureY = ((d * ptr->getScaledHeight()) / (float)spriteScreenHeight) / 256;
	        Uint32 pixelAccess = textureY * (float)ptr->getScaledWidth() + textureX;
          if(textPixels[pixelAccess] != 4294902015) // magenta color, transparency 
            pixels_to_draw[vertSprite + horizSprite * viewWidth] = textPixels[pixelAccess];
        }
      }
    }
  }

  
  
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture_buffer, NULL, NULL);

  //for(auto& it : collidables)
    //it->draw();

  for(auto& it : sprites)
    it->draw();

  if(hud.getActive() == true)
    hud.draw();
  viewport.draw();

  SDL_RenderPresent(renderer);
}

// Collision Detection.
void Engine::checkForCollisions(){
    std::vector<WallCollidable*>::iterator currItr = collidables.begin();
    if( currItr != collidables.end() ){
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
      // Enemy/Player.
      auto currTarget = collidables.begin();
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
