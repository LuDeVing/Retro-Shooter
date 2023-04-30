#ifndef MAPCLASS_H
#define MAPCLASS_H

#include <vector>
#include <set>
#include <utility>

#include "misc.h"
#include "Player.h"

static enum blockTypes { NONE,  WALL, DOOR, ENDDOOR };
static enum powerUpTypes { NOPWR, HEALTH1, HEALTH2, HEALTH3 };

struct Block {

	blockTypes type = NONE;

	int wallTextureID = -1;
	int ceilTextureID = -1;
	int floorTextureID = -1;

	bool hasCeil = true;

	float OGCloseTime = 1.5f;
	float closeTime = OGCloseTime;
	
	bool isEndDoor = false;

	bool isDoor = false;
	bool isOpen = false;

	void doorCicle(const olc::vf2d& playerPos, int cubeSize, olc::vi2d thisPos, float deltaTime);

};

struct MapData {
	
	std::vector <Block>* map;

	int width;
	int height;

	int cubeSize;

	MapData();

};

struct Sprite {

	float x, y, z;
	float width = 0, height = 0;

	int textureSectionID, textureID;
	
	powerUpTypes powerUpType = powerUpTypes::NOPWR;

	
	// textureSectionID:
	// 0 -> static
	// 1 -> enemy
	// 2 -> bullet


	Sprite();
	Sprite(float x, float y, float z, float width, float height);

};


class Enemy : public Sprite {

public:

	float health = 100.0f;

	float speed = 75.0f;
	float radius = 1.0f;

	float entityRadius = 25.0f;

	float fireRate = 0.5f;

	bool isActive = false;
	int bulletTagID = 0;

	int staticTextureID = 0;
	int shootTextureID = 1;
	int deadTextureID = 0;

	float shootingTextureStay = 2.5f;
	float ogShootingTextureStay = 0.1f;

	float checkPlayerOg = 0.05f;
	float checkPlayer = 0.05f;

	Enemy(int mapWidth, int mapHeight);
	Enemy(float x, float y, float z, float width, float height, int mapWidth, int mapHeight);

	bool enemyCicle(Player& player, MapData& map, float elapsedTime);

private:


	std::vector <std::vector <bool> > used;

	float curFireRateNum = 0.2f;

	float sqrtof2 = sqrtf(2.0f);

	bool fixCollisionOnRadius(MapData& map);
	void fixEnemyToWallCollision(MapData& map, float moveX, float moveY);

	olc::vf2d findShortestPath(MapData& map, olc::vf2d startIdxF, olc::vf2d endIdxF);

};

struct Bullet : public Sprite {

public:

	float damage = 20.0f;

	olc::vf2d direction;

	float zDirection = 0.0f;

	float speed = 1000.0f;
	float radius = 0.5f;

	float entityRadius = 1.0f;

	int bulletTagID = 0;

	Bullet();
	Bullet(float x, float y, float z, float width, float height, int bulletTagID = 0);

	bool bulletCicle(Player& player, std::vector <Enemy>& enemies, MapData& map, float elapsedTime);

private:

	float sqrtof2 = sqrtf(2.0f);

	bool getBulletToWallCollision(MapData& map);
	bool bulletAndEnemyCollision(std::vector <Enemy>& enemies, Player& player);
	bool bulletAndPlayeCollision(Player& player);

	bool checkProjectionLength(Player& player, Enemy& enemy, float distEpsilon);

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

	int getHeight();
	int getWidth();

	std::vector <Block>& getMap();

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
