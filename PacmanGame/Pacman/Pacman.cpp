#include "Pacman.h"
#include <time.h>
#include <sstream>
#include <iostream>


Pacman::Pacman(int argc, char* argv[], int munchieCount) : Game(argc, argv), _cPacmanSpeed(0.1f), _cPacmanFrameTime(250)
{
	// Initialise member varivables
	_pacman = new Player();
	_pacman->dead = false;

	this->munchieCount = munchieCount;

	_munchies = new Enemy*[munchieCount];

	// Local variable
	srand(time(NULL));

	int i;
	for (i = 0; i < munchieCount; i++)
	{
		_munchies[i] = new Enemy();
		_munchies[i]->frameCount = rand() % 1;
		_munchies[i]->currentFrameTime = 0;
		_munchies[i]->frameTime = rand() % 500 + 50;
	}

	//Initialise ghost character
	for (i = 0; i < GHOSTCOUNT; i++)
	{
		_ghosts[i] = new MovingEnemy();
		_ghosts[i]->direction = 0;
		_ghosts[i]->speed = 0.2f;
	}

	_cherry = new Enemy();
	_cherry->frameCount = 0;
	_cherry->currentFrameTime = 0;
	_cherry->frameTime = 500;

	_paused = false;
	_pKeyDown = false;
	_isStartup = true;
	_cKeyDown = false;

	_pacman->currentTimeFrame = 0;
	_pacman->direction = 0;
	_pacman->frame = 0;
	_pacman->speedMultiplier = 1.0f;

	_pop = new SoundEffect();
	_backgroundMusic = new SoundEffect();
	_pacmanDeath = new SoundEffect();

	Audio::Initialise();

	// Checks if sound works
	if (!Audio::IsInitialised())
		std::cout << "Audio is not initialsied" << std::endl;

	//Initialise important Game aspects
	Graphics::Initialise(argc, argv, this, 1024, 768, false, 25, 25, "Pacman", 60); // 1024x768
	Input::Initialise();

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

Pacman::~Pacman()
{
	delete _pacman->position;
	delete _pacman->texture;
	delete _pacman->sourceRect;
	delete _pacman;

	for (int i = 0; i < munchieCount; i++)
	{
		delete _munchies[i]->texture;
		delete _munchies[i]->sourceRect;
		delete _munchies[i];
	}
	delete[] _munchies;

	delete _cherry->position;
	delete _cherry->texture;
	delete _cherry->sourceRect;
	delete _cherry;

	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		delete _ghosts[0]->position;
		delete _ghosts[0]->texture;
		delete _ghosts[0]->sourceRect;
	}
	delete _ghosts;

	delete _pop;
	delete _backgroundMusic;
	delete _pacmanDeath;
}

void Pacman::LoadContent()
{
	// Load Pacman
	_pacman->texture = new Texture2D();
	_pacman->texture->Load("Textures/Pacman.tga", false);
	_pacman->position = new Vector2(350.0f, 350.0f);
	_pacman->sourceRect = new Rect(0.0f, 0.0f, 32, 32);

	// Load Munchie

	Texture2D* munchieTex = new Texture2D();
	munchieTex->Load("Textures/Munchie.png", false);
	for (int i = 0; i < munchieCount; i++)
	{
		_munchies[i]->texture = new Texture2D();
		_munchies[i]->texture = munchieTex;
		_munchies[i]->position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
		_munchies[i]->sourceRect = new Rect(12.0f, 12.0f, 12, 12);
	}

	Texture2D* cherryTex = new Texture2D();
	cherryTex->Load("Textures/Cherry.png", false);
	_cherry->texture = new Texture2D();
	_cherry->texture = cherryTex;
	_cherry->position = new Vector2(150.0f, 150.0f);
	_cherry->sourceRect = new Rect(0.0f, 0.0f, 32, 32);

	// Initialise ghost character
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		_ghosts[i]->texture = new Texture2D();
		if (i == 0)
			_ghosts[i]->texture->Load("Textures/GhostRed.png", false);
		else if (i == 1)
			_ghosts[i]->texture->Load("Textures/GhostPink.png", false);
		else if (i == 2)
			_ghosts[i]->texture->Load("Textures/GhostBlue.png", false);
		else
			_ghosts[i]->texture->Load("Textures/GhostOrange.png", false);
		_ghosts[i]->position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight() - 20));
		_ghosts[i]->sourceRect = new Rect(0.0f, 0.0f, 20, 20);
	}

	_backgroundMusic->Load("Sounds/background.wav");
	if (!_backgroundMusic->IsLoaded())
		std::cout << "_backgroundMusic effect sound effect has not loaded" << std::endl;

	_pop->Load("Sounds/pop.wav");
	if (!_pop->IsLoaded())
		std::cout << "_pop member sound effect has not loaded" << std::endl;

	_pacmanDeath->Load("Sounds/pacman_death.wav");
	if (!_pacmanDeath->IsLoaded())
		std::cout << "_pacmanDeath sound effect has not loaded" << std::endl;

	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f);

	// Set Menu Parameters
	_menuBackground = new Texture2D();
	_menuBackground->Load("Textures/Transparency.png", false);
	_menuRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_menuStringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);

	// Set Startup Parameters
	_startupBackground = new Texture2D();
	_startupBackground->Load("Textures/Startup.png", false);
	_startupRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_startupStringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);
}

void Pacman::Update(int elapsedTime)
{
	// Gets the current state of the keyboard
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();
	Input::MouseState* mouseState = Input::Mouse::GetState();

	CheckPaused(keyboardState, Input::Keys::P);

	if (!_paused && !_isStartup)
	{
		Input(elapsedTime, keyboardState, mouseState);
		UpdatePacman(elapsedTime);
		CheckGhostCollision();

		for (int i = 0; i < munchieCount; i++)
		{
			UpdateMunchie(_munchies[i], elapsedTime);
		}
		UpdateMunchie(_cherry, elapsedTime);
		CheckViewportCollision();

		for (int i = 0; i < GHOSTCOUNT; i++)
		{
			UpdateGhost(_ghosts[i], elapsedTime);
		}
	}
}

void Pacman::Input(int elapsedTime, Input::KeyboardState* keyboardState, Input::MouseState* mouseState)
{
	float pacmanSpeed = _cPacmanSpeed * elapsedTime * _pacman->speedMultiplier;

	if (!_pacman->dead)
	{
		if (keyboardState->IsKeyDown(Input::Keys::D))
		{
			_pacman->position->X += pacmanSpeed; //Moves Pacman
			_pacman->direction = 0;
		}

		else if (keyboardState->IsKeyDown(Input::Keys::A))
		{
			_pacman->position->X -= pacmanSpeed; //Moves Pacman left
			_pacman->direction = 2;
		}

		else if (keyboardState->IsKeyDown(Input::Keys::W))
		{
			_pacman->position->Y -= pacmanSpeed; //Moves Pacman up
			_pacman->direction = 3;
		}

		else if (keyboardState->IsKeyDown(Input::Keys::S))
		{
			_pacman->position->Y += pacmanSpeed; //Moves Pacman down
			_pacman->direction = 1;
		}

		if (keyboardState->IsKeyDown(Input::Keys::LEFTSHIFT))
			_pacman->speedMultiplier = 2.0f;
		else
		{
			_pacman->speedMultiplier = 1.0f;
		}

		if (keyboardState->IsKeyDown(Input::Keys::R) && _cKeyDown == false)
		{
			_cherry->position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
			_cKeyDown = true;
		}

		else if (keyboardState->IsKeyUp(Input::Keys::R) && _cKeyDown == true)
			_cKeyDown = false;

		if (mouseState->LeftButton == Input::ButtonState::PRESSED)
		{
			_cherry->position->X = mouseState->X;
			_cherry->position->Y = mouseState->Y;
		}
	}
}

void Pacman::CheckPaused(Input::KeyboardState * state, Input::Keys pauseKey)
{
	if (state->IsKeyDown(Input::Keys::SPACE) && _isStartup == true)
	{
		_isStartup = false;
		Audio::Play(_backgroundMusic);
	}

	if (state->IsKeyDown(Input::Keys::P) && !_pKeyDown && !_isStartup)
	{
		_pKeyDown = true;
		_paused = !_paused;
	}

	if (state->IsKeyUp(Input::Keys::P))
		_pKeyDown = false;
}

void Pacman::CheckViewportCollision()
{
	if (_pacman->position->X + _pacman->sourceRect->Width > Graphics::GetViewportWidth())
	{
		_pacman->position->X = 32 - _pacman->sourceRect->Width; // Pacman hit right wall - reset his position
	}

	else if (_pacman->position->X + _pacman->sourceRect->Width < 32)
	{
		_pacman->position->X = Graphics::GetViewportWidth() - _pacman->sourceRect->Width; // Pacman hit left wall - reset his position
	}

	else if (_pacman->position->Y + _pacman->sourceRect->Height > Graphics::GetViewportHeight())
	{
		_pacman->position->Y = 32 - _pacman->sourceRect->Height; // Pacman hit bottom wall - reset his position
	}

	else if (_pacman->position->Y + _pacman->sourceRect->Height < 32)
	{
		_pacman->position->Y = Graphics::GetViewportHeight() - _pacman->sourceRect->Height; // Pacman hit top wall - reset his position
	}
}

void Pacman::UpdatePacman(int elaspedTime)
{
	_pacman->currentTimeFrame += elaspedTime;

	if (_pacman->currentTimeFrame > _cPacmanFrameTime)
	{
		_pacman->frame++;

		if (_pacman->frame >= 2)
			_pacman->frame = 0;

		_pacman->currentTimeFrame = 0;
	}

	_pacman->sourceRect->Y = _pacman->sourceRect->Height * _pacman->direction;
	_pacman->sourceRect->X = _pacman->sourceRect->Width * _pacman->frame;
}

void Pacman::UpdateMunchie(Enemy*& refMunchie, int elapsedTime)
{
	for (int i = 0; i < munchieCount; i++)
	{
		if (!_paused)
			_munchies[i]->currentFrameTime += elapsedTime;

		if (_munchies[i]->currentFrameTime > _munchies[i]->frameTime)
		{
			_munchies[i]->frameCount++;

			if (_munchies[i]->frameCount >= 2)
				_munchies[i]->frameCount = 0;

			_munchies[i]->currentFrameTime = 0;
		}

		_munchies[i]->sourceRect->X = _munchies[i]->sourceRect->Width * _munchies[i]->frameCount;

	}

	if (!_paused)
		_cherry->currentFrameTime += elapsedTime;

	if (_cherry->currentFrameTime > _cherry->frameTime)
	{
		_cherry->frameCount++;

		if (_cherry->frameCount >= 2)
			_cherry->frameCount = 0;

		_cherry->currentFrameTime = 0;
	}
	_cherry->sourceRect->X = _cherry->sourceRect->Width * _cherry->frameCount;

	// Collision with munchie
	int i = 0;
	int bottom1 = _pacman->position->Y + _pacman->sourceRect->Height;
	int bottom2 = 0;
	int left1 = _pacman->position->X;
	int left2 = 0;
	int right1 = _pacman->position->X + _pacman->sourceRect->Width;
	int right2 = 0;
	int top1 = _pacman->position->Y;
	int top2 = 0;

	for (i = 0; i < munchieCount; i++)
	{
		// Populate variables with Ghost data
		bottom2 = _munchies[i]->position->Y + _munchies[i]->sourceRect->Height;
		left2 = _munchies[i]->position->X;
		right2 = _munchies[i]->position->X + _munchies[i]->sourceRect->Width;
		top2 = _munchies[i]->position->Y;

		if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
		{
			Audio::Play(_pop);
			_munchies[i]->position = new Vector2(-150.0f, -150.0f);
		}
	}
}

void Pacman::UpdateGhost(MovingEnemy* ghost, int elapsedTime)
{

	if (ghost->direction == 0) // Moves Right
		ghost->position->X += ghost->speed * elapsedTime;

	else if (ghost->direction == 1) // Moves Left
		ghost->position->X -= ghost->speed * elapsedTime;

	else if (ghost->direction == 2) // Moves Up?
		ghost->position->Y -= ghost->speed * elapsedTime;

	else if (ghost->direction == 3) // Moves Down?
		ghost->position->Y += ghost->speed * elapsedTime;


	if (ghost->position->X + ghost->sourceRect->Width >= Graphics::GetViewportWidth()) // Hits right edge
	{
		ghost->direction = 1; // Change direction
		ghost->sourceRect->X = ghost->sourceRect->Width * ghost->direction;
	}

	else if (ghost->position->X <= 0) // Hits Left Edge
	{
		ghost->direction = 0; // Change direction
		ghost->sourceRect->X = ghost->sourceRect->Width * ghost->direction;
	}

	if (ghost->position->Y + ghost->sourceRect->Width >= Graphics::GetViewportHeight()) // Hits bottom edge
	{
		ghost->direction = 2; // Change direction
		ghost->sourceRect->Y = ghost->sourceRect->Width * ghost->direction;
	}

	else if (ghost->position->Y <= 0) // Hits Left Edge
	{
		ghost->direction = 3; // Change direction
		ghost->sourceRect->Y = ghost->sourceRect->Width * ghost->direction;
	}
}

void Pacman::CheckGhostCollision()
{
	// Local variables
	int i = 0;
	int bottom1 = _pacman->position->Y + _pacman->sourceRect->Height;
	int bottom2 = 0;
	int left1 = _pacman->position->X;
	int left2 = 0;
	int right1 = _pacman->position->X + _pacman->sourceRect->Width;
	int right2 = 0;
	int top1 = _pacman->position->Y;
	int top2 = 0;

	for (i = 0; i < GHOSTCOUNT; i++)
	{
		// Populate variables with Ghost data
		bottom2 = _ghosts[i]->position->Y + _ghosts[i]->sourceRect->Height;
		left2 = _ghosts[i]->position->X;
		right2 = _ghosts[i]->position->X + _ghosts[i]->sourceRect->Width;
		top2 = _ghosts[i]->position->Y;

		if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
		{
			if (_pacman->dead == false)
				Audio::Play(_pacmanDeath);
			_pacman->dead = true;
			i = GHOSTCOUNT;
		}
	}
}

void Pacman::Draw(int elapsedTime)
{
	// Allows us to easily create a string
	std::stringstream stream;
	stream << "Pacman X: " << _pacman->position->X << " Y: " << _pacman->position->Y;

	SpriteBatch::BeginDraw(); // Starts Drawing
	if (!_pacman->dead)
	{
		SpriteBatch::Draw(_pacman->texture, _pacman->position, _pacman->sourceRect); // Draws Pacman
	}

	for (int i = 0; i < munchieCount; i++)
	{
		if (_munchies[i]->frameCount == 0)
		{
			// Draws Red Munchie
			SpriteBatch::Draw(_munchies[i]->texture, _munchies[i]->position, _munchies[i]->sourceRect, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);

		}
		else
		{
			// Draw Blue Munchie
			SpriteBatch::Draw(_munchies[i]->texture, _munchies[i]->position, _munchies[i]->sourceRect, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);

			if (_munchies[i]->frameCount >= 60)
				_munchies[i]->frameCount = 0;
		}
	}

	if (_cherry->frameCount == 0)
		SpriteBatch::Draw(_cherry->texture, _cherry->position, _cherry->sourceRect, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);
	else
	{
		SpriteBatch::Draw(_cherry->texture, _cherry->position, _cherry->sourceRect, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);

		if (_cherry->frameCount >= 60)
			_cherry->frameCount = 0;
	}

	//Draw ghosts
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		SpriteBatch::Draw(_ghosts[i]->texture, _ghosts[i]->position, _ghosts[i]->sourceRect);
	}

	if (_isStartup)
	{
		std::stringstream startupStream;
		startupStream << "PRESS SPACE TO START!";

		SpriteBatch::Draw(_startupBackground, _startupRectangle, nullptr);
		SpriteBatch::DrawString(startupStream.str().c_str(), _startupStringPosition, Color::Red);
	}

	if (_paused)
	{
		std::stringstream menuStream;
		menuStream << "PAUSED!";

		SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Red);
	}

	// Draws String
	SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);
	SpriteBatch::EndDraw(); // Ends Drawing
}

