#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")



#include <Windows.h>
#include <windowsx.h>
#include <string>
#include <exception>
#include <stdexcept>
#include <random>
#include <thread>
#include <mutex>
#include <chrono>
#include <functional>

#include "client.h"
#include "RSA.h"


class WinRegApp
{
	enum class CTL_ID {
		BUTTON_REG_ID,
		BUTTON_SING_ID,

		STATIC_LOGIN_ID,
		STATIC_PASS_ID,

		BUTTON_GO_SING_ID,
		BUTTON_VK_ID,
		BUTTON_GOOGLE_ID,
		EDIT_SING_LOGIN_ID,
		EDIT_SING_PASS_ID,

		EDIT_REG_LOGIN_ID,
		EDIT_REG_PASS_ID,
		EDIT_REG_PASS_REP_ID,
		BUTTON_GO_REG_ID,
		STATIC_REG_PASS_REP_ID,

		EDIT_MESS_OUT_ID,
		EDIT_MESS_IN_ID,
		BUTTON_MESS_SEND_ID,
		EDIT_MESS_FRIEND_LOGIN_ID,
		BUTTON_MESS_FRIEND_COONECT_ID,
		BUTTON_MESS_FRIEND_DISCOONECT_ID
	};
public:
	WinRegApp();
	~WinRegApp() {};

	int Run();

private:
	void WindowRegistration();

	static LRESULT CALLBACK AppProc(HWND hWinowMain, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK AppWindowProc(HWND hWinowMain, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void CreateStartMenuForm();
	void SingInForm();
	void RegisrationForm();
	void MessengerWndForm();

	unsigned long ReadFriendMessages();


	HFONT hFont{};
	const std::wstring ClassName = L"ClassMyApplication";
	HWND hWinowMain{}, hButtonREG{}, hbuttonSING{};  //  Дескриптор окна: указатель на объект ядка, в котором будет храниться информация об окне
	HWND hButtonGOSING{}, hButtonGOOGLE{}, hButtonVK{}, hEditLOG_S{}, hEditPASS_S{};
	HWND hStaticLogin{}, hStaticPass{}, hStaticPassRep;
	HWND hButtonGOREG{}, hEditLOG_R{}, hEditPASS_R{}, hEditPASSREP_R;
	HWND hMessIn{}, hMessOut{}, hMessButtonSEND{}, hFriendLogin{}, hButtonFriendConnect{}, hButtonFriendDisconnect{};

	const int AppWidth = 400, AppHight = 250;
	const int AppRSWidth = 400, AppRSHight = 350;
	const int AppMessWidth = 600, AppMessHight = 680;

	ServerConnection server{ hWinowMain };
	RSA rsa;
	std::wstring messengeDB{};
	std::string FriendLogin{};

	void* HandleThread;
	unsigned long IdThread;
	std::mutex g_lock;
	static DWORD WINAPI ThreadF(void* arg) { return ((WinRegApp*)arg)->ReadFriendMessages(); }
	std::wstring friend_name;
};

