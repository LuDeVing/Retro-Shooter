#ifndef MAPCLASS_H
#define MAPCLASS_H

#include <vector>
#include "misc.h"

static enum blockTypes { NONE,  WALL };

struct Block {

	blockTypes type = NONE;

	int wallTextureID = -1;
	int ceilTextureID = -1;
	int floorTextureID = -1;

};

class Room {

	int width, height;
	std::vector <Block> map;

	std::vector <Texture> wallTextures;
	std::vector <Texture> ceilTextures;
	std::vector <Texture> floorTextures;


public:


	Room() = default;
	Room(int mapWidth, int mapHeight);

	void changeBlock(int x, int y, blockTypes type);
	void changeWallTexture(int x, int y, int textureID);
	void changeCeilTexture(int x, int y, int textureID);
	void changeFloorTexture(int x, int y, int textureID);

	Block& getBlock(int x, int y);
	bool pointInRoom(int x, int y);
	Block& getBlockSizeAdjusted(float x, float y, int cubeSize);

	Texture& getWallTexture(int textureID);
	Texture& getCeilTexture(int textureID);
	Texture& getFloorTexture(int textureID);

	void addWallTexture(const char* texturePath);
	void addCeilTexture(const char* texturePath);
	void addFloorTexture(const char* texturePath);

};

#endif