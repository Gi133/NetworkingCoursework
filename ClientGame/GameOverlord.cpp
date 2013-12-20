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

GameOverlord::GameOverlord()
{
	_gameState = Initialization;
}

void GameOverlord::GameInit()
{
	theWorld.SetSideBlockers(true, 0.7f);

	_fullScreenActor.SetSprite("Resources/Images/background.jpg");
	theWorld.Add(&_fullScreenActor);

	theSound.SetSoundCallback(this, &GameManager::SoundEnded);

	_bgm = theSound.LoadSample("Resources/Sounds/bgm.mp3", true);
	if (_bgm)
		theSound.PlaySound(_bgm, 0.5f, true, 0);

	_sfx = theSound.LoadSample("Resources/Sounds/sfx.wav", false);

	FindWorldBounds();
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
		theWorld.StopGame();
}

void GameOverlord::OnSetupGame()
{
	// Change background image.
	_fullScreenActor.SetSprite("Resources/Images/gamebackground.jpg");

	_player = new Player();

	// Change game state.
	_gameState = Game;
}

void GameOverlord::OnSetupSpectator()
{

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
}

void GameOverlord::OnSpectator()
{

}