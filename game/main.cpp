#include "stdafx.h"
#include "MyGame.h"

CMyGame game;
CGameApp app;

int main(int argc, char* argv[])
{
	app.OpenConsole();
	app.OpenWindow(1280, 720, "My Game!");
	app.SetFPS(60);
	//app.OpenFullScreen(1280, 720, 0);
	app.SetClearColor(CColor::DarkGreen());
	app.Run(&game);
	return(0);
}
