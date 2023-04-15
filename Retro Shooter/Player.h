#ifndef PLAYERCLASS_H
#define PLAYERCLASS_H

#include "olcPixelGameEngine.h"

static enum GunState { STATIC, SHOOTING, COOLING };

class Player {

public:

	float health = 100.0f;
	float maxHealth = 100.0f;

	olc::vf2d position;
	float roation = 0.0f;

	float speed = 300.0f;
	float radius = 2;

	float entityRadius = 7.0f;

	int bulletTagID = 1;

	GunState gunState = GunState::STATIC;
	int gunID = 0;

	Player() = default;

};

#endif