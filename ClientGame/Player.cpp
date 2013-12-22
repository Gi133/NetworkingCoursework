#include "stdafx.h"
#include "Player.h"

Player::Player(bool bot)
{
	InitVariables();

	// Set sprite and shape
	if (!bot)
	{
		this->SetColor(0.0f, 0.0f, 1.0f);
		_speed = 100.0f;
		this->SetName("Player");
	}
	else
	{
		this->SetColor(1.0f, 0.0f, 0.0f);
		_speed = 90.0f;
		this->SetName("Bot");
	}
	this->SetDrawShape(ADS_Circle);
	this->SetSize(1.0f);
	
	// Set physics related to this object.
	this->SetDensity(0.5f);
	this->SetFriction(0.1f);
	this->SetRestitution(0.2f);
	this->SetShapeType(PhysicsActor::SHAPETYPE_CIRCLE);
	this->SetFixedRotation(true);

	this->InitPhysics();
	theWorld.Add(this);
}

Player::Player(bool bot, Vector2 force, Vector2 position)
{
	InitVariables();

	// Set sprite and shape
	if (!bot)
	{
		this->SetColor(0.0f, 0.0f, 1.0f);
		_speed = 100.0f;
		this->SetName("Player");
	}	
	else
	{
		this->SetColor(1.0f, 0.0f, 0.0f);
		_speed = 90.0f;
		this->SetName("Bot");
	}
		
	this->SetDrawShape(ADS_Circle);
	this->SetSize(1.0f);

	// Set physics related to this object.
	this->SetDensity(0.5f);
	this->SetFriction(0.1f);
	this->SetRestitution(0.2f);
	this->SetShapeType(PhysicsActor::SHAPETYPE_CIRCLE);
	this->SetFixedRotation(true);

	this->SetPosition(position);

	this->SetName("player");

	this->InitPhysics();
	this->ApplyForce(force, 0.0f);
	theWorld.Add(this);
}

Player::~Player()
{
}

void Player::InitVariables()
{
	_maxVx = _maxVy = 200.0f;
	_vx = _vy = 0.0f;

	aiTargetLocked = false;
	aiTimer = theWorld.GetCurrentTimeSeconds();
	aiTargetAcquisitionTimer = 3.0f;
}

void Player::ApplyVerticalForce(float dy)
{
	_vy = dy * _speed * theWorld.GetDT();

	if (_vy > _maxVy)
		_vy = _maxVy;

	this->ApplyForce(Vector2(0.0f, _vy), Vector2(0.0f, 0.0f));
}

void Player::ApplyHorizontalForce(float dx)
{
	_vx = dx * _speed * theWorld.GetDT();

	if (_vx > _maxVx)
		_vx = _maxVx;

	this->ApplyForce(Vector2(_vx, 0.0f), Vector2(0.0f, 0.0f));
}

void Player::AIUpdate(Player* target)
{
	if (!aiTargetLocked) // If not already moving towards the target
	{
		// Lock target. Take target's location, work out the x and y forces required to take you there.
		Vector2 targetPosition = target->GetPosition();
		Vector2 aiPosition = this->GetPosition();
		Vector2 aiTargetVector;
		aiTargetVector.X = targetPosition.X - aiPosition.X;
		aiTargetVector.Y = targetPosition.Y - aiPosition.Y;

		// Adjust forces to move towards target.
		this->ApplyForce(aiTargetVector * _speed, 0.0f);
		aiTargetLocked = true;
	}
	else
	{
		// Target already locked and bot moving to intercept.
		if (theWorld.GetTimeSinceSeconds(aiTimer) >= aiTargetAcquisitionTimer)
		{
			// Reset timer.
			aiTimer = theWorld.GetCurrentTimeSeconds();
			aiTargetLocked = false;
		}			
	}
}