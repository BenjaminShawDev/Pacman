#include "Pacman.h"
#include <time.h>
#include <sstream>
#include <iostream>

Pacman::Pacman(int argc, char* argv[], int munchieCount) : Game(argc, argv), _cPacmanSpeed(0.2f), _cPacmanFrameTime(250)
{
	while (_gameLoop)
	{
		// Initialise member varivables
		_pacman = new Player();
		_pacman->dead = false;

		this->munchieCount = munchieCount;

		_munchies = new Enemy * [munchieCount];

		// Local variable
		for (int i = 0; i < munchieCount; i++)
		{
			_munchies[i] = new Enemy();
			_munchies[i]->frameCount = 0;
			_munchies[i]->currentFrameTime = 0;
			_munchies[i]->frameTime = 5000;
		}

		//Initialise ghost character
		for (int i = 0; i < GHOSTCOUNT; i++)
		{
			_ghosts[i] = new MovingEnemy();
			_ghosts[i]->direction = 0;
			_ghosts[i]->speed = 1.0f;
		}

		_cherry = new Enemy();
		_cherry->frameCount = 0;
		_cherry->currentFrameTime = 0;
		_cherry->frameTime = 5000;

		_pill = new Enemy();

		_playerBox = new Enemy();

		_paused = false;
		_pKeyDown = false;
		_isStartup = true;
		_cKeyDown = false;
		_startupGhostDirection = true;
		_powerUpActive = false;
		_isPlayerMetal = false;
		_hasPlayerMoved = false;
		_playerWon = false;
		_playerScore = 0;
		_powerUpTime = 0;
		_timeBetweenBoxCols = 0;

		_pacman->currentTimeFrame = 0;
		_pacman->direction = 0;
		_pacman->frame = 0;
		_pacman->speedMultiplier = 1.0f;
		_playerLives = 3;

		_pop = new SoundEffect();
		_backgroundMusic = new SoundEffect();
		_pacmanDeath = new SoundEffect();
		_metalClang = new SoundEffect();
		_eatCherry = new SoundEffect();
		_screenWrapAround = new SoundEffect();

		Audio::Initialise();

		// Checks if sound works
		if (!Audio::IsInitialised())
			std::cout << "Audio is not initialsied" << std::endl;

		srand(time(NULL));

		//Initialise important Game aspects
		Graphics::Initialise(argc, argv, this, 1920, 1080, true, 25, 25, "Pacman", 60);
		Input::Initialise();

		// Start the Game Loop - This calls Update and Draw in game loop
		Graphics::StartGameLoop();
	}
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

	delete _pill->position;
	delete _pill->texture;
	delete _pill->sourceRect;
	delete _pill;

	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		delete _ghosts[0]->position;
		delete _ghosts[0]->texture;
		delete _ghosts[0]->sourceRect;
	}
	delete _ghosts;

	// Players box
	delete _playerBox->position;
	delete _playerBox->texture;
	delete _playerBox->sourceRect;
	delete _playerBox;

	// Sounds
	delete _pop;
	delete _backgroundMusic;
	delete _pacmanDeath;
	delete _metalClang;
	delete _eatCherry;
	delete _screenWrapAround;
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
		_munchies[i]->position = new Vector2(rand() % 1900 + 10, rand() % 1050 + 10);
		for (int j = 1; j <= i; j++)
		{
			// Stop munchies overlapping
			if ((_munchies[i]->position->Y - _munchies[j]->position->Y) < 100 || (_munchies[i]->position->Y - _munchies[j]->position->Y) < -100 || (_munchies[i]->position->X - _munchies[j]->position->X) < 100 || (_munchies[i]->position->X - _munchies[j]->position->X) < -100)
				_munchies[i]->position = new Vector2(rand() % 1900 + 10, rand() % 1050 + 10);
		}
		_munchies[i]->sourceRect = new Rect(12.0f, 12.0f, 12, 12);
	}

	Texture2D* cherryTex = new Texture2D();
	cherryTex->Load("Textures/Cherry.png", false);
	_cherry->texture = new Texture2D();
	_cherry->texture = cherryTex;
	_cherry->position = new Vector2(Graphics::GetViewportWidth() - 100, Graphics::GetViewportHeight() - 100);
	_cherry->sourceRect = new Rect(0.0f, 0.0f, 32, 32);

	Texture2D* pillTex = new Texture2D();
	pillTex->Load("Textures/Pill.png", false);
	_pill->texture = new Texture2D();
	_pill->texture = pillTex;
	_pill->position = new Vector2(Graphics::GetViewportWidth()/2, Graphics::GetViewportHeight()/2);
	_pill->sourceRect = new Rect(0.0f, 0.0f, 24, 24);

	// Load moveable box
	Texture2D* boxTex = new Texture2D();
	boxTex->Load("Textures/Box.png", false);
	_playerBox->texture = new Texture2D();
	_playerBox->texture = boxTex;
	_playerBox->position = new Vector2(350.0f, 350.0f);
	_playerBox->sourceRect = new Rect(0.0f, 0.0f, 32, 32);

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
		else if (i == 3)
			_ghosts[i]->texture->Load("Textures/GhostOrange.png", false);
		_ghosts[i]->position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight() - 20));
		_ghosts[i]->sourceRect = new Rect(0.0f, 0.0f, 20, 20);
	}

	_backgroundMusic->Load("Sounds/background.wav");
	if (!_backgroundMusic->IsLoaded())
		cout << "_backgroundMusic effect sound effect has not loaded" << endl;

	_pop->Load("Sounds/pop.wav");
	if (!_pop->IsLoaded())
		cout << "_pop member sound effect has not loaded" << endl;

	_pacmanDeath->Load("Sounds/pacman_death.wav");
	if (!_pacmanDeath->IsLoaded())
		cout << "_pacmanDeath sound effect has not loaded" << endl;

	_metalClang->Load("Sounds/metal_clang.wav");
	if (!_metalClang->IsLoaded())
		cout << "_pacmanDeath sound effect has not loaded" << endl;

	_eatCherry->Load("Sounds/pacman_eatfruit.wav");
	if (!_eatCherry->IsLoaded())
		cout << "_eatCherry sound effect has not loaded" << endl;

	_screenWrapAround->Load("Sounds/screen_wrap.wav");
	if (!_screenWrapAround->IsLoaded())
		cout << "_screenWrapAround sound effect has not loaded" << endl;

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

	// Set Endscreen Parameters
	_endScreenBackground = new Texture2D();
	_endScreenBackground->Load("Textures/Endscreen.png", false);
	_endScreenRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());

	// Set Game Over Paramters
	_gameOverBackground = new Texture2D();
	_gameOverBackground->Load("Textures/Gameover.png", false);
	_gameOverRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
}

void Pacman::Update(int elapsedTime)
{
	// Gets the current state of the keyboard
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();
	Input::MouseState* mouseState = Input::Mouse::GetState();

	CheckPaused(keyboardState, Input::Keys::P);

	Input(elapsedTime, keyboardState, mouseState);

	if (!_paused)
	{
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

		if (_playerScore == munchieCount)
		{
			_playerWon = true;
		}
	}
}

void Pacman::Input(int elapsedTime, Input::KeyboardState* keyboardState, Input::MouseState* mouseState)
{
	float pacmanSpeed = _cPacmanSpeed * elapsedTime * _pacman->speedMultiplier;

	if (keyboardState->IsKeyDown(Input::Keys::ESCAPE))
		exit(0);

	if (!_isStartup && !_paused && !_pacman->dead && !_playerWon)
	{
		if (keyboardState->IsKeyDown(Input::Keys::D))
		{
			// Moves Pacman Right
			_pacman->position->X += pacmanSpeed;
			_pacman->direction = 0;
			_hasPlayerMoved = true;
		}

		else if (keyboardState->IsKeyDown(Input::Keys::A))
		{
			// Moves Pacman left
			_pacman->position->X -= pacmanSpeed;
			_pacman->direction = 2;
			_hasPlayerMoved = true;
		}

		else if (keyboardState->IsKeyDown(Input::Keys::W))
		{
			// Moves Pacman up
			_pacman->position->Y -= pacmanSpeed;
			_pacman->direction = 3;
			_hasPlayerMoved = true;
		}

		else if (keyboardState->IsKeyDown(Input::Keys::S))
		{
			// Moves Pacman down
			_pacman->position->Y += pacmanSpeed;
			_pacman->direction = 1;
			_hasPlayerMoved = true;
		}

		if (!_hasPlayerMoved)
		{
			_powerUpTime = 3500;
			_isPlayerMetal = true;
		}

		_playerBox->position->X = mouseState->X - 16;
		_playerBox->position->Y = mouseState->Y - 16;
	}
}

void Pacman::CheckPaused(Input::KeyboardState * state, Input::Keys pauseKey)
{
	if (state->IsKeyDown(Input::Keys::SPACE) && _isStartup == true)
	{
		_isStartup = false;
		Audio::Play(_backgroundMusic);
	}

	if (state->IsKeyDown(Input::Keys::P) && !_pKeyDown && !_isStartup && !_playerWon && !_pacman->dead)
	{
		_pKeyDown = true;
		_paused = !_paused;
		// Pauses audio on pause
		if (_paused)
			Audio::Pause(_backgroundMusic);
		else if (!_paused)
			Audio::Play(_backgroundMusic);
	}

	if (state->IsKeyUp(Input::Keys::P) && !_playerWon && !_pacman->dead)
		_pKeyDown = false;
}

void Pacman::CheckViewportCollision()
{
	if (_pacman->position->X + _pacman->sourceRect->Width > Graphics::GetViewportWidth())
	{
		// Pacman hit right wall
		_pacman->position->X = 32 - _pacman->sourceRect->Width;
		Audio::Play(_screenWrapAround);
	}

	else if (_pacman->position->X + _pacman->sourceRect->Width < 32)
	{
		// Pacman hit left wall
		_pacman->position->X = Graphics::GetViewportWidth() - _pacman->sourceRect->Width;
		Audio::Play(_screenWrapAround);
	}

	else if (_pacman->position->Y + _pacman->sourceRect->Height > Graphics::GetViewportHeight())
	{
		// Pacman hit bottom wall
		_pacman->position->Y = 32 - _pacman->sourceRect->Height;
		Audio::Play(_screenWrapAround);
	}

	else if (_pacman->position->Y + _pacman->sourceRect->Height < 32)
	{
		// Pacman hit top wall
		_pacman->position->Y = Graphics::GetViewportHeight() - _pacman->sourceRect->Height;
		Audio::Play(_screenWrapAround);
	}
}

void Pacman::UpdatePacman(int elaspedTime)
{
	_pacman->currentTimeFrame += elaspedTime;

	if (_pacman->currentTimeFrame > _cPacmanFrameTime)
	{
		_pacman->frame++;

		if (_pacman->frame >= 6)
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
	_pill->sourceRect->X = _pill->sourceRect->Width * _cherry->frameCount;

	// Collision with munchie
	int bottom1 = _pacman->position->Y + _pacman->sourceRect->Height;
	int bottom2 = 0;
	int left1 = _pacman->position->X;
	int left2 = 0;
	int right1 = _pacman->position->X + _pacman->sourceRect->Width;
	int right2 = 0;
	int top1 = _pacman->position->Y;
	int top2 = 0;

	for (int i = 0; i < munchieCount; i++)
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
			_playerScore++;
		}
	}

	// Collision with cherry
	int cherryBottom = _cherry->position->Y + _cherry->sourceRect->Height;
	int cherryLeft = _cherry->position->X;
	int cherryRight = _cherry->position->X + _cherry->sourceRect->Width;
	int cherryTop = _cherry->position->Y;

	if ((bottom1 > cherryTop) && (top1 < cherryBottom) && (right1 > cherryLeft) && (left1 < cherryLeft+23) && !_isPlayerMetal)
	{
		Audio::Play(_eatCherry);
		_cherry->position = new Vector2(-150.0f, -150.0f);
		_powerUpActive = true;
		_powerUpTime = 30000;
		for (int i = 0; i < GHOSTCOUNT; i++)
			_ghosts[i]->speed = 0.1f;
		_pacman->speedMultiplier = 2.0f;
	}

	if (_powerUpActive && _powerUpTime != 0)
		_powerUpTime--;
	else if (_powerUpActive && _powerUpTime == 0)
	{
		for (int i = 0; i < GHOSTCOUNT; i++)
			_ghosts[i]->speed = 1.0f;
		_powerUpActive = false;
		_pacman->speedMultiplier = 1.0f;
		Audio::Play(_eatCherry);
	}

	int pillBottom = _pill->position->Y + _pill->sourceRect->Height;
	int pillLeft = _pill->position->X;
	int pillRight = _pill->position->X + _pill->sourceRect->Width;
	int pillTop = _pill->position->Y;

	if ((bottom1 > pillTop) && (top1 < pillBottom) && (right1 > pillLeft) && (left1 < pillLeft+23) && !_powerUpActive)
	{
		Audio::Play(_metalClang);
		_pill->position = new Vector2(-150.0f, -150.0f);
		_isPlayerMetal = true;
		_powerUpTime = 30000;
		_pacman->texture->Load("Textures/MetalPacman.tga", false);
	}
	
	if (_isPlayerMetal && _powerUpTime != 0)
		_powerUpTime--;
	else if (_isPlayerMetal && _powerUpTime == 0)
	{
		_isPlayerMetal = false;
		_pacman->texture->Load("Textures/Pacman.tga", false);
		Audio::Play(_metalClang);
	}

}

void Pacman::UpdateGhost(MovingEnemy* ghost, int elapsedTime)
{
	if (_startupGhostDirection)
	{
		for (int i = 0; i <= 3; i++)
			_ghosts[i]->direction = i;
		_startupGhostDirection = false;
	}

	// Ghost moves right
	if (ghost->direction == 0)
	{
		ghost->position->X += ghost->speed * elapsedTime;
		ghost->sourceRect->X = ghost->sourceRect->Width * ghost->direction;
	}
	// Ghost moves meft
	else if (ghost->direction == 1)
	{
		ghost->position->X -= ghost->speed * elapsedTime;
		ghost->sourceRect->X = ghost->sourceRect->Width * ghost->direction;
	}
	// Ghost moves up
	else if (ghost->direction == 2)
	{
		ghost->position->Y -= ghost->speed * elapsedTime;
		ghost->sourceRect->X = ghost->sourceRect->Width * ghost->direction;
	}
	// Ghost moves down
	else if (ghost->direction == 3)
	{
		ghost->position->Y += ghost->speed * elapsedTime;
		ghost->sourceRect->X = ghost->sourceRect->Width * ghost->direction;
	}

	// Ghost hits right edge
	if (ghost->position->X + ghost->sourceRect->Width >= Graphics::GetViewportWidth())
	{
		ghost->position->Y = (rand() % Graphics::GetViewportHeight() + 30) -30;
		ghost->direction = 1; 
	}
	// Ghost hits right edge
	 else if (ghost->position->X <= 0) 
	{
		ghost->position->Y = (rand() % Graphics::GetViewportHeight() + 30) -30;
		ghost->direction = 0;
	}
	// Ghost hits bottom edge
	else if (ghost->position->Y + ghost->sourceRect->Width >= Graphics::GetViewportHeight())
	{
		ghost->position->X = (rand() % Graphics::GetViewportWidth() + 30) -30;
		ghost->direction = 2; // Change direction
	}
	// Ghost hits top edge
	else if (ghost->position->Y <= 0)
	{
		ghost->sourceRect->X = ghost->sourceRect->Width * ghost->direction;
		ghost->position->X = (rand() % Graphics::GetViewportWidth() + 30) -30;
		ghost->direction = 3; // Change direction
	}
}

void Pacman::CheckGhostCollision()
{
	// Local variables
	int i = 0;
	int bottom1 = _pacman->position->Y + _pacman->sourceRect->Height;
	int bottom2 = 0;
	int boxBottom = _playerBox->position->Y + _playerBox->sourceRect->Height;;
	int left1 = _pacman->position->X;
	int left2 = 0;
	int boxLeft = _playerBox->position->X;
	int right1 = _pacman->position->X + _pacman->sourceRect->Width;
	int right2 = 0;
	int boxRight = _playerBox->position->X + _playerBox->sourceRect->Width;
	int top1 = _pacman->position->Y;
	int top2 = 0;
	int boxTop = _playerBox->position->Y;

	for (i = 0; i < GHOSTCOUNT; i++)
	{
		// Populate variables with Ghost data
		bottom2 = _ghosts[i]->position->Y + _ghosts[i]->sourceRect->Height;
		left2 = _ghosts[i]->position->X;
		right2 = _ghosts[i]->position->X + _ghosts[i]->sourceRect->Width;
		top2 = _ghosts[i]->position->Y;

		if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2) && !_playerWon && !_isStartup)
		{
			if (_pacman->dead == false && !_isPlayerMetal && _playerLives == 1)
			{
				_playerLives--;
				Audio::Play(_pacmanDeath);
				_pacman->dead = true;
				i = GHOSTCOUNT;
			}

			else if (!_pacman->dead && _isPlayerMetal)
			{
				if (_ghosts[i]->direction == 0)
					_ghosts[i]->direction = 1;
				else if (_ghosts[i]->direction == 1)
					_ghosts[i]->direction = 0;
				if (_ghosts[i]->direction == 2)
					_ghosts[i]->direction = 3;
				else if (_ghosts[i]->direction == 3)
					_ghosts[i]->direction = 2;
			}

			else if (!_pacman->dead)
			{
				_playerLives--;
				_pacman->position = new Vector2(150.0f, 150.0f);
				Audio::Play(_pacmanDeath);
				_powerUpTime = 3500;
				_isPlayerMetal = true;
			}
			
		}

		if ((boxBottom > top2) && (boxTop < bottom2) && (boxRight > left2) && (boxLeft < right2) && !_playerInsideBox && !_pacman->dead && _timeBetweenBoxCols == 0)
		{
			if (_ghosts[i]->direction == 0)
				_ghosts[i]->direction = 1;
			else if (_ghosts[i]->direction == 1)
				_ghosts[i]->direction = 0;
			if (_ghosts[i]->direction == 2)
				_ghosts[i]->direction = 3;
			else if (_ghosts[i]->direction == 3)
				_ghosts[i]->direction = 2;
			_timeBetweenBoxCols = 40;
		}
		if (_timeBetweenBoxCols > 0)
			_timeBetweenBoxCols--;
	}

	if ((boxBottom > top1) && (boxTop < bottom1) && (boxRight > left1) && (boxLeft < right1))
		_playerInsideBox = true;
	else
		_playerInsideBox = false;
}

void Pacman::Draw(int elapsedTime)
{
	// Allows us to easily create a string
	stringstream stream;
	if (!_isStartup && !_pacman->dead && !_playerWon)
		stream << "Munchies left: " << munchieCount - _playerScore << endl << "Lives: " << _playerLives;

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

	SpriteBatch::Draw(_pill->texture, _pill->position, _pill->sourceRect, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);

	if (!_playerInsideBox)
		SpriteBatch::Draw(_playerBox->texture, _playerBox->position, _playerBox->sourceRect, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);

	//Draw ghosts
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		SpriteBatch::Draw(_ghosts[i]->texture, _ghosts[i]->position, _ghosts[i]->sourceRect);
	}

	if (_isStartup)
	{
		SpriteBatch::Draw(_startupBackground, _startupRectangle, nullptr);
	}

	if (_paused)
	{
		stringstream menuStream;
		menuStream << "PAUSED!";

		SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Red);
	}

	if (_playerScore == munchieCount)
		SpriteBatch::Draw(_endScreenBackground, _endScreenRectangle, nullptr);

	if (_pacman->dead)
		SpriteBatch::Draw(_gameOverBackground, _gameOverRectangle, nullptr);

	// Draws String
	SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);
	SpriteBatch::EndDraw(); // Ends Drawing
}