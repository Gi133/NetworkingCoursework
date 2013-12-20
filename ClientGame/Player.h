#pragma once
#include "stdafx.h"

class Player : public PhysicsActor
{
public:
	Player();
	~Player();

	void ApplyVerticalForce(float dy); // Based on the number handed in, apply a positive or negative force.
	void ApplyHorizontalForce(float dx); // Based on the number handed in, apply a positive of negative force.

	float getVX(){ return _vx; }
	float getVY(){ return _vy; }
	float getMaxVX(){ return _maxVx; }
	float getMaxVY(){ return _maxVy; }

private:
	float _vx, _vy, _maxVx, _maxVy;
	float _speed; // Speed of the object.
};
