#pragma once

// If Windows and not in Debug, this will run without a console window
// You can use this to output information when debugging using cout or cerr
#ifdef WIN32 
#ifndef _DEBUG
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif
#endif

#define MUNCHIECOUNT 50
#define GHOSTCOUNT 4

// Just need to include main header file
#include "S2D/S2D.h"

// Reduces the amount of typing by including all classes in S2D namespace
using namespace S2D;

// Structure Definition
struct MovingEnemy
{
	Vector2* position;
	Texture2D* texture;
	Rect* sourceRect;
	int direction;
	float speed;
};

struct Player
{
	bool dead;
	float speedMultiplier;
	int currentTimeFrame;
	int direction;
	int frame;
	Rect* sourceRect;
	Texture2D* texture;
	Vector2* position;
};

struct Enemy
{
	int frameCount;
	int frameTime;
	Vector2* position;
	Rect* sourceRect;
	Texture2D* texture;
	int currentFrameTime;
};

// Declares the Pacman class which inherits from the Game class.
// This allows us to overload the Game class methods to help us
// load content, draw and update our game.
class Pacman : public Game
{
private:
	// Functions
	void Input(int elapsedTime, Input::KeyboardState* state, Input::MouseState* mouseState);
	void CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey);
	void CheckViewportCollision();
	void UpdatePacman(int elapsedTime);
	void UpdateMunchie(Enemy*& refMunchie, int elapsedTime);
	void CheckGhostCollision();
	void UpdateGhost(MovingEnemy*, int elapsedTime);

	// Constant data for game variables
	const float _cPacmanSpeed;

	// Data for menu
	Texture2D* _menuBackground;
	Rect* _menuRectangle;
	Vector2* _menuStringPosition;
	bool _paused;
	bool _pKeyDown;
	bool _cKeyDown;

	// Data for startup
	Texture2D* _startupBackground;
	Rect* _startupRectangle;
	Vector2* _startupStringPosition;
	bool _isStartup; // Is Pacman starting up for the first time

	// Data to represent Pacman
	Player* _pacman;
	const int _cPacmanFrameTime;
	int _playerScore;
	bool _isPlayerMetal; // Pill powerup
	int _playerLives;

	// Data to represent Munchie
	Enemy** _munchies;
	int munchieCount;
	Enemy* _cherry;
	MovingEnemy* _ghosts[GHOSTCOUNT];
	bool _startupGhostDirection;
	bool _cherryPowerUpActive;
	int _cherryPowerUpTime;
	Enemy* _pill;

	// Data to represet moveable box
	Enemy* _playerBox;
	bool _playerInsideBox;

	// Position for String
	Vector2* _stringPosition;

	// Sounds
	SoundEffect* _backgroundMusic;
	SoundEffect* _pop;
	SoundEffect* _pacmanDeath;
	SoundEffect* _metalClang;

public:
	/// <summary> Constructs the Pacman class. </summary>
	Pacman(int argc, char* argv[], int munchieCount);

	/// <summary> Destroys any data associated with Pacman class. </summary>
	virtual ~Pacman();

	/// <summary> All content should be loaded in this method. </summary>
	void virtual LoadContent();

	/// <summary> Called every frame - update game logic here. </summary>
	void virtual Update(int elapsedTime);

	/// <summary> Called every frame - draw game here. </summary>
	void virtual Draw(int elapsedTime);
};
