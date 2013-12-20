#pragma once

#include <vector>
#include <string>

#include "stdafx.h"
#include "Player.h"
#include "NetworkService.h"

#define theGameOverlord GameOverlord::getInstance()

class GameOverlord :
	public GameManager
{
public:
	static GameOverlord& getInstance();
	virtual void Render();
	virtual void Update();

protected:
	GameOverlord();
	static GameOverlord* _instance;

private:
	// Functions
	void Init(); // Initialize main window.
	void OnMenu(); // Function to be executed while on menu state.
	void OnSetupGame(); // Function to initialize the game portion of the application.
	void OnSetupSpectator(); // Function to initialize the spectator portion of the application.
	void OnGame();
	void OnSpectator();
	void OnShutdown();

	bool CheckSnapshotTime(); // Returns true if it's time to send the snapshot.

	void FindWorldBounds(); // Find the max values for X and Y so that they can be used in collision detection.

	FullScreenActor _fullScreenActor;
	AngelSampleHandle _bgm, _sfx;

	NetworkService* _networkService;

	Player* _player;

	const float SNAPSHOT_TIME; // Update time (20ms)
	float timer, snapshotTime;

	enum gameState{Initialization, Menu, SetupGame, SetupSpectator, Game, Spectator, Shutdown} _gameState;

	Vector2 WorldMax, WorldMin;
};