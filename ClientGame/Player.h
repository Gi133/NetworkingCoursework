#pragma once
#include "stdafx.h"

class Player : public PhysicsActor
{
public:
	Player(bool bot);
	Player(bool bot, Vector2 position, Vector2 force);
	~Player();

	void ApplyVerticalForce(float dy); // Based on the number handed in, apply a positive or negative force.
	void ApplyHorizontalForce(float dx); // Based on the number handed in, apply a positive of negative force.

	float getVX(){ return _vx; }
	float getVY(){ return _vy; }
	float getMaxVX(){ return _maxVx; }
	float getMaxVY(){ return _maxVy; }

	// AI Functions.
	void AIUpdate(Player* target);

private:
	void InitVariables();

	float _vx, _vy, _maxVx, _maxVy;
	float _speed; // Speed of the object.

	bool aiTargetLocked;
	float aiTimer;
	float aiTargetAcquisitionTimer;
};
