#include "WinRegApp.h"


WinRegApp::WinRegApp()
{
	using std::wstring;
	using std::string;

	try {
		this->WindowRegistration();
		this->CreateStartMenuForm();
	}
	catch (const std::exception & excep) {
		string expt_data = excep.what();

		MessageBox(nullptr, wstring(begin(expt_data), end(expt_data)).c_str(), L"Ошибка", MB_ICONERROR | MB_OK);
		ExitProcess(EXIT_FAILURE);
	}
}


void WinRegApp::WindowRegistration()
{
	using std::runtime_error;
	using namespace std::string_literals;

	WNDCLASSEX wc{ sizeof(WNDCLASSEX) };  //  структура, отвечающая за характеристики окна
	wc.cbClsExtra = 0;  //  Дополнительное выделение паямти в окне
	wc.cbWndExtra = 0;  //  Дополнительное выделение паямти в окне
	wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));  //  Возвращает дескриптор кисти, которая окрашивает окно
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);  //  Подгружаеам курсор
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);  //  Подгружаем иконку
	wc.hInstance = GetModuleHandle(nullptr);  //  Указатель на начало исполняемого модуля
	wc.lpfnWndProc = WinRegApp::AppProc;
	wc.lpszClassName = this->ClassName.c_str();  //  Имя класса
	wc.lpszMenuName = NULL;  //  Указатель на меню
	wc.style = CS_VREDRAW | CS_HREDRAW;  //  Стили окна, по дефолту эти флаги

	if (!RegisterClassEx(&wc))  //  Регистрация в системе класса окна
		throw runtime_error("ERROR: can't registr main window class");

	RECT _windowRC{ 0, 0, this->AppWidth, this->AppHight };
	AdjustWindowRect(&_windowRC, WS_OVERLAPPED, false);

	//	wc.lpszClassName - имя класса окна	//	L"Заголовок" - Заголовок	//	WS_OVERLAPPEDWINDOW - стили окна	//	x = 0	//	y = 0	//  Ширина = 600	//  Высота = 600	//	Указатель на родительское окно = nullptr	//  Указатель на меню = nullptr	//  wc.hInstance - указатель на начало исполняемого модуля  //	параметры
	this->hWinowMain = CreateWindow(
		this->ClassName.c_str(),
		L"2MAX production",
		WS_DLGFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZE,
		(GetSystemMetrics(SM_CXSCREEN) - _windowRC.right) / 2,
		(GetSystemMetrics(SM_CYSCREEN) - _windowRC.bottom) / 2,
		_windowRC.right, _windowRC.bottom,
		nullptr, nullptr, nullptr, this
	);


	if (!this->hWinowMain)
		throw runtime_error("ERROR: can't create main window");
}


LRESULT WinRegApp::AppProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WinRegApp* pApp;
	if (uMsg == WM_NCCREATE) {
		pApp = static_cast<WinRegApp*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
		SetLastError(0);
		if (!SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pApp))) {
			if (GetLastError() != 0)
				return false;
		}
	}
	else {
		pApp = reinterpret_cast<WinRegApp*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	}

	if (pApp) {
		pApp->hWinowMain = hWnd;
		return pApp->AppWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


int WinRegApp::Run() {
	MSG msg{};  //  Сутруктура, содержащая информацию о сообщении (свернуть окно, изменить что-нибудь), универсальный способ общения

	ShowWindow(this->hWinowMain, SW_SHOWDEFAULT);  //  Показать окно
	UpdateWindow(this->hWinowMain);  //  Перерисовка окна(рекомендуется)

	while (GetMessage(&msg, 0, 0, 0)) {  //  Цикл обработки сообщений  //  ??  //  Фильтры
		TranslateMessage(&msg);  //  Расшифрововает сообщение 
		DispatchMessage(&msg);	//  Передает сообщение в оконную процедуру на обработку
	}

	return static_cast<int>(msg.wParam);
}

void WinRegApp::CreateStartMenuForm()
{
	using std::runtime_error;
	using namespace std::string_literals;


	this->hButtonREG = CreateWindowEx(
		0,
		L"BUTTON",
		L"Registration",
		WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
		90, 50, 200, 37,
		this->hWinowMain,
		reinterpret_cast<HMENU>(WinRegApp::CTL_ID::BUTTON_REG_ID),
		nullptr, nullptr
	);
	if (!this->hButtonREG)
		throw runtime_error("ERROR: can't create button");

	this->hbuttonSING = CreateWindowEx(
		0,
		L"BUTTON",
		L"Sing in",
		WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
		90, 100, 200, 37,
		this->hWinowMain,
		reinterpret_cast<HMENU>(WinRegApp::CTL_ID::BUTTON_SING_ID),
		nullptr, nullptr
	);
	if (!this->hbuttonSING)
		throw runtime_error("ERROR: can't create button");



	this->hFont = CreateFont(18, 0, 0, 0, FW_REGULAR, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Roboto");

	SendMessage(this->hButtonREG, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
	SendMessage(this->hbuttonSING, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);

}

void WinRegApp::SingInForm()
{
	using std::runtime_error;
	using namespace std::string_literals;

	DestroyWindow(this->hbuttonSING);
	DestroyWindow(this->hButtonREG);

	SetWindowPos(
		this->hWinowMain,
		HWND_TOP,
		(GetSystemMetrics(SM_CXSCREEN) - 350) / 2,
		(GetSystemMetrics(SM_CYSCREEN) - 300) / 2,
		350,
		300,
		NULL
	);

	//  Кнопка авторицазии
	this->hButtonGOSING = CreateWindowEx(
		0,
		L"BUTTON",
		L"Sing in",
		WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
		65, 130, 200, 50,
		this->hWinowMain,
		reinterpret_cast<HMENU>(WinRegApp::CTL_ID::BUTTON_GO_SING_ID),
		nullptr, nullptr
	);
	if (!this->hButtonGOSING)
		throw runtime_error("ERROR: can't create button");

	//  Кнопка авторицазии через vk
	this->hButtonVK = CreateWindowEx(
		0,
		L"BUTTON",
		L"VK",
		WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
		65, 185, 98, 30,
		this->hWinowMain,
		reinterpret_cast<HMENU>(WinRegApp::CTL_ID::BUTTON_VK_ID),
		nullptr, nullptr
	);
	if (!this->hButtonVK)
		throw runtime_error("ERROR: can't create button");

	//  Кнопка авторицазии через Google
	this->hButtonGOOGLE = CreateWindowEx(
		0,
		L"BUTTON",
		L"Google",
		WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
		167, 185, 98, 30,
		this->hWinowMain,
		reinterpret_cast<HMENU>(WinRegApp::CTL_ID::BUTTON_GOOGLE_ID),
		nullptr, nullptr
	);
	if (!this->hButtonGOOGLE)
		throw runtime_error("ERROR: can't create button");

	//  Поле для ввода логина
	this->hEditLOG_S = CreateWindowEx(
		0,
		L"EDIT",
		L"",
		WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | WS_BORDER,
		65, 40, 200, 24,
		this->hWinowMain,
		reinterpret_cast<HMENU>(WinRegApp::CTL_ID::EDIT_SING_LOGIN_ID),
		nullptr, nullptr
	);
	if (!this->hEditLOG_S)
		throw runtime_error("ERROR: can't create edit");

	//  Поле для ввода пароля
	this->hEditPASS_S = CreateWindowEx(
		0,
		L"EDIT",
		L"",
		WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | WS_BORDER,
		65, 95, 200, 24,
		this->hWinowMain,
		reinterpret_cast<HMENU>(WinRegApp::CTL_ID::EDIT_SING_PASS_ID),
		nullptr, nullptr
	);
	if (!this->hEditPASS_S)
		throw runtime_error("ERROR: can't create edit");

	Edit_SetPasswordChar(hEditPASS_S, '*');

	//  Надпись Login
	this->hStaticLogin = CreateWindowEx(
		0,
		L"Static",
		L" Login:",
		WS_CHILD | WS_VISIBLE,
		65, 20, 200, 20,
		this->hWinowMain,
		reinterpret_cast<HMENU>(WinRegApp::CTL_ID::STATIC_LOGIN_ID),
		nullptr, nullptr
	);
	if (!this->hStaticLogin)
		throw runtime_error("ERROR: can't create static");

	//  Надпись Password
	this->hStaticPass = CreateWindowEx(
		0,
		L"Static",
		L" Password:",
		WS_CHILD | WS_VISIBLE,
		65, 75, 200, 20,
		this->hWinowMain,
		reinterpret_cast<HMENU>(WinRegApp::CTL_ID::STATIC_PASS_ID),
		nullptr, nullptr
	);
	if (!this->hStaticPass)
		throw runtime_error("ERROR: can't create static");



	SendMessage(this->hStaticLogin, WM_SETFONT, reinterpret_cast<WPARAM>(this->hFont), TRUE);
	SendMessage(this->hStaticPass, WM_SETFONT, reinterpret_cast<WPARAM>(this->hFont), TRUE);

	SendMessage(this->hEditLOG_S, WM_SETFONT, reinterpret_cast<WPARAM>(this->hFont), TRUE);
	SendMessage(this->hEditPASS_S, WM_SETFONT, reinterpret_cast<WPARAM>(this->hFont), TRUE);
	SendMessage(this->hButtonGOSING, WM_SETFONT, reinterpret_cast<WPARAM>(this->hFont), TRUE);
	SendMessage(this->hButtonVK, WM_SETFONT, reinterpret_cast<WPARAM>(this->hFont), TRUE);
	SendMessage(this->hButtonGOOGLE, WM_SETFONT, reinterpret_cast<WPARAM>(this->hFont), TRUE);
}

void WinRegApp::RegisrationForm()
{
	using std::runtime_error;
	using namespace std::string_literals;

	DestroyWindow(this->hbuttonSING);
	DestroyWindow(this->hButtonREG);

	SetWindowPos(
		this->hWinowMain,
		HWND_TOP,
		(GetSystemMetrics(SM_CXSCREEN) - this->AppRSWidth) / 2,
		(GetSystemMetrics(SM_CYSCREEN) - this->AppRSHight) / 2,
		AppRSWidth,
		AppRSHight,
		NULL
	);

	//  Поле для ввода логина
	this->hEditLOG_R = CreateWindowEx(
		0,
		L"EDIT",
		L"",
		WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | WS_BORDER,
		90, 40, 200, 24,
		this->hWinowMain,
		reinterpret_cast<HMENU>(WinRegApp::CTL_ID::EDIT_REG_LOGIN_ID),
		nullptr, nullptr
	);
	if (!this->hEditLOG_R)
		throw runtime_error("ERROR: can't create edit");

	//  Поле для ввода пароля
	this->hEditPASS_R = CreateWindowEx(
		0,
		L"EDIT",
		L"",
		WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | WS_BORDER,
		90, 95, 200, 24,
		this->hWinowMain,
		reinterpret_cast<HMENU>(WinRegApp::CTL_ID::EDIT_REG_PASS_ID),
		nullptr, nullptr
	);
	if (!this->hEditPASS_R)
		throw runtime_error("ERROR: can't create edit");

	Edit_SetPasswordChar(hEditPASS_R, '*');

	this->hEditPASSREP_R = CreateWindowEx(
		0,
		L"EDIT",
		L"",
		WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | WS_BORDER,
		90, 150, 200, 24,
		this->hWinowMain,
		reinterpret_cast<HMENU>(WinRegApp::CTL_ID::EDIT_REG_PASS_REP_ID),
		nullptr, nullptr
	);
	if (!this->hEditPASSREP_R)
		throw runtime_error("ERROR: can't create edit");

	Edit_SetPasswordChar(hEditPASSREP_R, '*');


	//  Кнопка регистрации
	this->hButtonGOREG = CreateWindowEx(
		0,
		L"BUTTON",
		L"Registration",
		WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
		90, 190, 200, 50,
		this->hWinowMain,
		reinterpret_cast<HMENU>(WinRegApp::CTL_ID::BUTTON_GO_REG_ID),
		nullptr, nullptr
	);
	if (!this->hButtonGOREG)
		throw runtime_error("ERROR: can't create button");

	//  Надпись Login
	this->hStaticLogin = CreateWindowEx(
		0,
		L"Static",
		L" Login:",
		WS_CHILD | WS_VISIBLE,
		90, 20, 200, 20,
		this->hWinowMain,
		reinterpret_cast<HMENU>(WinRegApp::CTL_ID::STATIC_LOGIN_ID),
		nullptr, nullptr
	);
	if (!this->hStaticLogin)
		throw runtime_error("ERROR: can't create static");

	//  Надпись Password
	this->hStaticPass = CreateWindowEx(
		0,
		L"Static",
		L" Password:",
		WS_CHILD | WS_VISIBLE,
		90, 75, 200, 20,
		this->hWinowMain,
		reinterpret_cast<HMENU>(WinRegApp::CTL_ID::STATIC_PASS_ID),
		nullptr, nullptr
	);
	if (!this->hStaticPass)
		throw runtime_error("ERROR: can't create static");

	//  Надпись Password repeat
	this->hStaticPassRep = CreateWindowEx(
		0,
		L"Static",
		L" Password repeat:",
		WS_CHILD | WS_VISIBLE,
		90, 130, 200, 20,
		this->hWinowMain,
		reinterpret_cast<HMENU>(WinRegApp::CTL_ID::STATIC_REG_PASS_REP_ID),
		nullptr, nullptr
	);
	if (!this->hStaticPassRep)
		throw runtime_error("ERROR: can't create static");

	SendMessage(this->hStaticLogin, WM_SETFONT, reinterpret_cast<WPARAM>(this->hFont), TRUE);
	SendMessage(this->hStaticPass, WM_SETFONT, reinterpret_cast<WPARAM>(this->hFont), TRUE);
	SendMessage(this->hStaticPassRep, WM_SETFONT, reinterpret_cast<WPARAM>(this->hFont), TRUE);

	SendMessage(this->hEditLOG_R, WM_SETFONT, reinterpret_cast<WPARAM>(this->hFont), TRUE);
	SendMessage(this->hEditPASSREP_R, WM_SETFONT, reinterpret_cast<WPARAM>(this->hFont), TRUE);
	SendMessage(this->hEditPASS_R, WM_SETFONT, reinterpret_cast<WPARAM>(this->hFont), TRUE);
	SendMessage(this->hButtonGOREG, WM_SETFONT, reinterpret_cast<WPARAM>(this->hFont), TRUE);
}

void WinRegApp::MessengerWndForm()
{
	using std::runtime_error;

	DestroyWindow(this->hButtonGOSING);
	DestroyWindow(this->hButtonGOOGLE);
	DestroyWindow(this->hButtonVK);
	DestroyWindow(this->hEditLOG_S);
	DestroyWindow(this->hEditPASS_S);

	DestroyWindow(this->hStaticLogin);
	DestroyWindow(this->hStaticPass);
	DestroyWindow(this->hStaticPassRep);

	DestroyWindow(this->hEditLOG_R);
	DestroyWindow(this->hEditPASS_R);
	DestroyWindow(this->hEditPASSREP_R);
	DestroyWindow(this->hButtonGOREG);

	SetWindowPos(
		this->hWinowMain,
		HWND_TOP,
		(GetSystemMetrics(SM_CXSCREEN) - this->AppMessWidth) / 2,
		(GetSystemMetrics(SM_CYSCREEN) - this->AppMessHight) / 2,
		AppMessWidth,
		AppMessHight,
		NULL
	);

	this->hMessOut = CreateWindowEx(
		0,
		L"EDIT",
		L"",
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL | WS_DISABLED | ES_READONLY,
		90, 60, 300, 335,
		this->hWinowMain,
		reinterpret_cast<HMENU>(WinRegApp::CTL_ID::EDIT_MESS_OUT_ID),
		nullptr, nullptr
	);
	if (!this->hMessOut)
		throw runtime_error("ERROR: can't create DialogBox");


	this->hMessIn = CreateWindowEx(
		0,
		L"EDIT",
		L"Hello world!",
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_MULTILINE | WS_DISABLED,
		90, 400, 300, 100,
		this->hWinowMain,
		reinterpret_cast<HMENU>(WinRegApp::CTL_ID::EDIT_MESS_IN_ID),
		nullptr, nullptr
	);
	if (!this->hMessIn)
		throw runtime_error("ERROR: can't create DialogBox");

	this->hFriendLogin = CreateWindowEx(
		0,
		L"EDIT",
		L"Input friend's login",
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_MULTILINE,
		90, 10, 300, 30,
		this->hWinowMain,
		reinterpret_cast<HMENU>(WinRegApp::CTL_ID::EDIT_MESS_FRIEND_LOGIN_ID),
		nullptr, nullptr
	);
	if (!this->hFriendLogin)
		throw runtime_error("ERROR: can't create DialogBox");


	this->hMessButtonSEND = CreateWindowEx(
		0,
		L"BUTTON",
		L"SEND MESSANGE",
		WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE | WS_DISABLED,
		90, 550, 300, 50,
		this->hWinowMain,
		reinterpret_cast<HMENU>(WinRegApp::CTL_ID::BUTTON_MESS_SEND_ID),
		nullptr, nullptr
	);
	if (!this->hMessButtonSEND)
		throw runtime_error("ERROR: can't create button");

	this->hButtonFriendConnect = CreateWindowEx(
		0,
		L"BUTTON",
		L"Connect",
		WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
		400, 10, 120, 30,
		this->hWinowMain,
		reinterpret_cast<HMENU>(WinRegApp::CTL_ID::BUTTON_MESS_FRIEND_COONECT_ID),
		nullptr, nullptr
	);
	if (!this->hButtonFriendConnect)
		throw runtime_error("ERROR: can't create button");

	this->hButtonFriendDisconnect = CreateWindowEx(
		0,
		L"BUTTON",
		L"Disconnect",
		WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE | WS_DISABLED,
		400, 10, 120, 30,
		this->hWinowMain,
		reinterpret_cast<HMENU>(WinRegApp::CTL_ID::BUTTON_MESS_FRIEND_DISCOONECT_ID),
		nullptr, nullptr
	);
	if (!this->hButtonFriendConnect)
		throw runtime_error("ERROR: can't create button");

	ShowWindow(this->hButtonFriendDisconnect, SW_HIDE);

	SendMessage(this->hMessOut, WM_SETFONT, reinterpret_cast<WPARAM>(this->hFont), TRUE);
	SendMessage(this->hMessIn, WM_SETFONT, reinterpret_cast<WPARAM>(this->hFont), TRUE);
	SendMessage(this->hFriendLogin, WM_SETFONT, reinterpret_cast<WPARAM>(this->hFont), TRUE);
	SendMessage(this->hMessButtonSEND, WM_SETFONT, reinterpret_cast<WPARAM>(this->hFont), TRUE);
	SendMessage(this->hButtonFriendConnect, WM_SETFONT, reinterpret_cast<WPARAM>(this->hFont), TRUE);
	SendMessage(this->hButtonFriendDisconnect, WM_SETFONT, reinterpret_cast<WPARAM>(this->hFont), TRUE);
}

LRESULT WinRegApp::AppWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_COMMAND:
	{
		switch (static_cast<WinRegApp::CTL_ID>(LOWORD(wParam))) {

		case WinRegApp::CTL_ID::BUTTON_SING_ID:
		{
			try {
				this->SingInForm();
			}
			catch (const std::exception & excep) {
				MessageBox(this->hWinowMain, L"ERROR", L"ERROR", MB_ICONINFORMATION | MB_OK);
			}
		}
		return 0;

		case WinRegApp::CTL_ID::BUTTON_REG_ID:
		{
			try {
				this->RegisrationForm();
			}
			catch (const std::exception & excep) {
				MessageBox(this->hWinowMain, L"ERROR", L"ERROR", MB_ICONINFORMATION | MB_OK);
			}
		}
		return 0;

		case WinRegApp::CTL_ID::BUTTON_GO_SING_ID:
		{
			try {
				std::wstring login;
				std::wstring pass;

				login.resize(GetWindowTextLength(this->hEditLOG_S) + 1);
				GetWindowText(this->hEditLOG_S, &login[0], login.size());
				login.pop_back();

				pass.resize(GetWindowTextLength(this->hEditPASS_S) + 1);
				GetWindowText(this->hEditPASS_S, &pass[0], pass.size());
				pass.pop_back();

				if (login.empty() && pass.empty()) {
					MessageBox(this->hWinowMain, L"Input loging and password", L"Information", MB_ICONINFORMATION | MB_OK);
					break;
				}
				else {
					if (login.empty()) {
						MessageBox(this->hWinowMain, L"Input loging", L"Information", MB_ICONINFORMATION | MB_OK);
						break;
					}
					if (pass.empty()) {
						MessageBox(this->hWinowMain, L"Input password", L"Information", MB_ICONINFORMATION | MB_OK);
						break;
					}
				}

				std::string login_str = std::string(begin(login), end(login));
				std::string pass_str = std::string(begin(pass), end(pass));

				std::hash<std::string> pass_str_hash;

				if (this->server.Server_Connect(login_str, pass_str_hash(pass_str), false))
					this->MessengerWndForm();
				else
					MessageBox(this->hWinowMain, L"ERROR: Can't sing in ", L"Information", MB_ICONINFORMATION | MB_OK);
			}
			catch (...) {
				MessageBox(this->hWinowMain, L"ERROR", L"ERROR", MB_ICONINFORMATION | MB_OK);
			}
		}
		return 0;

		case WinRegApp::CTL_ID::BUTTON_GO_REG_ID:
		{
			try {
				std::wstring login;
				std::wstring pass;
				std::wstring pass_rep;

				login.resize(GetWindowTextLength(this->hEditLOG_R) + 1);
				GetWindowText(this->hEditLOG_R, &login[0], login.size());
				login.pop_back();

				pass.resize(GetWindowTextLength(this->hEditPASS_R) + 1);
				GetWindowText(this->hEditPASS_R, &pass[0], pass.size());
				pass.pop_back();

				pass_rep.resize(GetWindowTextLength(this->hEditPASSREP_R) + 1);
				GetWindowText(this->hEditPASSREP_R, &pass_rep[0], pass_rep.size());
				pass_rep.pop_back();


				if (login.empty()) {
					MessageBox(this->hWinowMain, L"Input loging", L"Information", MB_ICONINFORMATION | MB_OK);
					break;
				}
				if (pass.empty()) {
					MessageBox(this->hWinowMain, L"Input password", L"Information", MB_ICONINFORMATION | MB_OK);
					break;
				}
				if (pass_rep.empty()) {
					MessageBox(this->hWinowMain, L"Input password repeat", L"Information", MB_ICONINFORMATION | MB_OK);
					break;
				}
				if (pass_rep != pass) {
					MessageBox(this->hWinowMain, L"Password mismatch", L"Information", MB_ICONINFORMATION | MB_OK);
					break;
				}

				std::string login_str = std::string(begin(login), end(login));
				std::string pass_str = std::string(begin(pass), end(pass));

				std::hash<std::string> pass_str_hash;

				if (this->server.Server_Connect(login_str, pass_str_hash(pass_str), true))
					this->MessengerWndForm();
				else
					MessageBox(this->hWinowMain, L"ERROR: Can't reg", L"Information", MB_ICONINFORMATION | MB_OK);

			}
			catch (...) {
				MessageBox(this->hWinowMain, L"ERROR", L"ERROR", MB_ICONINFORMATION | MB_OK);
			}
		}
		return 0;


		case WinRegApp::CTL_ID::BUTTON_VK_ID:
		{
			MessageBox(this->hWinowMain, L"VK", L"Information", MB_ICONINFORMATION | MB_OK);
		}
		return 0;

		case WinRegApp::CTL_ID::BUTTON_GOOGLE_ID:
		{
			MessageBox(this->hWinowMain, L"Google", L"Information", MB_ICONINFORMATION | MB_OK);
		}
		return 0;

		case WinRegApp::CTL_ID::BUTTON_MESS_SEND_ID:
		{
			try {
				//EnableWindow(this->hMessButtonSEND, FALSE);
				std::wstring message;
				message.resize(GetWindowTextLength(this->hMessIn) + 1);
				GetWindowText(this->hMessIn, &message[0], message.size());
				message.pop_back();
				std::string mess_str(begin(message), end(message));
				message.push_back(L'\r');
				message.push_back(L'\n');

				this->messengeDB += (std::wstring(L"You:  ") + message);
				if (message.empty()) {
					MessageBox(this->hWinowMain, L"Input massage", L"Information", MB_ICONINFORMATION | MB_OK);
					break;
				}


				this->g_lock.lock();
				SetWindowText(this->hMessOut, this->messengeDB.c_str());
				this->g_lock.unlock();

				std::vector<long> mess_en;

				rsa.Encrypt(mess_en, mess_str);

				this->g_lock.lock();
				if (!server.Send_Message(mess_en, this->FriendLogin)) {
					this->g_lock.unlock();
					return 0;
				}

				this->g_lock.unlock();
				//EnableWindow(this->hMessButtonSEND, TRUE);
			}
			catch (...) {
				MessageBox(this->hWinowMain, L"ERROR", L"ERROR", MB_ICONINFORMATION | MB_OK);
			}
		}
		return 0;

		case WinRegApp::CTL_ID::BUTTON_MESS_FRIEND_COONECT_ID:
		{
			try {
				std::wstring FriendLogin_ws;
				bool fail = false;
				FriendLogin_ws.resize(GetWindowTextLength(this->hFriendLogin) + 1);
				GetWindowText(this->hFriendLogin, &FriendLogin_ws[0], FriendLogin_ws.size());
				FriendLogin_ws.pop_back();
				friend_name = FriendLogin_ws;
				this->FriendLogin = std::string(begin(FriendLogin_ws), end(FriendLogin_ws));

				SetWindowText(this->hButtonFriendConnect, L"Waiting friend");
				EnableWindow(this->hFriendLogin, FALSE);
				EnableWindow(this->hButtonFriendConnect, FALSE);

				if (!server.Send_Open_Key(rsa.MyOpenKey, FriendLogin)) {
					fail = true;
				}
				else {
					if (!server.Get_Open_Key(rsa.FriendOpenKey, FriendLogin))
						fail = true;
				}

				SetWindowText(this->hButtonFriendConnect, L"Connect");

				if (fail) {
					EnableWindow(this->hFriendLogin, TRUE);
					EnableWindow(this->hButtonFriendConnect, TRUE);
					return 0;
				}
				ShowWindow(this->hButtonFriendConnect, SW_HIDE);

				EnableWindow(this->hButtonFriendDisconnect, TRUE);
				ShowWindow(this->hButtonFriendDisconnect, SW_SHOW);

				EnableWindow(this->hMessIn, TRUE);
				EnableWindow(this->hMessOut, TRUE);
				EnableWindow(this->hMessButtonSEND, TRUE);

				/*MessageBox(this->hWinowMain, std::to_wstring(rsa.MyOpenKey.first).c_str(), L"ERROR", MB_ICONINFORMATION | MB_OK);
				MessageBox(this->hWinowMain, std::to_wstring(rsa.MyOpenKey.second).c_str(), L"ERROR", MB_ICONINFORMATION | MB_OK);*/

				this->HandleThread = CreateThread(NULL, 0, ThreadF, this, 0, &IdThread);
			}
			catch (...) {
				MessageBox(this->hWinowMain, L"ERROR", L"ERROR", MB_ICONINFORMATION | MB_OK);
			}
		}
		return 0;

		case WinRegApp::CTL_ID::BUTTON_MESS_FRIEND_DISCOONECT_ID:
		{
			if (!this->messengeDB.empty())
				this->messengeDB.clear();
			SetWindowText(this->hMessOut, this->messengeDB.c_str());


			EnableWindow(this->hButtonFriendDisconnect, FALSE);
			ShowWindow(this->hButtonFriendDisconnect, SW_HIDE);

			EnableWindow(this->hButtonFriendConnect, TRUE);
			ShowWindow(this->hButtonFriendConnect, SW_SHOW);

			EnableWindow(this->hMessIn, FALSE);
			EnableWindow(this->hMessOut, FALSE);
			EnableWindow(this->hMessButtonSEND, FALSE);
			EnableWindow(this->hFriendLogin, TRUE);
		}
		return 0;


		}
	}
	return 0;

	case WM_DESTROY:
	{
		PostQuitMessage(EXIT_SUCCESS);
	}
	return 0;

	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

unsigned long WinRegApp::ReadFriendMessages()
{
	std::vector<long> mess_en;
	std::string mess;

	while (IsWindowEnabled(this->hButtonFriendDisconnect)) {

		this->g_lock.lock();
		if (server.Read_Message(mess_en, this->FriendLogin)) {
			rsa.Decrypt(mess_en, mess);

			std::wstring mess_ws(begin(mess), end(mess));
			mess_ws.push_back(L'\r');
			mess_ws.push_back(L'\n');
			this->messengeDB += (friend_name + L"  " + mess_ws);

			SetWindowText(this->hMessOut, this->messengeDB.c_str());

			mess.clear();
			mess_en.clear();
			this->g_lock.unlock();
		}
		else {
			this->g_lock.unlock();
			Sleep(3000);
		}
	}

	return 0;
}