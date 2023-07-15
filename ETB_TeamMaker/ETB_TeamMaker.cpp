#define STRICT

#include "framework.h"
#include "ETB_TeamMaker.h"


// Global Variables:
HINSTANCE hInst;                                // Current instance.
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text.
WCHAR szWindowClass[MAX_LOADSTRING];            // The main window class name.


HWND hInfo;
HWND hPList;
HWND hTeamSize;
HWND hPInputArea;


int p_list_cursel = 0, _p_list_cursel = 0;

LPSTR p_cursel = new char[50];
LPSTR teamsize = (LPSTR)"4";

std::vector<LPSTR> p_list{};					// Player list.
std::vector<LPSTR> team_output{};				// Temporarily stores player names until they reach teamsize then it gets added to teams.
std::vector<std::vector<LPSTR>> teams{};		// Full team roster.
std::vector<LPSTR> _player_list{};  int _i = 0;	// Stores player names when entered temporarily.


bool isnum(std::string& str)
{
	for (char const& c : str) {

		// using the std::isdigit() function
		if (std::isdigit(c) == 0)
			return false;
	}
	return true;
}

void display_results()
{
	std::string result = "";

	for (unsigned int k = 0; k < teams.size(); k++) {
		result += "Team " + std::to_string(k + 1) + ':' + '\n';

		for (auto player : teams[k]) {
			result += std::string("").append(player);
		}

		result += '\n';
	}

	MessageBox(nullptr, std::wstring(result.begin(), result.end()).c_str(), TEXT("Team results:"), MB_OK);
}


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    CommonMsgBox(HWND, UINT, WPARAM, LPARAM);



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// code.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_ETBTEAMMAKER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ETBTEAMMAKER));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ETBTEAMMAKER));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_ETBTEAMMAKER);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}




//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	srand((unsigned int)time(NULL));

	hInst = hInstance; // Store instance handle in our global variable


	HWND hWnd = CreateWindowW(szWindowClass, szTitle,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,			// MAIN WINDOW
		CW_USEDEFAULT, LENGTH, WIDTH, HEIGHT, nullptr, nullptr, hInstance, nullptr);

	SetWindowTextA(hWnd, (LPCSTR)"Escape The Backrooms TeamMaker");


	HWND hWndListText = CreateWindowEx(														// PL TEXT
		WS_EX_CLIENTEDGE, TEXT("STATIC"),
		TEXT("Currently added players ->\nClick on a name to remove it."),
		WS_CHILD | WS_VISIBLE | WS_BORDER,
		0, 75, 200, 75, hWnd, nullptr, nullptr, nullptr);



	HWND hInputArea = CreateWindowEx(														// PLAYER INPUT AREA
		WS_EX_CLIENTEDGE, TEXT("EDIT"),
		nullptr, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | EM_SCROLLCARET,
		0, 25, 200, 50, hWnd, (HMENU)IDE_P_INPUT, nullptr, nullptr); hPInputArea = hInputArea;

	HWND hInputAreaText1 = CreateWindowEx(
		WS_EX_CLIENTEDGE, TEXT("STATIC"), TEXT("Enter player names here:"),					// PIA TEXT #1
		WS_CHILD | WS_VISIBLE | WS_BORDER, 0, 0, 200, 25, hWnd, nullptr, nullptr, nullptr);

	HWND hInputAreaText2 = CreateWindowEx(
		WS_EX_CLIENTEDGE, TEXT("STATIC"), TEXT("Press ENTER to confirm."),					// CONFIRM USERNAME INPUT BUTTON
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | BS_DEFPUSHBUTTON,
		200, 0, 200, 25, hWnd, (HMENU)IDB_CONFIRM, nullptr, nullptr);



	HWND hWndList = CreateWindowEx(
		WS_EX_CLIENTEDGE, TEXT("LISTBOX"),
		nullptr, WS_VSCROLL | WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL | LBS_NOTIFY,			// PLAYER LIST
		200, 25, 200, 160, hWnd, (HMENU)IDLB_P, GetModuleHandle(nullptr), nullptr); hPList = hWndList;



	HWND hTeamSizeArea = CreateWindowEx(													// TEAM SIZE AREA
		WS_EX_CLIENTEDGE, TEXT("EDIT"),
		nullptr, WS_CHILD | WS_VISIBLE | WS_BORDER,
		0, 175, 100, 25, hWnd, nullptr, nullptr, nullptr);
	SetWindowTextA(hTeamSizeArea, (LPCSTR)"4"); hTeamSize = hTeamSizeArea;

	HWND hTeamSizeInfo = CreateWindowEx(													// TEAM SIZE INFO
		WS_EX_CLIENTEDGE, TEXT("STATIC"), TEXT("Input team size down below."),
		WS_CHILD | WS_VISIBLE | WS_BORDER,
		0, 150, 200, 25, hWnd, nullptr, nullptr, nullptr);

	HWND hTeamSizeConfirm = CreateWindowEx(
		WS_EX_CLIENTEDGE, TEXT("BUTTON"), TEXT("Change"),									// TEAM SIZE CHANGE BUTTON
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | BS_DEFPUSHBUTTON,
		100, 175, 100, 25, hWnd, (HMENU)IDB_CHANGE_TEAMSIZE, nullptr, nullptr);



	HWND hRandomizeTeam = CreateWindowEx(
		WS_EX_CLIENTEDGE, TEXT("BUTTON"), TEXT("Randomize teams!"),							// BUTTON TO RANDOMIZE TEAMS
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | BS_DEFPUSHBUTTON,
		200, 175, 200, 25, hWnd, (HMENU)IDB_CREATE_RANDOM, nullptr, nullptr);



	HWND hInfoBar = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("STATIC"), (LPCWSTR)(const char*)TEXT(""),// INFO BAR 
		WS_CHILD | WS_VISIBLE | WS_BORDER, 0, 200, WIDTH - 15, 25, hWnd, nullptr, nullptr, nullptr); hInfo = hInfoBar;



	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}



//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);

		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
		{
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, CommonMsgBox);
			break;
		}

		case IDM_CHANGELOG:
		{
			DialogBox(hInst, MAKEINTRESOURCE(IDM_CHANGELOG), hWnd, CommonMsgBox);
			break;
		}

		case IDM_EXIT:
		{
			DestroyWindow(hWnd);
			break;
		}

		case IDE_P_INPUT:
		{
			_player_list.push_back(new char[50]); _i++;

			GetWindowTextA(hPInputArea, _player_list.back(), 50);

			LPSTR p_name = _player_list.back();

			if (p_name[strlen(p_name) - 1] == '\n')
			{
				SetWindowTextA(hPInputArea, STR_NOTHING);

				if (strlen(p_name) == 2 && p_name[0] == '\r' && p_name[1] == '\n') {
					break;
				}

				if (_player_list.size() == 4096) {
					SetWindowTextA(hInfo, (LPCSTR)"Cannot add more than 4096 players to the list!");

					std::thread clear_box(
						[]() -> void {
							Sleep(5000);
							SetWindowTextA(hInfo, STR_NOTHING);
						});
					clear_box.detach();

					break;
				}


				for (auto x : p_list)
					if (strcmp(x, p_name) == 0) {
						SetWindowTextA(hInfo, (LPCSTR)"Name already exists on the list!");

						std::thread clear_box(
							[]() -> void {
								Sleep(3000);
								SetWindowTextA(hInfo, STR_NOTHING);
							});
						clear_box.detach();

						goto WndProcEND;
					}


				if (p_name != "" && strlen(p_name) != 0) {
					p_list.push_back(p_name);

					int tMsg = 0;
					bool f = false;

					try
					{
						tMsg = SendMessage(hPList, LB_ADDSTRING, 0, (LPARAM)(std::wstring(std::string(p_name).begin(), std::string(p_name).end())).c_str());
					}
					catch (...)
					{
						f = true;
					}
					if (!f)
					{
						SendMessage(hPList, LB_SETITEMDATA, tMsg, 0);
					}
				}
				else {
					SetWindowTextA(hInfo, (LPCSTR)"Player name cannot be empty!");

					std::thread clear_box(
						[]() -> void {
							Sleep(2000);
							SetWindowTextA(hInfo, STR_NOTHING);
						}); clear_box.detach();
				}


				break;
			}

			break;
		}

		case IDB_CREATE_RANDOM:
		{
			if (p_list.size() < (unsigned int)atoi(teamsize)) {

				SetWindowTextA(hInfo, (LPCSTR)"Too little names to build teams!");

				std::thread clear_box(
					[]() -> void {
						Sleep(5000);
						SetWindowTextA(hInfo, STR_NOTHING);
					});
				clear_box.detach();

				break;
			}

			while (true) {

				if (team_output.size() == atoi(teamsize)) {
					teams.push_back(team_output);
					team_output = {};


					if (p_list.size() < (unsigned int)atoi(teamsize) && p_list.size() != 0) {
						teams.push_back(p_list);
						display_results();

						break;
					}

					if (p_list.size() == 0) {
						display_results();

						break;
					}
				}

				LPSTR pick = p_list[rand() % p_list.size()];
				p_list.erase(std::find(p_list.begin(), p_list.end(), pick));
				team_output.push_back(pick);
			}

			// Clear out everything.
			_player_list.clear();
			p_list.clear();
			teams.clear();
			p_list_cursel = 0;
			_p_list_cursel = 0;
			p_cursel = new char[50];
			SendMessage(hPList, LB_RESETCONTENT, 0, 0);
			SetWindowTextA(hInfo, STR_NOTHING);
			SetWindowTextA(hPInputArea, STR_NOTHING);


			break;
		}

		case IDB_CHANGE_TEAMSIZE:
		{

			_player_list.push_back(new char[50]); _i++;

			GetWindowTextA(hTeamSize, _player_list.back(), 50);
			LPSTR p_name = _player_list.back();

			if (!isnum(std::string("").append(p_name)) || atoi(p_name) < 2 || atoi(p_name) > 12) {

				SetWindowTextA(hInfo, (LPCSTR)"Invalid input for teamsize!");
				SetWindowTextA(hTeamSize, teamsize);

				std::thread clear_box(
					[]() -> void {
						Sleep(3000);
						SetWindowTextA(hInfo, STR_NOTHING);
					});
				clear_box.detach();

				break;
			}

			teamsize = p_name;

			SetWindowTextA(hInfo, std::string("Changed team size to: ").append(p_name).c_str());

			std::thread clear_box(
				[]() -> void {
					Sleep(4000);
					SetWindowTextA(hInfo, STR_NOTHING);
				});
			clear_box.detach();

			break;
		}

		case IDLB_P:
		{
			DWORD index = SendMessage(hPList, LB_GETCURSEL, 0, 0); p_list_cursel = index;

			if (index > 4096) {
				break;
			}

			_player_list.push_back(new char[50]); _i++; _p_list_cursel = _i - 1;
			LPSTR p_name = _player_list.back();

			SendMessage(hPList, LB_GETTEXT, index, (LPARAM)p_name);				// Get text.

			if (strlen(p_name) > 40) {
				break;
			}


			//SetWindowTextA(hInfo, std::string("Currently selected: ").append((std::string)p_name).c_str());

			p_list.erase(p_list.begin() + p_list_cursel);
			SendMessage(hPList, LB_DELETESTRING, (WPARAM)p_list_cursel, 0);

			//std::thread clear_box(
			//	[]() -> void {
			//		//Sleep(3000);
			//		SetWindowTextA(hInfo, STR_NOTHING);
			//	});
			//clear_box.detach();

			break;
		}

		default:
			return DefWindowProcW(hWnd, message, wParam, lParam);			// DefWindowProcW for ASCII support
		}
	}

	break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
	}
	break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProcW(hWnd, message, wParam, lParam);				// DefWindowProcW for ASCII support
	}

WndProcEND:
	return 0;
}



// Message handlers for information boxes.
INT_PTR CALLBACK CommonMsgBox(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
