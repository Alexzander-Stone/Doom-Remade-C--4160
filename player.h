#include <SDL2/SDL.h>
#include <string>

class Player {
public:
  static Player& getInstance();
  
  // Movement options
  void horizontalLeftMove();
  void horizontalRightMove();
  void verticalUpMove();
  void verticalDownMove();

private:
  std::string name;
  int x_location;
  int y_location;

  Player(const std::string&);
  Player();
};
