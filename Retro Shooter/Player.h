#ifndef PLAYERCLASS_H
#define PLAYERCLASS_H

#include "olcPixelGameEngine.h"

class Player {

public:

	olc::vd2d position;
	float roation = 0.0f;

	float speed = 200.0f;
	float radius = 2;
	
	Player() = default;

};

#endif