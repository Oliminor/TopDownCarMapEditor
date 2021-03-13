#include "stdafx.h"
#include "SpriteCar.h"

CSpriteCar::CSpriteCar(Sint16 x, Sint16 y, Uint32 time, int m)
	: CSprite(x, y, 25, 50, "car.bmp",CColor::Black(), time)
{
	position = 0;
	turn = 0;
	gear = 0;
	checkPoint = 0;
	lap = 1;
	name = "car";
	brakes = false;
	mode = m;

	//Sensors pictures (not visible normally)
	point.LoadImageW("particle.png");
	point.SetImage("particle.png");

	point2.LoadImageW("particle.png");
	point2.SetImage("particle.png");

	switch (mode)
	{
	case PLAYER:
		// Accelerate when forward
		accelerate = 0.5;
		// Rolling resistance
		rRoll = 0.6;
		// Turning value (turning diameter change)
		turnValue = 50;
		break;
	case AI:
		// Few random stats for the AI
		accelerate = (40 + rand() % 25) / 100.0f;
		rRoll = (50 + rand() % 20) / 100.0f;
		turnValue = 35 + rand() % 25;
		break;
	}
}

CSpriteCar::~CSpriteCar()
{
}

void CSpriteCar::OnUpdate(Uint32 time, Uint32 deltaTime)
{
	// first thing done for you: retrieve the current speed
	float v = GetSpeed();
	// Air rasistance
	float rAir = pow(v, 2) * 0.0001;
	// brake resistance only active when brake is on, otherwise 0
	float rBrake = 0;
	if (brakes) rBrake = 5;
	// all the resistance (or drag?) together
	float resistance = rRoll + rAir + rBrake;

	// Accelerate forwards / backwards
	// Disabled if brakes are on (Player mode only)
	switch (mode)
	{
	case AI:
		if (gear == 1) v = v + accelerate * deltaTime;
		else if (gear == -1) v = v - (accelerate / 2) * deltaTime;
		break;

	case PLAYER:
		if (!brakes)
		{
			if (gear == 1) v = v + accelerate * deltaTime;
			else if (gear == -1) v = v - (accelerate / 2) * deltaTime;
		}
		break;
	case FINISH:
		brakes = true;
		break;
	}

	// Applying resistance, depending if the car is going forwards or backwards 
	if (v > 3) v = v - resistance;
	else if (v < -3) v = v + resistance;

	// Set the speed / velocity 0 between 2 value (otherwise can be glitchy)
	if (v < 3 && v > -3) v = 0;

	// At the end, don't forget to set the new speed back to your car!
	SetSpeed(v);
	
	// TURN
	// Turning direction / rotation calculating based on the velocity, so only turning when the car is accelerate (both way)
	if (turn == 1)
	{
		SetDirection(GetDirection() + v / turnValue);
		SetRotation(GetDirection());
	}
	else if (turn == -1)
	{
		SetDirection(GetDirection() - v / turnValue);
		SetRotation(GetDirection());
	}

	// AI stuff
	// Sensors position

	CVector leftB = CVector(GetX() + GetWidth() / 1.3, GetY() + GetHeight() / 1);
	CVector rightB = CVector(GetX() - GetWidth() / 1.3, GetY() + GetHeight() / 1);

	// Calculating the points position while the vehicle rotate
	CVector carP = GetPosition();
	float xN = sin(DEG2RAD(-GetRotation()));
	float yN = cos(DEG2RAD(-GetRotation()));
	// Sorry about this, but its work
	CVector right = CVector(yN * (leftB.m_x - carP.m_x) - xN * (leftB.m_y - carP.m_y) + carP.m_x, xN * (leftB.m_x - carP.m_x) + yN * (leftB.m_y - carP.m_y) + carP.m_y);
	CVector left = CVector(yN * (rightB.m_x - carP.m_x) - xN * (rightB.m_y - carP.m_y) + carP.m_x, xN * (rightB.m_x - carP.m_x) + yN * (rightB.m_y - carP.m_y) + carP.m_y);

	// Setting the sensor sprites to the sensors position
	point.SetPosition(right);
	point2.SetPosition(left);

	point.Update(time);
	point2.Update(time);
	point.SetSize(10, 10);
	point2.SetSize(10, 10);
	SetSize(25, 50);

	CSprite::OnUpdate(time, deltaTime);
}

// For showing the Ai / player position at the end
void CSpriteCar::OnDraw(CGraphics* g) 
{
	if (GetGamePosition() > 0)
		*g << left << xy(1080, 700 - GetGamePosition() * 30) << font("ChavaRegular.ttf", 25) << color(CColor::White()) << GetGamePosition() << ". - " << GetName();
}