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

int Room::getHeight() {
	return this->height;
}

int Room::getWidth() {
	return this->width;
}

std::vector <Block>& Room::getMap() {
	return this->map;
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

// ************************************* Enemy functions ********************************************** //

Block& Enemy::getBlockFloat(MapData& map, float x, float y) {

	int xPos = x / map.cubeSize;
	int yPos = y / map.cubeSize;

	if (xPos < 0 || xPos >= map.width || yPos < 0 || yPos >= map.height) {
		Block vl;
		return vl;
	}

	int blockPos = yPos * map.width + xPos;

	return (*map.map)[blockPos];

}

Block& Enemy::getBlockInt(MapData& map, int x, int y) {

	if (x < 0 || x >= map.width || y < 0 || y >= map.height) {
		Block vl;
		return vl;
	}

	int blockPos = y * map.width + x;

	return (*map.map)[blockPos];

}

// *************************************** PATH FINDING ************************************************ //

struct node {

	int x, y;
	int cost, costA, costB;

	node(int x, int y, int cost, int costA, int costB) {

		this->x = x;
		this->y = y;

		this->cost = cost;
		this->costA = costA;
		this->costB = costB;

	}

	bool operator < (const node& o2) const {

		if (cost == o2.cost) {

			if (costB == o2.costB)
				return (costA < o2.costA);

			return (costB < o2.costB);

		}

		return (cost < o2.cost);

	}


};

MapData::MapData() {

	this->map = nullptr;

	this->cubeSize = 0;
	this->width = 0;
	this->height = 0;

}

int calcDist(int x1, int y1, int x2, int y2) {

	int dx = abs(x1 - x2);
	int dy = abs(y1 - y2);
	int mn = std::min(dx, dy);
	int mx = std::max(dx, dy);

	return mn * 14 + (mx - mn) * 10;

}

void fillSet(std::multiset <node>& st, std::pair <int, int> cur, std::pair <int, int> par,
	std::vector < std::vector <std::pair <int, int> > >& last,
	std::pair <int, int> startIdx, std::pair <int, int> endIdx, int vsw, int vsh, std::vector <std::vector <bool> >& used) {

	if (cur.first >= 0 && cur.first < vsw &&
		cur.second >= 0 && cur.second < vsh &&
		!used[cur.second][cur.first]) {

		used[cur.second][cur.first] = true;
		last[cur.second][cur.first] = par;

		int costA = calcDist(cur.first, cur.second, startIdx.first, startIdx.second);
		int costB = calcDist(cur.first, cur.second, endIdx.first, endIdx.second);

		node Node = node(cur.first, cur.second, costA + costB, costA, costB);

		st.insert(Node);

	}

}

olc::vf2d Enemy::findShortestPath(MapData& map, olc::vf2d startIdxF, olc::vf2d endIdxF) {

	int vsh = map.height;
	int vsw = map.width;

	std::pair <int, int> startIdx = { startIdxF.x / map.cubeSize, startIdxF.y / map.cubeSize };
	std::pair <int, int> endIdx = { endIdxF.x / map.cubeSize, endIdxF.y / map.cubeSize };

	std::vector <std::vector <bool> > used(vsh, std::vector <bool> (vsw));

	std::vector < std::vector <std::pair <int, int> > > last =
		std::vector < std::vector < std::pair <int, int> > >(vsh, std::vector < std::pair <int, int> >(vsw));

	for (int curY = 0; curY < vsh; curY++) {
		for (int curX = 0; curX < vsw; curX++) {

			used[curY][curX] = false;

			if (getBlockInt(map, curX, curY).type != NONE)
				used[curY][curX] = true;

			last[curY][curX] = { -1, -1 };

		}
	}

	std::multiset <node> st;

	st.insert(node(startIdx.first, startIdx.second,
		calcDist(startIdx.first, startIdx.second, endIdx.first, endIdx.second), 0,
		calcDist(startIdx.first, startIdx.second, endIdx.first, endIdx.second)));

	std::pair <int, int> cur = startIdx;

	used[startIdx.second][startIdx.first] = true;
	last[startIdx.second][startIdx.first] = { startIdx.first, startIdx.second };

	while (cur != endIdx && !st.empty()) {

		auto ptr = st.begin();
		cur = { ptr->x, ptr->y };
		st.erase(st.begin());

		if (cur == endIdx) break;

		fillSet(st, { cur.first - 1, cur.second }, cur, last, startIdx, endIdx, vsw, vsh, used);
		fillSet(st, { cur.first + 1, cur.second }, cur, last, startIdx, endIdx, vsw, vsh, used);
		fillSet(st, { cur.first, cur.second - 1 }, cur, last, startIdx, endIdx, vsw, vsh, used);
		fillSet(st, { cur.first, cur.second + 1 }, cur, last, startIdx, endIdx, vsw, vsh, used);

	}

	olc::vi2d retValInt = olc::vi2d();

	while (endIdx != startIdx) {

		if (endIdx == std::pair<int, int>({ -1, -1 })) {
			olc::vi2d retVal = olc::vi2d(startIdxF.x / map.cubeSize, startIdxF.y / map.cubeSize);
			break;
		}
		
		retValInt = olc::vi2d(endIdx.first, endIdx.second);
		

		endIdx = last[endIdx.second][endIdx.first];

	}

	return olc::vf2d(retValInt.x * map.cubeSize + map.cubeSize / 2, retValInt.y * map.cubeSize + map.cubeSize / 2);

}

bool Enemy::fixCollisionOnRadius(MapData& map) {

	float posX, posY;

	for (int k = -1; k <= 1; k++) {
		for (int j = -1; j <= 1; j++) {

			posX = x + radius * (j == 0 ? k : k * sqrtof2);
			posY = y + radius * (k == 0 ? j : j * sqrtof2);

			if (getBlockFloat(map, posX, posY).type != blockTypes::NONE)
				return true;

		}
	}

	return false;

}

void Enemy::fixEnemyToWallCollision(MapData& map, float moveX, float moveY) {

	if (fixCollisionOnRadius(map)) {
		x -= moveX;
	}

	if (fixCollisionOnRadius(map)) {
		x += moveX;
		y -= moveY;
	}

	if (fixCollisionOnRadius(map)) {
		x -= moveX;
	}

}

void Enemy::enemyCicle(Player& player, MapData& map, float elapsedTime) {

	int dirX = 0;
	int dirY = 0;

	olc::vf2d nextPos = Enemy::findShortestPath(map, olc::vf2d(x, y), player.position);

	if (x < nextPos.x) dirX = 1;
	if (x > nextPos.x) dirX = -1;

	if (y < nextPos.y) dirY = 1;
	if (y > nextPos.y) dirY = -1;

	x += dirX * speed * elapsedTime;
	y += dirY * speed * elapsedTime;

	fixEnemyToWallCollision(map, dirX * speed * elapsedTime, dirY * speed * elapsedTime);

}

// ************************************* Bullet functions ********************************************** //

Block& Bullet::getBlockFloat(MapData& map, float x, float y) {

	int xPos = x / map.cubeSize;
	int yPos = y / map.cubeSize;

	if (xPos < 0 || xPos >= map.width || yPos < 0 || yPos >= map.height) {
		Block vl;
		return vl;
	}

	int blockPos = yPos * map.width + xPos;

	return (*map.map)[blockPos];

}

Block& Bullet::getBlockInt(MapData& map, int x, int y) {

	if (x < 0 || x >= map.width || y < 0 || y >= map.height) {
		Block vl;
		return vl;
	}

	int blockPos = y * map.width + x;

	return (*map.map)[blockPos];


}



bool Bullet::getBulletToWallCollision(MapData& map) {

	float posX, posY;

	for (int k = -1; k <= 1; k++) {
		for (int j = -1; j <= 1; j++) {

			posX = x + radius * (j == 0 ? k : k * sqrtof2);
			posY = y + radius * (k == 0 ? j : j * sqrtof2);

			if (getBlockFloat(map, posX, posY).type != blockTypes::NONE)
				return true;

		}
	}

	return false;


}

olc::vf2d vectorProjection(const olc::vf2d& v1, const olc::vf2d& v2) {
	float v2_ls = v2.x * v2.x + v2.y * v2.y;
	return v2 * (v2.dot(v1) / v2_ls);
}

bool Bullet::checkProjectionLength(Player& player, Enemy& enemy, float distEpsilon) {
	
	olc::vf2d playerForVec = olc::vf2d(cosf(player.roation), sinf(player.roation));

	olc::vf2d vecBullet = olc::vf2d(x - player.position.x, y - player.position.y);
	olc::vf2d vecEnemy = olc::vf2d(enemy.x - player.position.x, enemy.y - player.position.y);

	olc::vf2d bulletProj = (vecBullet, playerForVec);
	olc::vf2d enemyProj = (vecEnemy, playerForVec);

	float dist1 = bulletProj.x * bulletProj.x + bulletProj.y * bulletProj.y;
	float dist2 = enemyProj.x * enemyProj.x + enemyProj.y * enemyProj.y;

	return (fabs(dist2 - dist1) <= distEpsilon * distEpsilon);

}

bool Bullet::bulletAndEnemyCollision(std::vector <Enemy>& enemies, Player& player) {

	bool retVal = false;

	float bulletRadiusSquared = entityRadius * entityRadius;

	for (int k = 0; k < enemies.size(); k++) {

		float enemyRadiuseSqruared = enemies[k].entityRadius * enemies[k].entityRadius;
		float differenceSquared = (enemies[k].x - x) * (enemies[k].x - x) + (enemies[k].y - y) * (enemies[k].y - y);

		if (bulletRadiusSquared + enemyRadiuseSqruared >= differenceSquared && checkProjectionLength(player, enemies[k], 5.0f)) {
			enemies[k].health -= damage;
			retVal = true;
		}

	}

	return retVal;

}

bool Bullet::bulletCicle(Player& player, std::vector <Enemy>& enemies, MapData& map, float elapsedTime) {

	x += direction.x * speed * elapsedTime;
	y += direction.y * speed * elapsedTime;

	bool getDestroyed = false;

	getDestroyed |= getBulletToWallCollision(map);
	getDestroyed |= bulletAndEnemyCollision(enemies, player);

	return getDestroyed;

}

// ****************************************** DOOR ****************************************** //

void Block::doorCicle(const olc::vf2d& playerPos, int cubeSize, olc::vi2d thisPos, float deltaTime) {

	if (isOpen) {

		type = NONE;
		closeTime -= deltaTime;
		
		if (thisPos.x == (int)playerPos.x / cubeSize && thisPos.y == (int)playerPos.y / cubeSize)
			closeTime = 0.3f;

		if (closeTime <= 0) {
			closeTime = OGCloseTime;
			type = DOOR;
			isOpen = false;
		}

	}

}