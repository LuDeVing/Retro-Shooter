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


Texture& Room::getWallTexture(int textureID){
	return wallTextures[textureID];
}

Texture& Room::getCeilTexture(int textureID){
	return ceilTextures[textureID];
}

Texture& Room::getFloorTexture(int textureID){
	return floorTextures[textureID];
}