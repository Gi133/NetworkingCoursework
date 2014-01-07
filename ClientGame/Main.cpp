// Required to run the application : http://www.microsoft.com/en-us/download/details.aspx?id=30679
// Required to compile the project : Visual Studio 2012 or Visual Studio 2013

#include "stdafx.h"
#include "GameOverlord.h"


int main(int argc, char* argv[])
{
	// Arguments to be passed into world initialization.
	int _windowWidth = 1024;
	int _windowHeight = 768;
	std::string _windowName = "Networking Coursework Application";
	bool _antiAliasing = true;
	bool _fullScreen = false;
	bool _resizable = false;

	if (!theWorld.Initialize(_windowWidth, _windowHeight, _windowName, _antiAliasing, _fullScreen, _resizable))
	{
		// Something went horribly wrong, call 911 or something.
		// Add anything you want to tell the user that something went bad.
		return 0;
	}

	// Adds the default grid so you can more easily place Actors
	theWorld.Add(new GridActor(), -1);

	//YOUR GAME SETUP CODE HERE

	theWorld.SetGameManager(&theGameOverlord);

	// do all your setup first, because this function won't return until you're exiting
	theWorld.StartGame();

	// any cleanup can go here
	theWorld.Destroy();

	return 0;
}
