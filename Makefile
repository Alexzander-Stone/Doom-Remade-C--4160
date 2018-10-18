CXX = g++

# Warnings frequently signal eventual errors:
CXXFLAGS=`sdl2-config --cflags` -g -W -Wall -Werror -std=c++11 -Weffc++ -Wextra -pedantic -O0 -I `sdl2-config --prefix`/include/

LDFLAGS = `sdl2-config --libs` -lm -lexpat -lSDL2_ttf -lSDL2_image -lSDL2_mixer

OBJS = \
  	renderContext.o \
	ioMod.o \
	parseXML.o \
	gamedata.o \
	viewport.o \
	world.o \
	spriteSheet.o \
	image.o \
	imageFactory.o \
	frameGenerator.o \
	sprite.o \
	multisprite.o \
	subjectSprite.o \
	wallCollidable.o \
	player.o \
	vector2f.o \
	clock.o \
	engine.o \
	smartSprite.o \
	collisionStrategy.o \
	enemy.o \
	hud.o \
	enemyProjectile.o \
	bullet.o \
	living.o \
	sound.o \
	healthBar.o \
	main.o
EXEC = run

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS)

clean:
	rm -rf $(OBJS)
	rm -rf $(EXEC)
	rm -rf frames/*.bmp
