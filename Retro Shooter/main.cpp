#include <iostream>
#include <utility>

#define STB_IMAGE_IMPLEMENTATION

#include "Player.h"
#include "map.h"

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define PI 3.14159265359
#define DEG 0.0174533

constexpr int screenWidth = 256;
constexpr int screenheight = 240;

int level[400] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
	1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 ,1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
	1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

class Example : public olc::PixelGameEngine {
public:

	Player player;

	Room room;
	MapData mapData;

	Texture skyTexture;

	std::vector <Enemy> enemies;
	std::vector <Bullet> bullets;

	const int cubeSize = 64;

	const float sqrtof2 = sqrtf(2.0f);

	int fov;
	int numberOfRays;

	float ceilShade = 0.7f;
	float floorShade = 0.7f;

	float depthBuffer[screenWidth];

	Example() {
		sAppName = "Example";
	}

	/*************************************** MATH FUNCTIONS ***************************************/

	std::pair <float, std::pair<olc::vd2d, bool>> getRayDistance(Room& curRoom, float rot) {

		olc::vd2d rayPos = olc::vd2d(player.position);

		bool side;

		float shortestLength = 10000000000;

		float interx, intery;
		float dx, dy;

		bool breakFromLoop = false;

		olc::vd2d retVal(-10000000000, -10000000000);

		if (rot != 2 * PI && rot > PI) {

			intery = int(rayPos.y) / cubeSize * cubeSize - 0.0001;
			interx = rayPos.x - (rayPos.y - intery) / tanf(rot);

			dy = -cubeSize;
			dx = -cubeSize / tanf(rot);

		}
		else if (rot < PI) {

			intery = int(rayPos.y) / cubeSize * cubeSize + cubeSize;
			interx = rayPos.x - (rayPos.y - intery) / tanf(rot);

			dy = cubeSize;
			dx = cubeSize / tanf(rot);

		}

		if (rot == 0 || rot == PI) {
			breakFromLoop = true;
		}

		while (!breakFromLoop) {

			int pointx = interx / cubeSize;
			int pointy = intery / cubeSize;

			if (!curRoom.pointInRoom(pointx, pointy))
				break;

			if (curRoom.getBlock(pointx, pointy).type != blockTypes::NONE) {

				float delX2 = (interx - rayPos.x) * (interx - rayPos.x);
				float delY2 = (intery - rayPos.y) * (intery - rayPos.y);

				float newLenght = sqrtf(delX2 + delY2);

				if (newLenght < shortestLength) {

					shortestLength = newLenght;
					retVal = olc::vd2d(interx, intery);
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

			interx = int(rayPos.x) / cubeSize * cubeSize - 0.0001;
			intery = rayPos.y - (rayPos.x - interx) * tanf(rot);

			dx = -cubeSize;
			dy = -cubeSize * tanf(rot);

		}
		else if (rot < PI / 2 || rot > 3 * PI / 2) {

			interx = int(rayPos.x) / cubeSize * cubeSize + cubeSize;
			intery = rayPos.y - (rayPos.x - interx) * tanf(rot);

			dx = cubeSize;
			dy = cubeSize * tanf(rot);

		}

		if (rot == PI / 2 || rot == rot > 3 * PI / 2) {
			breakFromLoop = true;
		}

		while (!breakFromLoop) {

			int pointx = (int)interx / cubeSize;
			int pointy = (int)intery / cubeSize;

			if (!curRoom.pointInRoom(pointx, pointy))
				break;

			if (curRoom.getBlock(pointx, pointy).type != blockTypes::NONE) {

				float delX2 = (interx - rayPos.x) * (interx - rayPos.x);
				float delY2 = (intery - rayPos.y) * (intery - rayPos.y);

				float newLenght = sqrtf(delX2 + delY2);

				if (newLenght < shortestLength) {

					shortestLength = newLenght;
					retVal = olc::vd2d(interx, intery);
					side = 1;

				}

				break;

			}

			interx += dx;
			intery += dy;

		}

		return { shortestLength, {retVal,  side} };

	}

	/*************************************** HELPER FUNCTIONS ***************************************/

	bool fixCollisionOnRadius() {

		float posX, posY;

		for (int k = -1; k <= 1; k++) {
			for (int j = -1; j <= 1; j++) {

				posX = player.position.x + player.radius * (j == 0 ? k : k * sqrtof2);
				posY = player.position.y + player.radius * (k == 0 ? j : j * sqrtof2);
				
				if (room.getBlockSizeAdjusted(posX, posY, cubeSize).type != blockTypes::NONE)
					return true;

			}
		}

		return false;

	}

	void fixPlayerToWallCollision(float moveX, float moveY) {

		if (fixCollisionOnRadius()) {
			player.position.x -= moveX;
		}

		if (fixCollisionOnRadius()) {
			player.position.x += moveX;
			player.position.y -= moveY;
		}

		if (fixCollisionOnRadius()) {
			player.position.x -= moveX;
		}

	}

	float fixAngle(float Angle) {

		if (Angle < 0)
			Angle += 2 * PI;

		if (Angle > 2 * PI)
			Angle -= 2 * PI;

		return Angle;

	}

	/*************************************** IMPORTANT FUNCTIONS ***************************************/

	void getInputs(float fElapsedTime) {

		if (GetKey(olc::A).bHeld) {

			player.roation -= DEG * 100 * fElapsedTime;

			if (player.roation < 0)
				player.roation += 2 * PI;

		}

		if (GetKey(olc::D).bHeld) {

			player.roation += DEG * 100 * fElapsedTime;

			if (player.roation > 2 * PI)
				player.roation -= 2 * PI;

		}

		if (GetKey(olc::W).bHeld) {

			float moveX = player.speed * cosf(player.roation) * fElapsedTime;
			float moveY = player.speed * sinf(player.roation) * fElapsedTime;

			player.position.x += moveX;
			player.position.y += moveY;

			fixPlayerToWallCollision(moveX, moveY);
		}

		if (GetKey(olc::S).bHeld) {

			float moveX = -player.speed * cosf(player.roation) * fElapsedTime;
			float moveY = -player.speed * sinf(player.roation) * fElapsedTime;

			player.position.x += moveX;
			player.position.y += moveY;

			fixPlayerToWallCollision(moveX, moveY);

		}

		if (GetKey(olc::SPACE).bPressed) {

			bullets.push_back(Bullet(player.position.x, player.position.y, 7.0f, 20, 20));
			bullets[bullets.size() - 1].direction = olc::vd2d(cosf(player.roation), sinf(player.roation));
			bullets[bullets.size() - 1].textureID = 0;

		}


	}

	void renderScene(int fov, int numberOfRays) {

		float curDeg = DEG * -(fov / 2) + player.roation;

		for (int x = 0; x < numberOfRays; x++) {

			float useDeg = fixAngle(curDeg);
			float degBet = fixAngle(player.roation - useDeg);

			float curDist; bool side;
			olc::vd2d curInter;

			std::pair <float, std::pair<olc::vd2d, bool>> retVal = getRayDistance(room, useDeg);
			curDist = retVal.first, curInter = retVal.second.first, side = retVal.second.second;

			depthBuffer[x] = curDist;

			float curLightMultiplier = (!side ? 1.0f : 0.7f);

			curDist *= cosf(degBet);

			float lineHeight = (cubeSize * ScreenHeight()) / curDist;
			float unclippedLineHeight = lineHeight;

			lineHeight = std::min(lineHeight, (float)ScreenHeight());

			olc::Pixel curColor((float)255 * curLightMultiplier, (float)255 * curLightMultiplier, (float)255 * curLightMultiplier);

			Block& curWall = room.getBlockSizeAdjusted(curInter.x, curInter.y, cubeSize);

			if (curWall.wallTextureID != -1) {

				float texSizeDiffX = (float)cubeSize / (float)room.getWallTexture(curWall.wallTextureID).imageWidth;
				float texSizeDiffY = (float)cubeSize / (float)room.getWallTexture(curWall.wallTextureID).imageHeight;

				float ty_step = ((float)cubeSize / unclippedLineHeight) / texSizeDiffX;
				float ty_offset = (unclippedLineHeight > ScreenHeight() ? (unclippedLineHeight - ScreenHeight()) / 2.0 : 0);

				float ty = ty_step * ty_offset;
				float tx;

				if (!side) {
					tx = ((int)(curInter.x / 2.0f) % cubeSize) / texSizeDiffY;
					if (useDeg > PI) tx = (float)room.getWallTexture(curWall.wallTextureID).imageWidth - tx - 1;
				}
				else {
					tx = ((int)(curInter.y / 2.0f) % cubeSize) / texSizeDiffY;
					if (useDeg > PI / 2 && useDeg < 3 * PI / 2) tx = (float)room.getWallTexture(curWall.wallTextureID).imageWidth - tx - 1;
				}

				for (int y = (ScreenHeight() - lineHeight) / 2; y <= (ScreenHeight() + lineHeight) / 2; y++) {

					olc::Pixel curColor = room.getWallTexture(curWall.wallTextureID).getPixel((int)(tx), (int)(ty));

					curColor.r *= curLightMultiplier;
					curColor.g *= curLightMultiplier;
					curColor.b *= curLightMultiplier;

					Draw(x, y, curColor);

					ty += ty_step;

				}

			}
			else DrawLine(olc::vd2d(x, (ScreenHeight() - lineHeight) / 2), olc::vd2d(x, (ScreenHeight() + lineHeight) / 2), curColor);

			for (int y = (ScreenHeight() + lineHeight) / 2 + 1; y < ScreenHeight(); y++) {

				float dy = y - (ScreenHeight() / 2.0), raFix = cosf(degBet);

				float px = player.position.x;
				float py = player.position.y;

				float magicNum = 158.0f * ((float)ScreenHeight() / 320.0f);

				float tx = px / 2 + cos(useDeg) * magicNum * (cubeSize / 2.0) / dy / raFix;
				float ty = py / 2 + sin(useDeg) * magicNum * (cubeSize / 2.0) / dy / raFix;

				int mpX = tx / (cubeSize / 2.0);
				int mpY = ty / (cubeSize / 2.0);

				Block& curFloorWall = room.getBlock(mpX, mpY);

				int curImgWidth = room.getFloorTexture(curFloorWall.floorTextureID).imageWidth;
				int curImgHeight = room.getFloorTexture(curFloorWall.floorTextureID).imageHeight;

				tx /= (float)cubeSize / (float)curImgWidth;
				ty /= (float)cubeSize / (float)curImgHeight;

				tx = ((int)tx & (curImgWidth - 1));
				ty = ((int)ty & (curImgHeight - 1));

				olc::Pixel curColor = room.getFloorTexture(curWall.floorTextureID).getPixel((int)tx, (int)ty);

				curColor.r *= floorShade;
				curColor.g *= floorShade;
				curColor.b *= floorShade;


				Draw(x, y, curColor);
			}

			for (int y = (ScreenHeight() + lineHeight) / 2; y <= ScreenHeight(); y++) {

				float dy = y - (ScreenHeight() / 2.0), raFix = cosf(degBet);

				float px = player.position.x;
				float py = player.position.y;

				float magicNum = 158.0f * ((float)ScreenHeight() / 320.0f);

				float tx = px / 2 + cos(useDeg) * magicNum * (cubeSize / 2.0) / dy / raFix;
				float ty = py / 2 + sin(useDeg) * magicNum * (cubeSize / 2.0) / dy / raFix;

				int mpX = tx / (cubeSize / 2.0);
				int mpY = ty / (cubeSize / 2.0);

				Block& curCeilWall = room.getBlock(mpX, mpY);

				if (curCeilWall.hasCeil == false)
					continue;

				int curImgWidth = room.getCeilTexture(curCeilWall.ceilTextureID).imageWidth;
				int curImgHeight = room.getCeilTexture(curCeilWall.ceilTextureID).imageHeight;

				tx /= (float)cubeSize / (float)curImgWidth;
				ty /= (float)cubeSize / (float)curImgHeight;

				tx = ((int)tx & (curImgWidth - 1));
				ty = ((int)ty & (curImgHeight - 1));

				olc::Pixel curColor = room.getCeilTexture(curWall.ceilTextureID).getPixel((int)tx, (int)ty);

				curColor.r *= ceilShade;
				curColor.g *= ceilShade;
				curColor.b *= ceilShade;


				Draw(x, ScreenHeight() - y, curColor);
			}

			curDeg += DEG * ((float)fov / (float)numberOfRays);

		}

	}

	void drawSky() {

		float playerRotInDegs = float(player.roation / (2 * PI) * 360.0f);

		for (int y = ScreenHeight() / 2; y >= 0; y--) {
			for (int x = 0; x < ScreenWidth(); x++) {

				float xRotated = playerRotInDegs * (ScreenWidth() / float(128) * 2) + x;
				if (xRotated < 0) { xRotated += 120; } xRotated = (int)xRotated % 120;

				float tx = xRotated * skyTexture.imageWidth / (float)ScreenWidth() * 2;
				float ty = y * skyTexture.imageHeight / (float)ScreenHeight() * 2;

				Draw(x, y, skyTexture.getPixel((int)tx, (int)ty));

			}
		}

	}

	void drawSprite(Sprite curSprite) {

		float dx = curSprite.x - player.position.x;
		float dy = curSprite.y - player.position.y;
		float dz = curSprite.z;

		const float minDist = cubeSize / 2.9f;

		if (dx * dx + dy * dy < minDist * minDist)
			return;

		olc::vd2d curForwardVec = olc::vd2d(cos(player.roation), sin(player.roation));
		olc::vd2d curVecDir = olc::vd2d(-dx, -dy);

		float dotProd = curForwardVec.dot(curVecDir);

		if (dotProd > 0)
			return;

		float csns = cosf(-(player.roation));
		float sns = sinf(-(player.roation));

		float rt1 = dy * csns + dx * sns;
		float rt2 = dx * csns - dy * sns;

		dx = rt1;
		dy = rt2;

		float magicNum = 108.0f / 128.0f * (float)ScreenWidth();

		dx = (dx * magicNum / dy) + (ScreenHeight() / 2.0f);
		dy = (dz * magicNum / dy) + (ScreenWidth() / 2.0f);

		olc::Pixel curCol;

		int tx;
		int ty;

		int txW = room.getSpriteTexture(curSprite.textureID, curSprite.textureSectionID).imageWidth;
		int txH = room.getSpriteTexture(curSprite.textureID, curSprite.textureSectionID).imageHeight;

		float ScaleMult = (80.0f / 256.0f * (float)ScreenWidth()) / rt2;

		float heightScaled = curSprite.height * ScaleMult;
		float widthScaled = curSprite.width * ScaleMult;

		for (int y = 0; y <= heightScaled; y++) {

			if (dy - y < 0 || dy - y > ScreenHeight())
				continue;

			for (int x = 0; x <= widthScaled; x++) {

				int xPlace = dx + x - widthScaled / 2;

				if (xPlace < 0 || xPlace >= ScreenWidth() || rt2 > depthBuffer[xPlace])
					continue;

				depthBuffer[xPlace] = rt2;

				tx = (float)x * (float)room.getSpriteTexture(curSprite.textureID, curSprite.textureSectionID).imageWidth / widthScaled;
				ty = (float)y * (float)room.getSpriteTexture(curSprite.textureID, curSprite.textureSectionID).imageWidth / widthScaled;

				curCol = room.getSpriteTexture(curSprite.textureID, curSprite.textureSectionID).getPixel(txW - tx, txH - ty);

				if (curCol.a != 0)
					Draw(xPlace, dy - y, curCol);

			}
		}

	}

public:

	bool OnUserCreate() override {

		player.position = olc::vd2d(256, 256);

		int szOfRm = 40;

		room = Room(szOfRm, szOfRm);

		room.addWallTexture("Resources/wall.jpg");
		room.addFloorTexture("Resources/floor.png");
		room.addCeilTexture("Resources/ceiling.jpg");

		skyTexture = Texture("Resources/sky.bmp");

		enemies.push_back(Enemy(256, 256, 30, cubeSize * 2, cubeSize * 2));
		enemies[enemies.size() - 1].textureID = 0;

		room.addSpriteTexture("Resources/spainPenguin.png", enemies[enemies.size() - 1].textureSectionID);

		room.addSpriteTexture("Resources/EnergyBall.png", 2);

		for (int k = 0; k < szOfRm * szOfRm; k++) {

			if (level[k] != 0) {

				room.changeBlock(k % szOfRm, k / szOfRm, blockTypes::WALL);
				room.changeWallTexture(k % szOfRm, k / szOfRm, 0);
				room.changeFloorTexture(k % szOfRm, k / szOfRm, 0);
				room.changeCeilTexture(k % szOfRm, k / szOfRm, 0);

			}
			else {

				room.changeWallTexture(k % szOfRm, k / szOfRm, 0);
				room.changeFloorTexture(k % szOfRm, k / szOfRm, 0);
				room.changeCeilTexture(k % szOfRm, k / szOfRm, 0);

				if (k % szOfRm != 2 && k / szOfRm != 2 && k / szOfRm != 3)
					room.getBlock(k % szOfRm, k / szOfRm).hasCeil = false;

			}

		}

		mapData.cubeSize = cubeSize;

		mapData.width = room.getWidth();
		mapData.height = room.getHeight();

		mapData.map = &room.getMap();

		fov = 60;
		numberOfRays = ScreenWidth();

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override {

		getInputs(fElapsedTime);

		//Clear(olc::Pixel(135, 206, 235));

		drawSky();
		renderScene(fov, numberOfRays);

		for (int k = 0; k < enemies.size(); k++) {
			enemies[k].enemyCicle(player, mapData, fElapsedTime);
			drawSprite(enemies[k]);
		}

		for (int k = 0; k < bullets.size(); k++) {

			bool getDestroyed = bullets[k].bulletCicle(player, enemies, mapData, fElapsedTime);

			if (getDestroyed) {

				bullets.erase(bullets.begin() + k);
				k--;

			}
			else drawSprite(bullets[k]);
		
		}

		for (int k = 0; k < enemies.size(); k++) {
			
			if (enemies[k].health <= 0) {

				enemies.erase(enemies.begin() + k);
				k--;

			}

		}

		return true;
	}
};

int main() {

	Example demo;

	if (demo.Construct(screenWidth, screenheight, 5, 5))
		demo.Start();

	return 0;
}

