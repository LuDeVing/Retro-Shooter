#ifndef MAPCLASS_H
#define MAPCLASS_H

#include <vector>
#include "misc.h"
#include "Player.h"

static enum blockTypes { NONE,  WALL };

struct Block {

	blockTypes type = NONE;

	int wallTextureID = -1;
	int ceilTextureID = -1;
	int floorTextureID = -1;

	bool hasCeil = true;

};

struct Sprite {

	float x, y, z;
	float width = 0, height = 0;

	int textureSectionID, textureID;
	
	// textureSectionID:
	// 0 -> static
	// 1 -> enemy
	// 2 -> bullet


	Sprite();
	Sprite(float x, float y, float z, float width, float height);

};


struct Enemy : public Sprite {

	float speed = 75.0f;

	Enemy();
	Enemy(float x, float y, float z, float width, float height);

	void enemyCicle(Player& player, float elapsedTime);

};

struct Bullet : public Sprite {

	Bullet();
	Bullet(float x, float y, float z, float width, float height);

	void bulletCicle(Player& player, float elapsedTime);

};

class Room {

	int width, height;

	std::vector <Block> map;
	std::vector <Sprite> sprite;

	std::vector <Texture> wallTextures;
	std::vector <Texture> ceilTextures;
	std::vector <Texture> floorTextures;

	std::vector <Texture> spriteTextures[3];


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
	Texture& getSpriteTexture(int textureID, int sectionID);

	void addWallTexture(const char* texturePath);
	void addCeilTexture(const char* texturePath);
	void addFloorTexture(const char* texturePath);
	void addSpriteTexture(const char* texturePath, int sectionID);

};

#endif