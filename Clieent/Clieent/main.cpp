#include "WinRegApp.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int nCmdShow)  // CALLBACK - ���������� ������� �� ����� ������� ����, hInstance - ��������� �� ������ ������������ ������, szCmdLine - ��������� �� �������� ������� ���������� ���������, mCmdShow - � ����� ��������� ��������� ���� (��������, ����������, �� ���� �����)
{
	WinRegApp app;
	return app.Run();
}