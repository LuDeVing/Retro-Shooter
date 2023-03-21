#ifndef MAPCLASS_H
#define MAPCLASS_H

#include <vector>

static enum blockTypes { NONE,  BLANK };

class Room {

	int width, height;
	std::vector <blockTypes> map;

public:


	Room() = default;
	Room(int mapWidth, int mapHeight);

	void changeBlock(int x, int y, blockTypes type);
	blockTypes getBlock(int x, int y);

	bool pointInRoom(int x, int y);

};

#endif