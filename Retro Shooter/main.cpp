#include <iostream>
#include <utility>

#define STB_IMAGE_IMPLEMENTATION

#include "Player.h"
#include "map.h"

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define PI 3.14159265359
#define DEG 0.0174533

int level[400] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
	1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
	1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

class Example : public olc::PixelGameEngine {
public:

	Player player;
	Room room;

	const int cubeSize = 64;

	int fov;
	int numberOfRays;

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

		olc::vd2d retVal(-10000000000,  -10000000000);

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

		if(rot == 0 || rot == PI) {
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
		
		if(rot == PI / 2 || rot == rot > 3 * PI / 2) {
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

	void fixPlayerToWallCollision(float moveX, float moveY) {

		if (room.getBlockSizeAdjusted(player.position.x, player.position.y, cubeSize).type != blockTypes::NONE) {
			player.position.x -= moveX;
		}

		if (room.getBlockSizeAdjusted(player.position.x, player.position.y, cubeSize).type != blockTypes::NONE) {
			player.position.x += moveX;
			player.position.y -= moveY;
		}

		if (room.getBlockSizeAdjusted(player.position.x, player.position.y, cubeSize).type != blockTypes::NONE) {
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

			if (curWall.floorTextureID != -1) {

				for (int y = (ScreenHeight() + lineHeight) / 2 + 1; y < ScreenHeight(); y++) {

					float dy = y - (ScreenHeight() / 2.0), raFix = cosf(degBet);

					float px = player.position.x;
					float py = player.position.y;

					float magicNum = 158.0f * ((float)ScreenHeight() / 320.0f);

					float tx = px / 2 + cos(useDeg) * magicNum * (cubeSize / 2.0) / dy / raFix;
					float ty = py / 2 + sin(useDeg) * magicNum * (cubeSize / 2.0) / dy / raFix;

					int curImgWidth = room.getFloorTexture(curWall.floorTextureID).imageWidth;
					int curImgHeight = room.getFloorTexture(curWall.floorTextureID).imageHeight;

					tx /= (float)cubeSize / (float)curImgWidth;
					ty /= (float)cubeSize / (float)curImgHeight;

					tx = ((int)tx & (curImgWidth - 1));
					ty = ((int)ty & (curImgHeight - 1));

					olc::Pixel curColor = room.getFloorTexture(curWall.floorTextureID).getPixel((int)tx, (int)ty);

					Draw(x, y, curColor);
				}
			}

			if (curWall.hasCeil) {
				if (curWall.ceilTextureID != -1) {

					for (int y = (ScreenHeight() + lineHeight) / 2 + 1; y < ScreenHeight(); y++) {

						float dy = y - (ScreenHeight() / 2.0), raFix = cosf(degBet);

						float px = player.position.x;
						float py = player.position.y;

						float magicNum = 158.0f * ((float)ScreenHeight() / 320.0f);

						float tx = px / 2 + cos(useDeg) * magicNum * (cubeSize / 2.0) / dy / raFix;
						float ty = py / 2 + sin(useDeg) * magicNum * (cubeSize / 2.0) / dy / raFix;

						int curImgWidth = room.getCeilTexture(curWall.ceilTextureID).imageWidth;
						int curImgHeight = room.getCeilTexture(curWall.ceilTextureID).imageHeight;

						tx /= (float)cubeSize / (float)curImgWidth;
						ty /= (float)cubeSize / (float)curImgHeight;

						tx = ((int)tx & (curImgWidth - 1));
						ty = ((int)ty & (curImgHeight - 1));

						olc::Pixel curColor = room.getCeilTexture(curWall.ceilTextureID).getPixel((int)tx, (int)ty);

						Draw(x, ScreenHeight() - y, curColor);
					}
				}
			}

			curDeg += DEG * ((float)fov / (float)numberOfRays);

		}

	}

public:

	bool OnUserCreate() override {

		player.position = olc::vd2d(256, 256);
		
		room = Room(20, 20);

		room.addWallTexture("Resourses/wall.jpg");
		room.addFloorTexture("Resourses/floor.png");
		room.addCeilTexture("Resourses/ceiling.jpg");

		for (int k = 0; k < 400; k++) {
		
			if (level[k] != 0) {
				room.changeBlock(k % 20, k / 20, blockTypes::WALL);
				room.changeWallTexture(k % 20, k / 20, 0);
				room.changeFloorTexture(k % 20, k / 20, 0);
				room.changeCeilTexture(k % 20, k / 20, 0);
			}

		}

		fov = 60;
		numberOfRays = ScreenWidth();

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override {

		getInputs(fElapsedTime);
		Clear(olc::Pixel(135, 206, 235));

		

		renderScene(fov, numberOfRays);
		
		return true;
	}
};

int main() {

	Example demo;
	
	if (demo.Construct(256, 240, 4, 4))
		demo.Start();

	return 0;
}