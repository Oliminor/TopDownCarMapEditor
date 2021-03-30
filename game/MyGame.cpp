#include "stdafx.h"
#include "MyGame.h"

CMyGame::CMyGame(void)
{
	player = NULL;
}

CMyGame::~CMyGame(void)
{

}

void CMyGame::OnUpdate()
{
	if (player) player->Update(GetTime());

	camera.Update(GetTime());
	start.Update(GetTime());

	for (CSprite* ui : uiList)
	{
		ui->Update(GetTime());
		if (ui->GetState() == 1) ui->SetSize(256, 64);
	}
	for (CSpriteCar* bots : aiList) bots->Update(GetTime());

	start.SetSize(80, 20);

	CameraMode();
	RoadSystem();

	startCounter -= (float)GetDeltaTime() / 1000;
	if (startCounter < 1) startBool = true;

	// I moved the forward here, so you can start the race while pressing it
	if (IsKeyDown(SDLK_UP) && startBool)
		player->SetGear(1);
}

void CMyGame::OnDraw(CGraphics* g)
{
	g->SetScrollPos(-camera.GetX() + GetWidth() / 2, -camera.GetY() + GetHeight() / 2);

	// Only Draw the pieces if its on the screen for a better FPS
	for (CSprite* roads : mapList)
	{
		if (roads->GetX() > camera.GetX() - GetWidth() && roads->GetX() < camera.GetX() + GetWidth() &&
			roads->GetY() > camera.GetY() - GetHeight() && roads->GetY() < camera.GetY() + GetHeight())
		{
			if (roads->GetState() == PLACED_PIECE || roads->GetState() == GRABBED_PIECE || roads->GetState() == FIXED_PIECE) roads->Draw(g);
		}
	}

	start.Draw(g);

	for (CSpriteCar* bots : aiList) bots->Draw(g);
	// Checkpoint drwaing for test testing
	//for (CSprite* checkp : checkPointList) checkp->Draw(g);

	if (player) player->Draw(g);

	g->SetScrollPos(0, 0);

	if (gameMode == EDITOR_MODE)
	{
		for (CSprite* ui : uiList)
		{
			if (ui->GetState() == 0) ui->Draw(g);
		}

		for (CSprite* roads : mapList)
		{
			if (roads->GetState() == BUILD_TEMPLATE)
				roads->Draw(g);
		}

		if (tutorialBool)
		{
			*g << center << xy(GetWidth() / 2, GetHeight() / 2 + size * 2.5) << font("ChavaRegular.ttf", 25) << color(CColor::White()) << "[F2] Start Race - [R] reset the map (remove every placed piece)";
			*g << center << xy(GetWidth() / 2, GetHeight() / 2 + size * 2.3) << font("ChavaRegular.ttf", 25) << color(CColor::White()) << "Hold [RMB] anywhere on the map and move your mouse to scroll the map";
			*g << center << xy(GetWidth() / 2, GetHeight() / 2 + size * 2.1) << font("ChavaRegular.ttf", 25) << color(CColor::White()) << "Grab a piece from the buttom left tileMap hold [LMB] and place it anywhere";
			*g << center << xy(GetWidth() / 2, GetHeight() / 2 + size * 1.9) << font("ChavaRegular.ttf", 25) << color(CColor::White()) << "[Q] delete the piece under your mouse position";
		}
		else
		{
			*g << center << xy(GetWidth() / 2, GetHeight() / 2 + size * 2.5) << font("ChavaRegular.ttf", 25) << color(CColor::White()) << "[F1] Show keybindings";
		}
		// FPS counter from you 
		/*
		static int prevT = GetTime();
		if (GetTime() > prevT)
			*g << left << xy(GetWidth() - 170, GetHeight() - 50) << font("ChavaRegular.ttf", 30) << color(CColor::White()) << "fps = " << (int)(1000 / (GetTime() - prevT));
		prevT = GetTime();
		*/
	}
	else if (gameMode == PLAY_MODE)
	{
		for (CSpriteCar* bots : aiList) bots->OnDraw(g);
		if (player) player->OnDraw(g);
		if (laps >= player->GetLap())
		*g << left << xy(0, GetHeight() - 50) << font("ChavaRegular.ttf", 40) << color(CColor::White()) << "Laps: " << player->GetLap() << "/" << laps;
		else
		{
			*g << center << xy(GetWidth() / 2, GetHeight() / 2 + size) << font("ChavaRegular.ttf", 60) << color(CColor::White()) << "Finish!";
			*g << center << xy(GetWidth() / 2, GetHeight() / 2 + size / 2) << font("ChavaRegular.ttf", 25) << color(CColor::White()) << "[F2] restart race or [F3] editor mode";
		}

		for (CSprite* ui : uiList)
		{
			if (ui->GetState() == 1 && startCounter > 0) ui->Draw(g);
		}
	}
}

void CMyGame::OnInitialize()
{
	TemplateGenerator();

	start.LoadImageW("start.bmp", CColor::Black());
	start.SetImage("start.bmp");

	EditorMode();
}

void CMyGame::EditorMode()
{
	gameMode = EDITOR_MODE;
	cameraMode = FREE_CAMERA;
	laps = 3;
	checkPointList.delete_all();
	startBool = false;
	tutorialBool = false;

	// Spawn the player vehicle for testing
	if (player) delete player;
	player = new CSpriteCar(0, 0, GetTime(), CSpriteCar::PLAYER);
	player->SetMode(CSpriteCar::PLAYER);
	player->SetName("Player");
	player->SetPosition(start.GetPosition());

	camera.SetPosition(start.GetPosition());

	for (CSpriteCar* bots : aiList) delete bots;
	aiList.clear();
}

void CMyGame::StartGame()
{
	gameMode = PLAY_MODE;

	// sethealth is the checkpoint state
	start.SetHealth(0);
	startBool = false;

	// start lamp animation
	for (CSprite* ui : uiList)
	{
		if (ui->GetState() == 1) ui->SetAnimation("lampAnim", 1);
	}
	startCounter = 6;

	// Spawn new player car
	if (player) delete player;
	player = new CSpriteCar(0, 0, GetTime(), CSpriteCar::PLAYER);
	player->SetMode(CSpriteCar::PLAYER);
	player->SetName("Player");

	// Set the camera to follow the player
	cameraMode = PLAYER_CAMERA;

	// Clear AI cars
	for (CSpriteCar* bots : aiList) delete bots;
	aiList.clear();

	// Reset checkpoint system for the new map
	checkPointList.delete_all();

	float tempPosX = 0;
	float tempPosY = 0;
	int nameInt = 0;

	// Placing down the AI cars
	for (int i = 0; i < 8; i++)
	{
		nameInt++;
		int sidePosition = 15;
		if (i % 2 == 0) sidePosition = -15;
		CColor color = CColor(rand() % 255, rand() % 255, rand() % 255);

		CSpriteCar* ai = new CSpriteCar(size / 2, 0, GetTime(), CSpriteCar::AI);
		aiList.push_back(ai);
		ai->SetMode(CSpriteCar::AI);
		ai->SetY((start.GetY() - size / 1.4) - (i * size / 4));
		ai->SetX(start.GetX() + sidePosition);
		ai->SetName("AiCar" + to_string(nameInt));
		ai->LoadImageW("carA.bmp", CColor::Black());
		ai->SetImage("carA.bmp");

		tempPosY = (start.GetY() - size / 1.4) - ((i + 1) * size / 4);
		tempPosX = (start.GetX() + sidePosition * -1);
		/*
		for (int i = 0; i < ai->GetWidth(); i++)
		{
			for (int j = 0; j < ai->GetHeight(); j++)
			{
				if (ai->GetGraphics()->GetPixel(i, j).GetR() == 210)
				{
					ai->GetGraphics()->SetPixel(i, j, color);
				}
			}
		}
		*/
	}
	// Set the player behind them
	player->SetY(tempPosY);
	player->SetX(tempPosX);

	// Placing down the checkpoints to every single road piece (invisible)
	int checkPointNumber = 0;
	for (CSprite* roads : mapList)
	{
		if (roads->GetState() == PLACED_PIECE)
		{
			checkPointNumber++;
			CSprite* checkPoint = new CSprite;
			checkPointList.push_back(checkPoint);
			checkPoint->LoadImageW("particle.png", CColor::Black());
			checkPoint->SetImage("particle.png");
			checkPoint->SetPosition(roads->GetPosition());
			checkPoint->SetSize(120, 120);
			checkPoint->SetState(checkPointNumber);
		}
	}
}

void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_F4 && (mod & (KMOD_LALT | KMOD_RALT)))
		StopGame();

	if (sym == SDLK_F1)
	{
		if (tutorialBool) tutorialBool = false;
		else tutorialBool = true;
	}

	if (sym == SDLK_F2)
		StartGame();

	if (sym == SDLK_F3)
		EditorMode();

	if (sym == SDLK_r && gameMode == EDITOR_MODE)
	{
		for (CSprite* roads : mapList)
		{
			if (roads->GetState() == PLACED_PIECE) roads->Delete();
		}
		mapList.delete_if(deleted);
	}

	// Steer the car
	if (!player) return;
	if (sym == SDLK_RIGHT)
		player->Turn(1);
	if (sym == SDLK_LEFT)
		player->Turn(-1);
	if (sym == SDLK_DOWN && startBool)
		player->SetGear(-1);
	if (sym == SDLK_SPACE)
		player->SetBrake(true);
}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_RIGHT || sym == SDLK_LEFT)
		player->Turn(0);
	if (sym == SDLK_UP || sym == SDLK_DOWN)
		player->SetGear(0);
	if (sym == SDLK_SPACE)
		player->SetBrake(false);
}

void CMyGame::OnLButtonDown(Uint16 x,Uint16 y)
{
	// If you click on the template piece (not sure how to call them) it gives a placeable piece in your mouse position
	// and you can player wherever
	for (CSprite* roads : mapList)
	{
		if (roads->GetState() == BUILD_TEMPLATE && roads->HitTest(x, y))
		{
			CSprite* newRoads = roads->Clone();

			mapList.push_back(newRoads);
			newRoads->SetState(GRABBED_PIECE);
		}
	}
}

void CMyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
	// If you let the mouse button, it places down the piece and change the stat into check for further checking
	for (CSprite* roads : mapList)
	{
		if (roads->GetState() == GRABBED_PIECE)
		{
			roads->SetState(CHECK_PIECE);
		}

		// If other piece is under your placed piece it delete it and replace your new piece 
		for (CSprite* roadCheck : mapList)
		{
			if (roadCheck->HitTest(roads) && roads->GetState() == CHECK_PIECE && roadCheck->GetState() == PLACED_PIECE)
			{
				roadCheck->Delete();
				roads->SetState(PLACED_PIECE);
			}
		}

		// If you put a new piece on the starting road it delete it, you can't place roads there
		for (CSprite* roadCheck : mapList)
		{
			if (roadCheck->HitTest(roads) && roads->GetState() == CHECK_PIECE && roadCheck->GetState() == FIXED_PIECE)
			{
				roads->Delete();
			}
		}
	}

	// If the placed piece is not collide with anything (nothing under it), it changed the status int placed piece
	for (CSprite* roads : mapList)
	{
		if (roads->GetState() == CHECK_PIECE) roads->SetState(PLACED_PIECE);
	}
	mapList.delete_if(deleted);
}

// Distance calculator
float CMyGame::Distance(float target, float source)
{
	float distance = target - source;

	return distance;
}

// Vector Lerp for smooth camera movement
CVector CMyGame::VectorLerp(CVector targetA, CVector targetB, float time)
{
	return targetA * (1 - time) + targetB * time;
}

// Generating the map builder pieces what you can grab and build your own road
void CMyGame::TemplateGenerator()
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (i != 1 || j != 1)
			{
				CSprite* roads = new CSprite;

				mapList.push_back(roads);
				roads->LoadImageW("road.bmp", "road", CSprite::Sheet(3, 3).Tile(i, j), CColor::Black());
				roads->SetImage("road");
				roads->SetPosition(50 + i * 64, 50 + j * 64);
				roads->SetState(BUILD_TEMPLATE);
			}
		}
	}

	// Background for the builder pieces
	CSprite* ui = new CSprite;

	uiList.push_back(ui);
	ui->LoadImageW("pieceBG.bmp", CColor::Black());
	ui->SetImage("pieceBG.bmp");
	ui->SetSize(180, 180);
	ui->SetPosition(114, 114);
	ui->SetState(0);

	CSprite* uiLamp = new CSprite;

	uiList.push_back(uiLamp);
	uiLamp->LoadAnimation("startLamp.bmp", "lampAnim", CSprite::Sheet(6, 1).Row(0).From(0).To(5), CColor::Black());
	uiLamp->SetSize(256, 64);
	uiLamp->SetPosition(GetWidth() / 2, GetHeight() / 2 + size * 2);
	uiLamp->SetState(1);


	// Generate the starting road + start/finish line
	for (int i = 0; i < 4; i++)
	{
		CSprite* roads = new CSprite;

		mapList.push_back(roads);
		if (i == 0) roads->LoadImageW("road.bmp", "road", CSprite::Sheet(3, 3).Tile(0, 1), CColor::Black());
		else roads->LoadImageW("road.bmp", "road", CSprite::Sheet(3, 3).Tile(1, 1), CColor::Black());
		roads->SetImage("road");
		roads->SetPosition(size / 2, -(i * size - size / 2));
		roads->SetState(FIXED_PIECE);
	}

	start.SetPosition(size / 2, size / 2);
}

// Checing the cars current checkPoint position and lap
void CMyGame::CheckPosition(CSpriteCar& car)
{
	for (CSprite* checkPoint : checkPointList)
	{
		if (checkPoint->HitTest(&car) && checkPoint->GetStatus() > car.GetCheckPoint())
		{
			car.SetCheckPoint(checkPoint->GetStatus());
		}
	}

	if (car.HitTest(&start) && car.GetCheckPoint() == checkPointList.size())
	{
		car.SetLap(car.GetLap() + 1);
		car.SetCheckPoint(0);
		if ((car.GetLap() - 1) == laps)
		{
			start.SetHealth(start.GetHealth() + 1);
			car.SetGamePosition(start.GetHealth());
			car.SetMode(CSpriteCar::FINISH);
		}
	}
}

// Different camera modes
void CMyGame::CameraMode()
{
	// Static variable for the camera current position (used for lerping the camera)
	static CVector prevC;

	// Two different camera mode, one for the player and for the the map editor (Free camera)
	switch (cameraMode)
	{
	case FREE_CAMERA:
	{
		static CVector temp;
		// If you click anywhere on the map with RMB you can drag the camera
		if (IsRButtonDown())
		{
			CVector newP = CVector(Distance(temp.m_x, GetMouseCoords().m_x), Distance(temp.m_y, GetMouseCoords().m_y));
			camera.SetPosition(camera.GetPosition() + newP);
			temp = GetMouseCoords();
		}
		else
		{
			// temp position when before you click on the screen
			temp = GetMouseCoords();
		}
	}
	break;
	// Lerping camera for the player
	case PLAYER_CAMERA:
		camera.SetPosition(VectorLerp(prevC, player->GetPosition(), 0.3));
		break;
	}

	prevC = CVector(camera.GetPosition());
}

// Basically everything what happens on the road / map
void CMyGame::RoadSystem()
{
	// Calculating the map editor piece positions (snapping)
	CVector position = CVector(((GetMouseCoords().m_x + ((int)camera.GetX() - GetWidth() / 2))), (GetMouseCoords().m_y + ((int)camera.GetY() - GetHeight() / 2)));

	// Addition is needed because if the camera is on the negative side (either X or Y) the snapping is not right
	float Xaddition = 0;
	float Yaddition = 0;
	if (position.m_x > 0) Xaddition = size / 2;
	else Xaddition = -(size / 2);

	if (position.m_y > 0) Yaddition = size / 2;
	else Yaddition = -(size / 2);

	//Road system update
	for (CSprite* roads : mapList)
	{
		roads->Update(GetTime());

		// The placable pieces is also stored on the mapList but their size is different
		if (roads->GetState() != BUILD_TEMPLATE) roads->SetSize(size, size);
		else roads->SetSize(64, 64);

		// If you grab a piece (holding left mouse button) the calculated piece position is used here (snapping)
		if (roads->GetState() == GRABBED_PIECE)
		{
			roads->SetPosition(((GetMouseCoords().m_x + ((int)camera.GetX() - GetWidth() / 2)) / size * size + Xaddition), (GetMouseCoords().m_y + ((int)camera.GetY() - GetHeight() / 2)) / size * size + Yaddition);
		}

		// If you press Q and the mouse is on the placed map piece it delete it
		if (IsKeyDown(SDLK_q) && roads->HitTest(position) && roads->GetState() == PLACED_PIECE)
		{
			roads->Delete();
		}

		// road and AI cars system
		if (roads->GetState() == PLACED_PIECE || roads->GetState() == FIXED_PIECE)
		{
			for (CSpriteCar* bots : aiList)
			{
				//Checking the bots positions (placing)
				CheckPosition(*bots);

				// Set the gear 1, so the cars can move
				if (startBool) bots->SetGear(1);

				// If only the left sensor collide with the road, the car turn left (and brakes on, kind of slowing down when turning)
				if (roads->HitTest(&bots->point2, 1))
				{
					bots->Turn(-1);
					bots->SetBrake(1);
				}

				// If only the right sensor collide with the road, the car turn right (brakes on too)
				if (roads->HitTest(&bots->point, 1))
				{
					bots->Turn(1);
					bots->SetBrake(1);
				}

				// if both sensor collide with the road, the car going straight and brake is off
				if (roads->HitTest(&bots->point2, 1) && roads->HitTest(&bots->point, 1))
				{
					bots->Turn(0);
					bots->SetBrake(0);
				}
			}
		}
	}

	//Checking the player position (placing)
	if (gameMode == PLAY_MODE)
		CheckPosition(*player);

	mapList.delete_if(deleted);
}