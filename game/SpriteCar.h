#pragma once

class CSpriteCar : public CSprite
{
	int mode;		// Ai or Player mode 

	int turn;		// Steering Wheel: turn < 0 is left;  turn == 0 is straight on;  turn > 0 is right
	int gear;		// Gear:  gear > 0 is gear; gear < 0 is reverse; gear == 0 is neutral
	bool brakes;	// Brakes

	int checkPoint; // every road piece has a checkpoint and this int increase if you touced one (and didn't missed any)
	int position;	// car position
	int lap;		// which lap the vehicle is doing currently
	string name;	// car name

	float accelerate, rRoll, turnValue;
public:
	CSprite point, point2; // CSprite for the two sensor point (otherwise the hittest not pixel perfect)

	CSpriteCar(Sint16 x, Sint16 y, Uint32 time, int m);
	~CSpriteCar(void);

	//Enum because the code is more readable with it
	typedef enum
	{
		AI,
		PLAYER,
		FINISH,
	};

// User Steering (+ few additional setter / getter)

	void Turn(int n)				{ turn = n; }
	void SetGear(int n)				{ gear = n; }
	void SetBrake(bool b)			{ brakes = b; }
	void SetMode(int m)				{ mode = m; }
	void SetCheckPoint(int c)		{ checkPoint = c; }
	void SetLap(int l)				{ lap = l; }
	void SetGamePosition(int p)			{ position = p; }
	void SetName(string s)			{ name = s; }

	int GetCheckPoint()				{ return checkPoint; }
	int GetLap()					{ return lap; }
	int GetGamePosition()				{ return position; }
	string GetName()				{ return name; }

// Called 60 times every second to update the position of the car
	virtual void OnUpdate(Uint32 time, Uint32 deltaTime);
	virtual void OnDraw(CGraphics* g);
};
