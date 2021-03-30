#pragma once

#include "SpriteCar.h"

class CMyGame : public CGame
{
	typedef enum
	{
		FREE_CAMERA,
		PLAYER_CAMERA,

		EDITOR_MODE,
		PLAY_MODE,

		BUILD_TEMPLATE,
		GRABBED_PIECE,
		CHECK_PIECE,
		PLACED_PIECE,
		FIXED_PIECE,
	};
	int cameraMode;
	int gameMode;
	int laps;

	int size = 128;

	CSpriteCar *player;
	list<CSpriteCar*> aiList;
	CSprite camera;

	CSpriteList mapList;
	CSpriteList uiList;
	CSpriteList checkPointList;

	CSprite start;

	bool startBool;
	bool tutorialBool;
	float startCounter;
public:
	CMyGame(void);
	~CMyGame(void);

	// Per-Frame Callback Funtions (must be implemented!)
	virtual void OnUpdate();
	virtual void OnDraw(CGraphics* g);

	// Game Life Cycle
	virtual void OnInitialize();
	virtual void StartGame();
	virtual void EditorMode();

	// Keyboard Event Handlers
	virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	virtual void OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode);

	// Mouse Events Handlers
	virtual void OnLButtonDown(Uint16 x,Uint16 y);
	virtual void OnLButtonUp(Uint16 x,Uint16 y);

	float Distance(float target, float source);
	CVector VectorLerp(CVector targetA, CVector targetB, float time);
	void CheckPosition(CSpriteCar& car);
	void TemplateGenerator();
	void CameraMode();
	void RoadSystem();
};
