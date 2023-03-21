#include <iostream>

#include "Player.h"
#include "map.h"

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define PI 3.14159265359

int level[100] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

class Example : public olc::PixelGameEngine {
public:

	Player player;
	Room room;

	Example() {
		sAppName = "Example";
	}

	float getRayDistance(Room& curRoom, float rot) {

		olc::vd2d rayPos = olc::vd2d(player.position);
		
		float shortestLength = 10000000000;

		int interx, intery;
		float dx, dy;

		bool breakFromLoop = false;

		if (rot > PI) {
			
			intery = rayPos.y / 64 * 64 - 0.0001;
			interx = rayPos.x - (rayPos.y - intery) / tanf(rot);

			dy = -64;
			dx = -64 / tanf(rot);

		}
		else if (rot < PI) {

			intery = rayPos.y / 64 * 64 + 64;
			interx = rayPos.x - (rayPos.y - intery) / tanf(rot);

			dy = 64;
			dx = 64 / tanf(rot);

		} 
		else {
			breakFromLoop = true;
		}

		while (!breakFromLoop) {

			int pointx = interx / 64;
			int pointy = intery / 64;

			if (!curRoom.pointInRoom(pointx, pointy))
				break;

			if (curRoom.getBlock(pointx, pointy) != blockTypes::NONE) {

				float delX2 = (interx - rayPos.x) * (interx - rayPos.x);
				float delY2 = (intery - rayPos.y) * (intery - rayPos.y);
				
				shortestLength = std::min(shortestLength, sqrtf(delX2 + delY2));
				
				break;

			}

			interx += dx;
			intery += dy;
		}

		breakFromLoop = false;

		if (rot > PI / 2 && rot < 3 * PI / 2) {

			intery = rayPos.y / 64 * 64 - 0.0001;
			interx = rayPos.x - (rayPos.x - interx) * tanf(rot);

			dx = -64;
			dy = -64 * tanf(rot);

		}
		else if (rot < PI / 2 || rot > 3 * PI / 2) {

			interx = rayPos.x / 64 * 64 + 64;
			intery = rayPos.y - (rayPos.y - intery) * tanf(rot);

			dx = 64;	
			dy = 64 * tanf(rot);

		}
		else {
			breakFromLoop = true;
		}

		while (!breakFromLoop) {

			int pointx = interx / 64;
			int pointy = intery / 64;

			if (!curRoom.pointInRoom(pointx, pointy))
				break;

			if (curRoom.getBlock(pointx, pointy) != blockTypes::NONE) {

				float delX2 = (interx - rayPos.x) * (interx - rayPos.x);
				float delY2 = (intery - rayPos.y) * (intery - rayPos.y);

				shortestLength = std::min(shortestLength, sqrtf(delX2 + delY2));

				break;

			}

			interx += dx;
			intery += dy;
		}

		return shortestLength;

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

		int fov = 90;
		int numberOfRays = 90;

		std::cout<<getRayDistance(room, 0)<<'\n';

		for (int k = 0; k < numberOfRays; k++) {



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