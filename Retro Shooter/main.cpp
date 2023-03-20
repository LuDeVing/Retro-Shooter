#include <iostream>
#include <utility>

#include "Player.h"
#include "map.h"

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define PI 3.14159265359
#define DEG 0.0174533

int level[100] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 1, 1, 1, 1, 1,
	1, 0, 0, 0, 0, 1, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 1, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

class Example : public olc::PixelGameEngine {
public:

	Player player;
	Room room;

	int cubeSize = 64;

	Example() {
		sAppName = "Example";
	}

	std::pair <float, std::pair<olc::vd2d, float>> getRayDistance(Room& curRoom, float rot) {

		olc::vd2d rayPos = olc::vd2d(player.position);
		
		float lightMultiplier;

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

			if (curRoom.getBlock(pointx, pointy) != blockTypes::NONE) {

				float delX2 = (interx - rayPos.x) * (interx - rayPos.x);
				float delY2 = (intery - rayPos.y) * (intery - rayPos.y);
				
				float newLenght = sqrtf(delX2 + delY2);

				if (newLenght < shortestLength) {
					
					shortestLength = newLenght;
					retVal = olc::vd2d(interx, intery);
					lightMultiplier = 1;

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

			if (curRoom.getBlock(pointx, pointy) != blockTypes::NONE) {

				float delX2 = (interx - rayPos.x) * (interx - rayPos.x);
				float delY2 = (intery - rayPos.y) * (intery - rayPos.y);

				float newLenght = sqrtf(delX2 + delY2);

				if (newLenght < shortestLength) {

					shortestLength = newLenght;
					retVal = olc::vd2d(interx, intery);
					lightMultiplier = 0.7f;

				}

				break;

			}

			interx += dx;
			intery += dy;

		}

		return { shortestLength, {retVal,  lightMultiplier} };

	}

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
			player.position.x += player.speed * cosf(player.roation) * fElapsedTime;
			player.position.y += player.speed * sinf(player.roation) * fElapsedTime;
		}

		if (GetKey(olc::S).bHeld) {
			player.position.x -= player.speed * cosf(player.roation) * fElapsedTime;
			player.position.y -= player.speed * sinf(player.roation) * fElapsedTime;
		}

	}

public:

	bool OnUserCreate() override {

		player.position = olc::vd2d(256, 256);
		
		room = Room(10, 10);

		for (int k = 0; k < 100; k++) {
		
			if(level[k] != 0)
				room.changeBlock(k % 10, k / 10, blockTypes::BLANK);
			
		}

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override {

		getInputs(fElapsedTime);
		Clear(olc::Pixel(135, 206, 235));

		int fov = 90;
		int numberOfRays = ScreenWidth();

		float curDeg = DEG * -(fov / 2) + player.roation;

		for (int k = 0; k < numberOfRays; k++) {

			float useDeg = curDeg;

			if (useDeg < 0)
				useDeg += 2 * PI;

			if (useDeg > 2 * PI)
				useDeg -= 2 * PI;

			float degBet = player.roation - useDeg;

			if (degBet < 0)
				degBet += 2 * PI;

			if (degBet > 2 * PI)
				degBet -= 2 * PI;

			float curDist, curLightMultiplier;
			olc::vd2d curInter;

			std::pair <float, std::pair<olc::vd2d, float>> retVal = getRayDistance(room, useDeg);
			curDist = retVal.first, curInter = retVal.second.first, curLightMultiplier = retVal.second.second;

			curDist *= cos(degBet);

			float lineHeight = (cubeSize * ScreenHeight()) / curDist;
			lineHeight = std::min(lineHeight, (float)ScreenHeight());

			olc::Pixel curColor((float)255 * curLightMultiplier, (float)255 * curLightMultiplier, (float)255 * curLightMultiplier);

			DrawLine(olc::vd2d(k, (ScreenHeight() - lineHeight) / 2), olc::vd2d(k, (ScreenHeight() + lineHeight) / 2), curColor);

			curDeg += DEG * ((float)fov / (float)numberOfRays);

		}
		
		return true;
	}
};

int main() {

	Example demo;
	
	if (demo.Construct(256, 240, 4, 4))
		demo.Start();
	
	return 0;
}