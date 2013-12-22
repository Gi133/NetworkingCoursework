#include "stdafx.h"
#include "Protocol.h"
#include "GameOverlord.h"

GameOverlord* GameOverlord::_instance = NULL;

GameOverlord& GameOverlord::getInstance()
{
	if (_instance == NULL)
		_instance = new GameOverlord();
	return *_instance;
}

GameOverlord::GameOverlord() :SNAPSHOT_TIME(0.20f), maxSpectatorNumber(MAX_SPECTATOR_NUMBER)
{
	_gameState = Initialization;
}

void GameOverlord::Render()
{
	// Update does not get called as the object is just rendered.
	// So call it here before doing any rendering work.
	Update();
}

void GameOverlord::Update()
{
	switch (_gameState)
	{
	case Initialization:
	{
						   Init();
						   break;
	}
	case Menu:
	{
				 OnMenu();
				 break;
	}
	case SetupGame:
	{
					  OnSetupGame();
					  break;
	}
	case SetupSpectator:
	{
						   OnSetupSpectator();
						   break;
	}
	case Game:
	{
				 OnGame();
				 break;
	}
	case Spectator:
	{
					  OnSpectator();
					  break;
	}
	case Shutdown:
	{
					 OnShutdown();
	}
	}
}

void GameOverlord::FindWorldBounds()
{
	// If using actors and physics then this is not required.
	WorldMax = theCamera.GetWorldMaxVertex();
	WorldMin = theCamera.GetWorldMinVertex();
}

void GameOverlord::Init()
{
	// Initialize any variables and engine stuff you need.
	theWorld.SetupPhysics(Vector2(0.0f, 0.0f));
	theWorld.SetSideBlockers(true, 0.7f);

	_networkService = NULL;

	// Set player pointer to NULL.
	_player = NULL;
	_bot = NULL;

	spectatorNumber = 0;

	// Set full-screen background.
	_fullScreenActor.SetSprite("Resources/Images/menubackground.jpg");
	theWorld.Add(&_fullScreenActor);

	// Register any custom fonts we may require.

	// Change game state.
	_gameState = Menu;
}

void GameOverlord::OnMenu()
{
	// Draw text. (This could also be done using text actors)
	DrawGameText("GAME MODE (Press to select): ", "Console", theCamera.GetWindowWidth() / 3 * 2 - 150, theCamera.GetWindowHeight() / 3, 0.0f);
	DrawGameText("Press G to go into Game Mode. ", "Console", theCamera.GetWindowWidth() / 3 * 2 - 150, theCamera.GetWindowHeight() / 3 + 50, 0.0f);
	DrawGameText("Press S to go into Spectator Mode. ", "Console", theCamera.GetWindowWidth() / 3 * 2 - 150, theCamera.GetWindowHeight() / 3 + 100, 0.0f);
	DrawGameText("Press E to exit. ", "Console", theCamera.GetWindowWidth() / 3 * 2 - 150, theCamera.GetWindowHeight() / 3 + 150, 0.0f);

	// Wait for input.
	if (theInput.IsKeyDown('g'))
		_gameState = SetupGame;
	if (theInput.IsKeyDown('s'))
		_gameState = SetupSpectator;
	if (theInput.IsKeyDown('e'))
		_gameState = Shutdown;
}

void GameOverlord::OnSetupGame()
{
	// Initialize the network.
	_networkService = new NetworkService(false);

	// Change background image.
	_fullScreenActor.SetSprite("Resources/Images/gamebackground.jpg");

	// Set up the snapshot timer.
	timer = theWorld.GetCurrentTimeSeconds();

	_player = new Player();

	// Change game state.
	_gameState = Game;
}

void GameOverlord::OnSetupSpectator()
{
	// Initialize the network.
	_networkService = new NetworkService(true);

	// Change background image.
	_fullScreenActor.SetSprite("Resources/Images/gamebackground.jpg");

	bool connected = false;
	while (!connected)
	{
		DrawGameText("CONNECTING TO GAME SESSION...", "Console", theCamera.GetWindowWidth() / 3 * 2 - 150, theCamera.GetWindowHeight() / 3, 0.0f);

		// Send Handshake to game client.
		if (!_networkService->Send(NETMESSAGE_HANDSHAKE))
			sysLog.Log("Error sending handshake message!");

		// Receive handshake back
		if (!_networkService->Receive())
			sysLog.Log("Error receiving handshake message.");
		else
		{
			// Message received, check if handshake.
			if (_networkService->getReceivedMessage().messageType == NETMESSAGE_HANDSHAKE)
			{
				connected = true;
				sysLog.Log("Connected to game.");
			}
		}

		// You have connected.
	}

	// Set up the snapshot timer.
	timer = theWorld.GetCurrentTimeSeconds();

	_gameState = Spectator;
}

void GameOverlord::OnGame()
{
	// Process Input.
	if (theInput.IsKeyDown('w'))
		_player->ApplyVerticalForce(1.0f);
	if (theInput.IsKeyDown('s'))
		_player->ApplyVerticalForce(-1.0f);
	if (theInput.IsKeyDown('a'))
		_player->ApplyHorizontalForce(-1.0f);
	if (theInput.IsKeyDown('d'))
		_player->ApplyHorizontalForce(1.0f);

	// Check for handshakes and respond.
	if (spectatorNumber < maxSpectatorNumber)
	{
		if (_networkService->Receive())
		{
			if (_networkService->getReceivedMessage().messageType == NETMESSAGE_HANDSHAKE)
			{
				sysLog.Log("Received Handshake, sending reply.");
				_networkService->Send(NETMESSAGE_HANDSHAKE);
				spectatorNumber++;
			}
		}
	}

	// Check if snapshot timer.
	if (CheckSnapshotTime())
	{
		if (spectatorNumber > 0)
		{
			// Pack and send snapshot.
			if (_networkService->Send(NETMESSAGE_UPDATE, _player))
				sysLog.Log("Message sent successfully.");
		}
	}
}

void GameOverlord::OnSpectator()
{
	if (_networkService->Receive())
	{
		sysLog.Log("Message received successfully.");

		// Process Message.
		ProcessMessages();
	}
	else
	{
		// Timeout timer update.

		// Dead reckoning.

		// If timeout, alert user.
	}
}

bool GameOverlord::CheckSnapshotTime()
{
	if (theWorld.GetTimeSinceSeconds(timer) >= SNAPSHOT_TIME)
	{
		// Reset timer.
		timer = theWorld.GetCurrentTimeSeconds();
		return true;
	}
	else
		return false;
}

void GameOverlord::OnShutdown()
{
	if (_networkService)
	{
		_networkService->ShutdownWSA();
		delete (_networkService);
		_networkService = NULL;
	}

	if (_player)
	{
		delete(_player);
		_player = NULL;
	}

	if (_bot)
	{
		delete(_bot);
		_bot = NULL;
	}

	theWorld.StopGame();
}

void GameOverlord::ProcessMessages()
{
	if (_networkService->CheckMessegeRelevant())
	{
		switch (_networkService->getReceivedMessage().messageType)
		{
		case NETMESSAGE_UPDATE:
		{
								  if (_networkService->getReceivedMessage().ID == "Player")
								  {
									  if (_player == NULL)
									  {
										  // Need to create player.
										  _player = new Player(_networkService->getReceivedMessage().position, _networkService->getReceivedMessage().velocity);
									  }
									  else
									  {
										  // Force the player into position.
										  _player->GetBody()->SetTransform(b2Vec2(_networkService->getReceivedMessage().position.X, _networkService->getReceivedMessage().position.Y), 0.0f);
										  _player->GetBody()->SetLinearVelocity(b2Vec2(_networkService->getReceivedMessage().velocity.X, _networkService->getReceivedMessage().velocity.Y));
									  }
								  }

								  else if (_networkService->getReceivedMessage().ID == "Bot")
								  {
									  if (_bot == NULL)
									  {
										  // Need to create player.
										  _bot = new Player(_networkService->getReceivedMessage().position, _networkService->getReceivedMessage().velocity);
									  }
									  else
									  {
										  // Force the player into position.
										  _bot->GetBody()->SetTransform(b2Vec2(_networkService->getReceivedMessage().position.X, _networkService->getReceivedMessage().position.Y), 0.0f);
										  _bot->GetBody()->SetLinearVelocity(b2Vec2(_networkService->getReceivedMessage().velocity.X, _networkService->getReceivedMessage().velocity.Y));
									  }
								  }
		}
		}
	}
	else
	{
		sysLog.Log("Message was irrelevant (newer data already received).");
	}
}