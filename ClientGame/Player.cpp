#include "stdafx.h"
#include "Player.h"

Player::Player()
{
	_speed = 100.0f;

	_maxVx = _maxVy = 200.0f;

	// Set sprite and shape
	this->SetColor(1.0f, 0.0f, 0.0f);
	this->SetDrawShape(ADS_Circle);
	this->SetSize(1.0f);
	
	// Set physics related to this object.
	this->SetDensity(0.5f);
	this->SetFriction(0.1f);
	this->SetRestitution(0.2f);
	this->SetShapeType(PhysicsActor::SHAPETYPE_CIRCLE);
	this->SetFixedRotation(true);

	this->SetName("player");

	this->InitPhysics();
	theWorld.Add(this);
}

Player::~Player()
{
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