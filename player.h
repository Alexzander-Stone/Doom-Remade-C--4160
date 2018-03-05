#include <SDL2/SDL.h>
#include <string>

class Player {
public:
  static Player& getInstance();
  Player& operator=(const Player&);

  // Movement options
  void horizontalLeftMove();
  void horizontalRightMove();
  void verticalUpMove();
  void verticalDownMove();
  // Drawing
  void draw();

private:
  std::string name;
  int x_location;
  int y_location;
  SDL_Renderer * renderer; 

  Player(const std::string&);
  Player(const Player&);
  Player();
};
