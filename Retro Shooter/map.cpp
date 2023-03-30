#include "map.h"

Room::Room(int mapWidth, int mapHeight) {

	this->width = mapWidth;
	this->height = mapHeight;

	this->map.resize(width * height, Block());

};

void Room::changeBlock(int x, int y, blockTypes type) {

	if (x < 0 || x >= width || y < 0 || y >= height)
		return;

	int blockPos = y * width + x;

	map[blockPos].type = type;

}

void Room::changeWallTexture(int x, int y, int textureID) {

	if (x < 0 || x >= width || y < 0 || y >= height)
		return;

	int blockPos = y * width + x;

	map[blockPos].wallTextureID = textureID;

}

void Room::changeCeilTexture(int x, int y, int textureID) {

	if (x < 0 || x >= width || y < 0 || y >= height)
		return;

	int blockPos = y * width + x;

	map[blockPos].ceilTextureID = textureID;

}

void Room::changeFloorTexture(int x, int y, int textureID) {

	if (x < 0 || x >= width || y < 0 || y >= height)
		return;

	int blockPos = y * width + x;

	map[blockPos].floorTextureID = textureID;

}

Block& Room::getBlock(int x, int y) {

	if (x < 0 || x >= width || y < 0 || y >= height) {
		Block vl;
		return vl;
	}
		
	int blockPos = y * width + x;

	return map[blockPos];

}

bool Room::pointInRoom(int x, int y) {
	return !(x < 0 || x >= width || y < 0 || y >= height);
}

Block& Room::getBlockSizeAdjusted(float x, float y, int cubeSize) {

	int posX = (int)x / cubeSize;
	int posY = (int)y / cubeSize;
	
	return getBlock(posX, posY);

}

void Room::addWallTexture(const char* texturePath) {
	wallTextures.push_back(Texture(texturePath));
}

void Room::addFloorTexture(const char* texturePath) {
	floorTextures.push_back(Texture(texturePath));
}

void Room::addCeilTexture(const char* texturePath) {
	ceilTextures.push_back(Texture(texturePath));
}

void Room::addSpriteTexture(const char* texturePath, int sectionID) {
	spriteTextures[sectionID].push_back(Texture(texturePath));
}

Texture& Room::getWallTexture(int textureID){
	return wallTextures[textureID];
}

Texture& Room::getCeilTexture(int textureID){
	return ceilTextures[textureID];
}

Texture& Room::getFloorTexture(int textureID){
	return floorTextures[textureID];
}

Texture& Room::getSpriteTexture(int textureID, int sectionID) {
	return spriteTextures[sectionID][textureID];
}

Sprite::Sprite() {
	this->textureSectionID = 0;
}

Sprite::Sprite(float x, float y, float z, float width, float height) {
	
	this->textureSectionID = 0;

	this->x = x;
	this->y = y;
	this->z = z;

	this->width = width;
	this->height = height;

}

Enemy::Enemy() {
	this->textureSectionID = 1;
}

Enemy::Enemy(float x, float y, float z, float width, float height) {

	this->textureSectionID = 1;

	this->x = x;
	this->y = y;
	this->z = z;

	this->width = width;
	this->height = height;

}

Bullet::Bullet() {
	this->textureSectionID = 2;
}

Bullet::Bullet(float x, float y, float z, float width, float height) {

	this->textureSectionID = 2;

	this->x = x;
	this->y = y;
	this->z = z;

	this->width = width;
	this->height = height;

}

void Enemy::enemyCicle(Player& player, float elapsedTime) {

	int dirX = 0;
	int dirY = 0;

	if (x < player.position.x) dirX = 1;
	if (x > player.position.x) dirX = -1;

	if (y < player.position.y) dirY = 1;
	if (y > player.position.y) dirY = -1;

	x += dirX * speed * elapsedTime;
	y += dirY * speed * elapsedTime;

	//std::cout << x << ' ' << y << '\n';

}