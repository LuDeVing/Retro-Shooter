#include "map.h"

Room::Room(int mapWidth, int mapHeight) {

	this->width = mapWidth;
	this->height = mapHeight;

	this->map.resize(width * height, NONE);

};

void Room::changeBlock(int x, int y, blockTypes type) {

	if (x < 0 || x >= width || y < 0 || y >= height)
		return;

	int blockPos = y * width + x;

	map[blockPos] = type;

}

blockTypes Room::getBlock(int x, int y) {

	if (x < 0 || x >= width || y < 0 || y >= height)
		return NONE;

	int blockPos = y * width + x;

	return map[blockPos];

}

bool Room::pointInRoom(int x, int y) {
	return !(x < 0 || x >= width || y < 0 || y >= height);
}

