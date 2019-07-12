#include "WinRegApp.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int nCmdShow)  // CALLBACK - вызываемый обйъект за собой очищает стек, hInstance - указатель на начало исполняемого модуля, szCmdLine - указатель на арументы которые передаются программе, mCmdShow - в каком состоянии находится окно (свернуто, развернуто, на весь экран)
{
	WinRegApp app;
	return app.Run();
}