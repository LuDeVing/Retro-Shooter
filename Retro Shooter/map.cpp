#include "map.h"

// ************************************* Helper functions and viarables ********************************************** //

#define PI 3.14159265359
#define DEG 0.0174533
#define FLOATMAX 3.40282e+038

bool pointInRoom(MapData& map, int x, int y) {
	return !(x < 0 || x >= map.width || y < 0 || y >= map.height);
}

Block& getBlockFloat(MapData& map, float x, float y) {

	int xPos = x / map.cubeSize;
	int yPos = y / map.cubeSize;

	if (xPos < 0 || xPos >= map.width || yPos < 0 || yPos >= map.height) {
		Block vl;
		return vl;
	}

	int blockPos = yPos * map.width + xPos;

	return (*map.map)[blockPos];

}

Block& getBlockInt(MapData& map, int x, int y) {

	if (x < 0 || x >= map.width || y < 0 || y >= map.height) {
		Block vl;
		return vl;
	}

	int blockPos = y * map.width + x;

	return (*map.map)[blockPos];

}

std::pair <float, std::pair<olc::vf2d, bool>> getRayDistance(MapData& map, olc::vf2d rayPos, float rot, bool changeSign = false) {

	bool side;

	float shortestLength = FLOATMAX;

	float interx, intery;
	float dx, dy;

	bool breakFromLoop = false;

	olc::vf2d retVal(-10000000000, -10000000000);

	if (rot != 2 * PI && rot > PI) {
		
		intery = int(rayPos.y) / map.cubeSize * map.cubeSize - 0.001;
		interx = rayPos.x - (rayPos.y - intery) / tanf(rot) * (changeSign ? -1 : 1);

		dy = -map.cubeSize;
		dx = -map.cubeSize / tanf(rot);

		if (changeSign)
			dx *= -1;

	}
	else if (rot < PI) {

		intery = int(rayPos.y) / map.cubeSize * map.cubeSize + map.cubeSize;
		interx = rayPos.x - (rayPos.y - intery) / tanf(rot);

		dy = map.cubeSize;
		dx = map.cubeSize / tanf(rot);

	}


	if (rot == 0 || rot == PI) {
		breakFromLoop = true;
	}

	int maxIterations = 100;

	while (!breakFromLoop) {

		int pointx = interx / map.cubeSize;
		int pointy = intery / map.cubeSize;

		if (!pointInRoom(map, pointx, pointy) || --maxIterations <= 0)
			break;

		if (getBlockInt(map, pointx, pointy).type != blockTypes::NONE) {

			float delX2 = (interx - rayPos.x) * (interx - rayPos.x);
			float delY2 = (intery - rayPos.y) * (intery - rayPos.y);

			float newLenght = sqrtf(delX2 + delY2);

			if (newLenght < shortestLength) {

				shortestLength = newLenght;
				retVal = olc::vf2d(interx, intery);
				side = 0;

			}

			break;

		}

		interx += dx;
		intery += dy;
	}

	interx = 0, intery = 0;
	dx = 0, dy = 0;

	breakFromLoop = false;

	if (rot > PI / 2 && rot < 3 * PI / 2) {

		interx = int(rayPos.x) / map.cubeSize * map.cubeSize - 0.001;
		intery = rayPos.y - (rayPos.x - interx) * tanf(rot);

		dx = -map.cubeSize;
		dy = -map.cubeSize * tanf(rot);

	}
	else if (rot < PI / 2 || rot > 3 * PI / 2) {

		interx = int(rayPos.x) / map.cubeSize * map.cubeSize + map.cubeSize;
		intery = rayPos.y - (rayPos.x - interx) * tanf(rot);

		dx = map.cubeSize;
		dy = map.cubeSize * tanf(rot);

		if (changeSign)
			dx *= -1;

	}

	if (rot == PI / 2 || rot == rot > 3 * PI / 2) {
		breakFromLoop = true;
	}
	
	maxIterations = 100;

	while (!breakFromLoop) {

		int pointx = (int)interx / map.cubeSize;
		int pointy = (int)intery / map.cubeSize;

		if (!pointInRoom(map, pointx, pointy) || --maxIterations <= 0)
			break;

		if (getBlockInt(map, pointx, pointy).type != blockTypes::NONE) {

			float delX2 = (interx - rayPos.x) * (interx - rayPos.x);
			float delY2 = (intery - rayPos.y) * (intery - rayPos.y);

			float newLenght = sqrtf(delX2 + delY2);

			if (newLenght < shortestLength) {

				shortestLength = newLenght;
				retVal = olc::vf2d(interx, intery);
				side = 1;

			}

			break;

		}

		interx += dx;
		intery += dy;

	}

	return { shortestLength, {retVal,  side} };

}

Room::Room(int mapWidth, int mapHeight) {

	this->width = mapWidth;
	this->height = mapHeight;

	this->map.resize(width * height, Block());

};

float fixAngle(float Angle) {

	if (Angle < 0)
		Angle += 2 * PI;

	if (Angle > 2 * PI)
		Angle -= 2 * PI;

	return Angle;

}

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

Enemy::Enemy(int mapWidth, int mapHeight) {

	this->textureSectionID = 1;

	used = std::vector <std::vector <bool> >(mapHeight, std::vector <bool>(mapWidth));

}

Enemy::Enemy(float x, float y, float z, float width, float height, int mapWidth, int mapHeight) {

	this->textureSectionID = 1;

	this->x = x;
	this->y = y;
	this->z = z;

	this->width = width;
	this->height = height;

	used = std::vector <std::vector <bool> >(mapHeight, std::vector <bool>(mapWidth));

}

Bullet::Bullet() {
	this->textureSectionID = 2;
}

Bullet::Bullet(float x, float y, float z, float width, float height, int bulletTagID) {

	this->textureSectionID = 2;

	this->x = x;
	this->y = y;
	this->z = z;

	this->width = width;
	this->height = height;

	this->bulletTagID = bulletTagID;
	this->zDirection = zDirection;

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

	int maxDistance = 100;

	while (endIdx != startIdx) {

		if (endIdx == std::pair<int, int>({ -1, -1 }) || --maxDistance <= 0) {
			retValInt = olc::vi2d(startIdxF.x / map.cubeSize, startIdxF.y / map.cubeSize);
			isActive = false;
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

bool Enemy::enemyCicle(Player& player, MapData& map, float elapsedTime) {

	if (!isActive) {

		checkPlayer -= elapsedTime;
		
		if (checkPlayer <= 0) {
			checkPlayer = checkPlayerOg;
		}
		else return false;

	}

	olc::vf2d ETP = olc::vf2d(player.position.x - x, player.position.y - y);
	float ETPDist = sqrtf(ETP.x * ETP.x + ETP.y * ETP.y);


	float angleOfRay = fixAngle(asinf(ETP.y / ETPDist));

	bool changeSign = (ETP.x < 0);

	float distToWall = getRayDistance(map, olc::vf2d(x, y), angleOfRay, changeSign).first;

	if (ETPDist < distToWall && distToWall < FLOATMAX / 10) {
		isActive = true;
	}
	
	if (!isActive) return false;

	int dirX = 0;
	int dirY = 0;

	olc::vf2d nextPos = Enemy::findShortestPath(map, olc::vf2d(x, y), player.position);

	if (ETPDist * ETPDist > (map.cubeSize / 2 * map.cubeSize / 2)) {

		if (x < nextPos.x) dirX = 1;
		if (x > nextPos.x) dirX = -1;

		if (y < nextPos.y) dirY = 1;
		if (y > nextPos.y) dirY = -1;

		x += dirX * speed * elapsedTime;
		y += dirY * speed * elapsedTime;

		fixEnemyToWallCollision(map, dirX * speed * elapsedTime, dirY * speed * elapsedTime);

	}

	bool shootBullet = false;

	curFireRateNum -= elapsedTime;

	if (curFireRateNum < 0 && ETPDist < distToWall && distToWall < FLOATMAX / 10) {
		curFireRateNum = fireRate;
		shootBullet = true;
	}

	if (shootBullet) {
		PlaySound(TEXT("Resources/Sounds/EnemyShoot.wav"), NULL, SND_ASYNC);
		textureID = shootTextureID; 
		shootingTextureStay = ogShootingTextureStay;
	}
	else if(shootingTextureStay >= 0) {
		shootingTextureStay -= elapsedTime;
	}
	else {
		textureID = staticTextureID;
	}
	
	return shootBullet;

}

// ************************************* Bullet functions ********************************************** //


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

		if (bulletTagID != enemies[k].bulletTagID)
			continue;

		float enemyRadiuseSqruared = enemies[k].entityRadius * enemies[k].entityRadius;
		float differenceSquared = (enemies[k].x - x) * (enemies[k].x - x) + (enemies[k].y - y) * (enemies[k].y - y);

		if (bulletRadiusSquared + enemyRadiuseSqruared >= differenceSquared && checkProjectionLength(player, enemies[k], 5.0f)) {
			enemies[k].health -= damage;
			retVal = true;
		}

	}

	if (retVal) {
		PlaySound(TEXT("Resources/Sounds/EnemyHit.wav"), NULL, SND_ASYNC);
	}

	return retVal;

}

bool Bullet::bulletAndPlayeCollision(Player& player) {

	bool retVal = false;

	if (bulletTagID != player.bulletTagID)
		return false;

	float bulletRadiusSquared = entityRadius * entityRadius;

	float enemyRadiuseSqruared = player.entityRadius * player.entityRadius;
	float differenceSquared = (player.position.x - x) * (player.position.x - x) + (player.position.y - y) * (player.position.y - y);


	if (bulletRadiusSquared + enemyRadiuseSqruared >= differenceSquared) {
		player.health -= damage;
		retVal = true;
	}

	if (retVal) {
		PlaySound(TEXT("Resources/Sounds/PlayerHit.wav"), NULL, SND_ASYNC);
	}

	return retVal;

}

bool Bullet::bulletCicle(Player& player, std::vector <Enemy>& enemies, MapData& map, float elapsedTime) {

	x += direction.x * speed * elapsedTime;
	y += direction.y * speed * elapsedTime;
	z += zDirection * speed * elapsedTime;

	bool getDestroyed = false;

	getDestroyed |= getBulletToWallCollision(map);
	getDestroyed |= bulletAndEnemyCollision(enemies, player);
	getDestroyed |= bulletAndPlayeCollision(player);

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
			PlaySound(TEXT("Resources/Sounds/DoorSound.wav"), NULL, SND_ASYNC);
			closeTime = OGCloseTime;
			type = DOOR;
			isOpen = false;
		}

	}

}
