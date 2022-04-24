// lab2.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "lab2.h"
#include <map>
#include <vector>
#include <time.h>
#include <algorithm>
#include <iostream>
#include <fstream>

#pragma comment(lib, "Msimg32.lib")
#define MAX_LOADSTRING 100
#define BRICK_SIZE 60
#define SPACING 4
#define WINCHILD_SIZE 68
#define CONFIG_FILE "2048.config"

using namespace std;

// Global Variables:   
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
WCHAR szChildClass[MAX_LOADSTRING];
WCHAR szCounterClass[MAX_LOADSTRING];
WCHAR szEndScreenClass[MAX_LOADSTRING];


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
ATOM                RegisterChildClass(HINSTANCE hInstance);
ATOM                RegisterCounterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    WndChildProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK    WndCounterProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK       EnumChildProc(HWND hWnd, LPARAM lParam);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

map<HWND, int> valuesMap;
map<HWND, int> openedTimers;
map<HWND, pair<int,int>> windowProperties;
map<HWND, double> scale;
int sum = 0;
int goal = 2048;
HWND hWnd1 = NULL;
HWND hWnd2 = NULL;
int gameStatus = 0;

map<int, COLORREF> colors_map{
	{0, RGB(204, 192, 174)},
	{2, RGB(238, 228, 198)},
	{4, RGB(239, 225, 218)},
	{8, RGB(243, 179, 124)},
	{16, RGB(246, 153, 100)},
	{32, RGB(246, 125, 98)},
	{64, RGB(247, 93, 60)},
	{128, RGB(237, 206, 116)},
	{256, RGB(239, 204, 98)},
	{512, RGB(243, 201, 85)},
	{1024, RGB(238, 200, 72)},
	{2048, RGB(239, 192, 47)}

};


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_LAB2, szWindowClass, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_LAB1CHILD, szChildClass, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_COUNTER, szCounterClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	RegisterChildClass(hInstance);
	RegisterCounterClass(hInstance);


	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB2));

	MSG msg;
	srand(time(NULL));


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
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2048));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(250, 247, 238));
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB2);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2048));

	return RegisterClassExW(&wcex);
}

ATOM RegisterChildClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndChildProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = nullptr;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(250, 247, 238));
	wcex.lpszClassName = szChildClass;
	wcex.hIconSm = nullptr;
	wcex.lpszMenuName = nullptr;

	return RegisterClassExW(&wcex);
}

ATOM RegisterCounterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndCounterProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = nullptr;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(250, 247, 238));
	wcex.lpszClassName = szCounterClass;
	wcex.hIconSm = nullptr;
	wcex.lpszMenuName = nullptr;

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
	hInst = hInstance; // Store instance handle in our global variable

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int windowSizeX = 4 * WINCHILD_SIZE + 2*SPACING;
	int windowSizeY = 5 * WINCHILD_SIZE;


	RECT rc;
	rc.top = 0;
	rc.bottom = windowSizeY;
	rc.left = 0;
	rc.right = windowSizeX;
	AdjustWindowRect(&rc, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME, true);



	HWND hWnd1 = CreateWindowW(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX/* | WS_CLIPCHILDREN*/,
		0,
		0,
		rc.right - rc.left,
		rc.bottom - rc.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr);


	if (!hWnd1)
	{
		return FALSE;
	}

	HWND hWnd2 = CreateWindowW(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX/*| WS_CLIPCHILDREN*/,
		0,
		0,
		rc.right - rc.left,
		rc.bottom - rc.top,
		hWnd1,
		nullptr,
		hInstance,
		nullptr);

	SetWindowLong(hWnd2, GWL_EXSTYLE, GetWindowLong(hWnd2, GWL_EXSTYLE) | WS_EX_LAYERED);

	ShowWindow(hWnd1, nCmdShow);
	UpdateWindow(hWnd1);

	ShowWindow(hWnd2, nCmdShow);
	UpdateWindow(hWnd2);

	return TRUE;
}


void CreateChildren(HWND hWnd, HWND table[][4], HWND* counter) {

	RECT rc;
	GetWindowRect(hWnd, &rc);
	int weight = rc.right - rc.left;
	int height = rc.bottom - rc.top;

	*counter = CreateWindowEx(
		0,
		szCounterClass,
		(LPCWSTR)NULL,
		WS_CHILDWINDOW,
		SPACING,
		0,
		4* WINCHILD_SIZE,
		WINCHILD_SIZE,
		hWnd,
		NULL,
		hInst,
		NULL
	);

	ShowWindow(*counter, SW_SHOW);
	UpdateWindow(*counter);


	for (int i = 1; i < 5; i++)
	{
		for (int j = 1; j < 5; j++)
		{


			table[i - 1][j - 1] = CreateWindowEx(
				0,
				szChildClass,
				(LPCWSTR)NULL,
				WS_CHILDWINDOW,
				SPACING + (i - 1) * WINCHILD_SIZE,
				j * WINCHILD_SIZE,
				WINCHILD_SIZE,
				WINCHILD_SIZE,
				hWnd,
				NULL,
				hInst,
				NULL
			);

			windowProperties.insert(make_pair(table[i - 1][j - 1], make_pair(i * SPACING + (i - 1) * BRICK_SIZE, (j + 1) * SPACING + j * BRICK_SIZE)));

			ShowWindow(table[i - 1][j - 1], SW_SHOW);
			UpdateWindow(table[i - 1][j - 1]);

		}
	}
}


void UpdateMap(HWND hWndTable[][4], int values[][4]) {

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{

			auto it = valuesMap.find(hWndTable[i][j]);

			if (it != valuesMap.end())
				it->second = values[i][j];
			else
				valuesMap.insert(make_pair(hWndTable[i][j], values[i][j]));
		}
	}

	InvalidateRect(hWnd1, NULL, FALSE);
	InvalidateRect(hWnd2, NULL, FALSE);
}

void paint_raw(HWND hWnd, double scale) {


	RECT rc;
	GetClientRect(hWnd, &rc);
	HDC hdc = GetDC(hWnd);
	COLORREF curColor = RGB(250, 247, 238);
	HBRUSH brush = CreateSolidBrush(curColor);
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
	HPEN pen = CreatePen(PS_SOLID, 1, curColor);
	HPEN oldPen = (HPEN)SelectObject(hdc, pen);

	Rectangle(hdc, 0, 0, WINCHILD_SIZE, WINCHILD_SIZE);

	SelectObject(hdc, oldBrush);
	DeleteObject(brush);
	SelectObject(hdc, oldPen);
	DeleteObject(pen);
	ReleaseDC(hWnd, hdc);
}

void paint_tile(HWND hWnd, double scale) {

	TCHAR buf[6];
	COLORREF color = colors_map[valuesMap[hWnd]];

	RECT rc;
	GetClientRect(hWnd, &rc);
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);
	HPEN pen = CreatePen(PS_SOLID, 1, color);
	HPEN oldPen = (HPEN)SelectObject(hdc, pen);
	HBRUSH brush = CreateSolidBrush(color);
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

	HFONT font = CreateFont(
		-MulDiv(14, GetDeviceCaps(hdc, LOGPIXELSY), 72), //Height
		0, //Width
		0, //Escapement
		0, //Orientation
		FW_BOLD, //Weight
		false, //Italic
		FALSE, //Underline
		0, //StrikeOut
		EASTEUROPE_CHARSET, //CharSet
		OUT_DEFAULT_PRECIS, //OutPrecision
		CLIP_DEFAULT_PRECIS, //ClipPrecision
		DEFAULT_QUALITY, //Quality
		DEFAULT_PITCH | FF_SWISS, //PitchAndFamily
		_T("Verdana") //Facename
	);

	SetTextColor(hdc, RGB(255, 255, 255));
	SetBkMode(hdc, TRANSPARENT);

	HFONT oldFont = (HFONT)SelectObject(hdc, font);

	RoundRect(hdc, (WINCHILD_SIZE - scale * BRICK_SIZE) / 2, (WINCHILD_SIZE - scale * BRICK_SIZE) / 2, scale * BRICK_SIZE, scale * BRICK_SIZE, 20, 20);

	if (valuesMap[hWnd]) {
		_stprintf_s(buf, 6, _T("%d"), valuesMap[hWnd]);
		DrawText(hdc, buf, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
	SelectObject(hdc, oldPen);
	DeleteObject(pen);
	SelectObject(hdc, oldBrush);
	DeleteObject(brush);
	SelectObject(hdc, oldFont);
	DeleteObject(font);
	EndPaint(hWnd, &ps);
}

void CreateRandomBricks(HWND hWndTableParent[][4], HWND hWndTableChild[][4], int values[][4]) {

	vector<pair<int, int>> tempVector;
	int counter = 0;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (values[i][j] == goal) {
				gameStatus = 1;
				return;
			}
			if (values[i][j] == 0) {
				counter++;
				tempVector.push_back(make_pair(i, j));
			}

		}
	}

	//endgame
	if (counter == 0) {
		gameStatus = 2;

		return;
	}

	int randomBrick = rand() % counter;
	int x = tempVector[randomBrick].first;
	int y = tempVector[randomBrick].second;
	auto it = valuesMap.find(hWndTableParent[x][y]);

	if (it != valuesMap.end())
		it->second = 2;

	it = valuesMap.find(hWndTableChild[x][y]);

	if (it != valuesMap.end())
		it->second = 2;

	values[x][y] = 2;


	openedTimers.insert(make_pair(hWndTableParent[x][y], 3));
	SetTimer(hWndTableParent[x][y], 3, USER_TIMER_MINIMUM, NULL);


	openedTimers.insert(make_pair(hWndTableChild[x][y], 3));
	SetTimer(hWndTableChild[x][y], 3, USER_TIMER_MINIMUM, NULL);

}


void killAnimations() {

	for_each(openedTimers.begin(), openedTimers.end(), [](pair<HWND, int> x) {

		KillTimer(x.first, x.second);
		auto it = scale.find(x.first);
		it->second = 1.0;
		paint_raw(x.first, scale[x.first]);
		//MoveWindow(x.first, windowProperties[x.first].first, windowProperties[x.first].second, BRICK_SIZE, BRICK_SIZE, FALSE);
		paint_tile(x.first, 1.0);
	});

	openedTimers.clear();

}

void move(HWND hWndTableParent[][4], HWND hWndTableChild[][4], int values[][4]) {

	killAnimations();

	UpdateMap(hWndTableParent, values);
	UpdateMap(hWndTableChild, values);
	CreateRandomBricks(hWndTableParent, hWndTableChild, values);

}

void animate(HWND hWndTableParent[][4], HWND hWndTableChild[][4], vector<pair<int, int>>& changedBricks, int count) {

	HWND curhWnd;

	for (int i = 0; i < count; i++)
	{
		curhWnd = hWndTableParent[changedBricks[i].first][changedBricks[i].second];
		openedTimers.insert(make_pair(curhWnd, 1));
		SetTimer(curhWnd, 1, USER_TIMER_MINIMUM, NULL);
		curhWnd = hWndTableChild[changedBricks[i].first][changedBricks[i].second];
		openedTimers.insert(make_pair(curhWnd, 1));
		SetTimer(curhWnd, 1, USER_TIMER_MINIMUM, NULL);

	}

	changedBricks.clear();
}


bool configExists() {

	ifstream inputfile(CONFIG_FILE);
	return inputfile.good();
}

void read_config(int values[][4]) {

	ifstream inputfile(CONFIG_FILE);

	CheckMenuItem(GetMenu(hWnd1), goal, MF_UNCHECKED);
	CheckMenuItem(GetMenu(hWnd2), goal, MF_UNCHECKED);

	inputfile >> goal;
	inputfile >> gameStatus;
	inputfile >> sum;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			inputfile >> values[i][j];
		}

	}

	CheckMenuItem(GetMenu(hWnd1), goal, MF_CHECKED);
	CheckMenuItem(GetMenu(hWnd2), goal, MF_CHECKED);

}

void save_config(int values[][4]) {

	ofstream outputfile;
	outputfile.open(CONFIG_FILE, ios::out | ios::trunc);


	if (!outputfile.good()) return;

	outputfile << goal << endl;
	outputfile << gameStatus << endl;
	outputfile << sum << endl;

	for (int i = 0; i < 4; i++)
	{	
		for (int j = 0; j < 4; j++)
		{
			outputfile << values[i][j] << endl;
		}

	}

	outputfile.close();
}


void updateGoal(int newGoal) {

	CheckMenuItem(GetMenu(hWnd1), goal, MF_UNCHECKED);
	CheckMenuItem(GetMenu(hWnd2), goal, MF_UNCHECKED);


	CheckMenuItem(GetMenu(hWnd1), newGoal, MF_CHECKED);
	CheckMenuItem(GetMenu(hWnd2), newGoal, MF_CHECKED);

	goal = newGoal;
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

	static int values[4][4] = {0};

	static HWND hWndTableParent[4][4];
	static HWND hWndCounterParent;

	static HWND hWndTableChild[4][4];
	static HWND hWndCounterChild;

	static int testEndGame = 0;

	static vector <pair<int, int>> changedState;

	static int windowSizeX = 4 * BRICK_SIZE;
	static int windowSizeY = 5 * BRICK_SIZE;

	switch (message)
	{
	case WM_CREATE:
	{
		if (hWnd1 == NULL) {
			hWnd1 = hWnd;
			CreateChildren(hWnd, hWndTableParent, &hWndCounterParent);


		}
		else {
			hWnd2 = hWnd;
			CreateChildren(hWnd, hWndTableChild, &hWndCounterChild);
			if (configExists()) {
				read_config(values);
				UpdateMap(hWndTableParent, values);
				UpdateMap(hWndTableChild, values);
			} else 
			move(hWndTableChild, hWndTableParent, values);
		}


		break;
	}
	case WM_MOVE:
	{
		int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);

		RECT rc;
		GetWindowRect(hWnd, &rc);
		int x = rc.right - rc.left;
		int y = rc.bottom - rc.top;

		HWND hWndSecond = hWnd == hWnd1 ? hWnd2 : hWnd1;

		MoveWindow(hWndSecond, screenWidth - rc.right, screenHeight - rc.bottom, rc.right - rc.left, rc.bottom - rc.top, TRUE);

		RECT screenCoords;
		GetWindowRect(hWnd, &screenCoords);
		int windowSizeX = screenCoords.right - screenCoords.left;
		int windowSizeY = screenCoords.bottom - screenCoords.top;

		RECT rcSecond;

		HWND parent = GetParent(hWnd1) == NULL ? hWnd2 : hWnd1;
		HWND child = parent == hWnd1 ? hWnd2 : hWnd1;

		RECT rcParent;
		RECT rcChild;
		GetWindowRect(hWnd, &rcParent);
		GetWindowRect(child, &rcChild);

		x = rcParent.right - rcParent.left;
		y = rcParent.bottom - rc.top;

		RECT box;
		box.top = (screenHeight - 1.95 * y) / 2;
		box.bottom = screenHeight - box.top;
		box.left = (screenWidth - 1.95 * x) / 2;
		box.right = screenWidth - box.left;

		if (rcParent.top >= box.top && rcParent.bottom <= box.bottom && rcParent.right >= box.left && rcParent.left <= box.right && rcParent.right <= box.right
			&& rcParent.left >= box.left)
		{
			SetLayeredWindowAttributes(hWnd2, 0, (255 * 50) / 100, LWA_ALPHA);
			ShowWindow(hWnd2, SW_SHOW);
		}
		else {
			SetLayeredWindowAttributes(hWnd2, 0, (255 * 100) / 100, LWA_ALPHA);
			ShowWindow(hWnd2, SW_SHOW);
		}

		break;
	}
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case 0x41: // A
		{
			if (gameStatus != 0) break;
			int flagMoved = 0;
			int counter = 0;

			for (int j = 0; j >= 0 && j < 4; j++)
			{
				bool joined[4] = { false };

				for (int i = 1; i >= 0 && i < 4; i++)
				{
					if (values[i][j] != 0) {
						if (values[i - 1][j] == 0) {
							values[i - 1][j] = values[i][j];
							values[i][j] = 0;
							if (i - 2 >= 0) i -= 2;
							flagMoved = 1;
						}
						else if (values[i][j] == values[i - 1][j] && joined[i - 1] == false) {
							values[i - 1][j] += values[i][j];
							values[i][j] = 0;
							sum += values[i - 1][j];
							flagMoved = 1;
							joined[i - 1] = true;
							changedState.push_back(make_pair(i - 1, j));
							counter++;
						}
					}
				}
			}
			if (flagMoved) {
				move(hWndTableChild, hWndTableParent, values);
				animate(hWndTableParent, hWndTableChild, changedState, counter);
				save_config(values);
				testEndGame = 0;
			}
			else testEndGame |= 1;

			if (testEndGame == 0xF) {
				gameStatus = 2;
				InvalidateRect(hWnd1, NULL, TRUE);
				InvalidateRect(hWnd2, NULL, TRUE);
			}


			break;
		}
		case 0x57: // W
		{
			if (gameStatus != 0) break;
			int flagMoved = 0;
			int counter = 0;


			for (int i = 0; i >= 0 && i < 4; i++)
			{
				bool joined[4] = { false };
				for (int j = 1; j >= 0 && j < 4; j++)
				{
					if (values[i][j] != 0) {
						if (values[i][j - 1] == 0) {
							values[i][j - 1] = values[i][j];
							values[i][j] = 0;
							if (j - 2 >= 0) j -= 2;
							flagMoved = 1;

						}
						else if (values[i][j - 1] == values[i][j] && joined[j - 1] == false) {
							values[i][j - 1] += values[i][j];
							values[i][j] = 0;
							sum += values[i][j - 1];
							joined[j - 1] = true;
							changedState.push_back(make_pair(i, j - 1));
							flagMoved = 1;
							counter++;


						}
					}
				}
			}
			if (flagMoved) {
				move(hWndTableChild, hWndTableParent, values);
				animate(hWndTableParent, hWndTableChild, changedState, counter);
				save_config(values);
				testEndGame = 0;
			}
			else testEndGame |= 2;

			if (testEndGame == 0xF) {
				gameStatus = 2;
				InvalidateRect(hWnd1, NULL, TRUE);
				InvalidateRect(hWnd2, NULL, TRUE);
			}



			break;
		}
		case 0x53: //S 
		{
			if (gameStatus != 0) break;
			int flagMoved = 0;
			int counter = 0;


			for (int i = 0; i >= 0 && i < 4; i++)
			{
				bool joined[4] = { false };
				for (int j = 2; j >= 0 && j < 4; j--)
				{
					if (values[i][j] != 0) {
						if (values[i][j + 1] == 0) {
							values[i][j + 1] = values[i][j];
							values[i][j] = 0;
							if (j + 2 < 4) j += 2;
							flagMoved = 1;
						}
						else if (values[i][j + 1] == values[i][j] && joined[j + 1] == false) {
							values[i][j + 1] += values[i][j];
							values[i][j] = 0;
							sum += values[i][j + 1];
							joined[j + 1] = true;
							changedState.push_back(make_pair(i, j + 1));
							flagMoved = 1;
							counter++;
						}
					}
				}
			}

			if (flagMoved) {
				move(hWndTableChild, hWndTableParent, values);
				animate(hWndTableParent, hWndTableChild, changedState, counter);
				save_config(values);
				testEndGame = 0;
			}
			else testEndGame |= 4;

			if (testEndGame == 0xF) {
				gameStatus = 2;
				InvalidateRect(hWnd1, NULL, TRUE);
				InvalidateRect(hWnd2, NULL, TRUE);
			}



			break;
		}
		case 0x44: //D
		{
			if (gameStatus != 0) break;
			int flagMoved = 0;
			int counter = 0;


			for (int j = 3; j >= 0 && j < 4; j--)
			{
				bool joined[4] = { false };

				for (int i = 2; i >= 0 && i < 4; i--)
				{
					if (values[i][j] != 0) {
						if (values[i + 1][j] == 0) {
							values[i + 1][j] = values[i][j];
							values[i][j] = 0;
							if (i + 2 < 4) i += 2;
							flagMoved = 1;
						}
						else if (values[i + 1][j] == values[i][j] && joined[i + 1] == false) {
							values[i + 1][j] += values[i][j];
							values[i][j] = 0;
							sum += values[i + 1][j];
							joined[i + 1] = true;
							changedState.push_back(make_pair(i + 1, j));
							flagMoved = 1;
							counter++;

						}
					}
				}
			}

			if (flagMoved) {
				move(hWndTableChild, hWndTableParent, values);
				animate(hWndTableParent, hWndTableChild, changedState, counter);
				save_config(values);
				testEndGame = 0;
			}
			else testEndGame |= 8;

			if (testEndGame == 0xF) {
				gameStatus = 2;
				InvalidateRect(hWnd1, NULL, TRUE);
				InvalidateRect(hWnd2, NULL, TRUE);
			}

			break;
		}
		default:
			break;
		}


		break;
	}
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_NEWGAME:
		{
			EnumChildWindows(hWnd1, EnumChildProc, NULL);
			EnumChildWindows(hWnd2, EnumChildProc, NULL);

			sum = 0;
			gameStatus = 0;
			testEndGame = 0;

			InvalidateRect(hWnd1, NULL, TRUE);
			InvalidateRect(hWnd2, NULL, TRUE);


			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					values[i][j] = 0;
				}
			}

			CreateChildren(hWnd1, hWndTableParent, &hWndCounterParent);
			CreateChildren(hWnd2, hWndTableChild, &hWndCounterChild);

			move(hWndTableChild, hWndTableParent, values);
			save_config(values);

			break;
		}
		case IDM_GOAL_8:
		{
			updateGoal(IDM_GOAL_8);
			save_config(values);
			break;
		}
		case ID_GOAL_16:
		{
			updateGoal(ID_GOAL_16);
			save_config(values);
			break;
		}
		case ID_GOAL_64:
		{
			updateGoal(ID_GOAL_64);
			save_config(values);
			break;
		}
		case ID_GOAL_2048:
		{
			updateGoal(ID_GOAL_2048);
			save_config(values);
			break;
		}
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);

		if (gameStatus == 0) break;


		hdc = GetDCEx(hWnd, 0, DCX_CACHE);


		HBRUSH brush = CreateSolidBrush(RGB(250, 247, 238));
		HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
		HPEN pen = CreatePen(PS_SOLID, 10, RGB(250, 247, 238));
		HPEN oldPen = (HPEN)SelectObject(hdc, pen);
		Rectangle(hdc, 0, 0, 1000, 1000);
		SelectObject(hdc, oldBrush);
		SelectObject(hdc, oldPen);
		DeleteObject(pen);
		DeleteObject(brush);
		ReleaseDC(hWnd, hdc);


		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (hWnd == hWnd1)
					InvalidateRect(hWndTableParent[i][j], NULL, TRUE);
				else
					InvalidateRect(hWndTableChild[i][j], NULL, TRUE);


			}
		}

		if (hWnd == hWnd1)
			InvalidateRect(hWndCounterParent, NULL, TRUE);
		else
			InvalidateRect(hWndCounterChild, NULL, TRUE);



		killAnimations();
		SetTimer(hWnd, 7, 1, NULL);

	}
	break;
	case WM_TIMER:
	{
		COLORREF color = RGB(0, 255, 0);

		if (gameStatus == 2)
			color = RGB(255, 0, 0);

		HDC hdc = GetDCEx(hWnd, 0, DCX_CACHE);

		// TODO: Add any drawing code that uses hdc here...
		HDC memDC = CreateCompatibleDC(hdc);
		HPEN pen = CreatePen(PS_SOLID, 10, color);
		HBRUSH brush = CreateSolidBrush(color);
		HBRUSH oldBrush = (HBRUSH)SelectObject(memDC, brush);
		HPEN oldPen = (HPEN)SelectObject(memDC, pen);

		HFONT font = CreateFont(
			-MulDiv(28, GetDeviceCaps(hdc, LOGPIXELSY), 72), //Height
			0, //Width
			0, //Escapement
			0, //Orientation
			FW_BOLD, //Weight
			false, //Italic
			FALSE, //Underline
			0, //StrikeOut
			EASTEUROPE_CHARSET, //CharSet
			OUT_DEFAULT_PRECIS, //OutPrecision
			CLIP_DEFAULT_PRECIS, //ClipPrecision
			DEFAULT_QUALITY, //Quality
			DEFAULT_PITCH | FF_SWISS, //PitchAndFamily
			_T("Verdana") //Facename
		);

		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkMode(hdc, TRANSPARENT);

		RECT rc;
		GetClientRect(hWnd, &rc);
		int sizeX = 2 * SPACING + 4* WINCHILD_SIZE;
		int sizeY = 5*WINCHILD_SIZE;
		POINT test;
		test.x = sizeX;
		test.y = sizeY;

		PhysicalToLogicalPoint(hWnd, &test);


		HFONT oldFont = (HFONT)SelectObject(hdc, font);


		HBITMAP bmap = CreateCompatibleBitmap(hdc, sizeX, sizeY);
		HBITMAP oldBmap = (HBITMAP)SelectObject(memDC, bmap);
		Rectangle(memDC, 0, 0, sizeX, sizeY);

		BLENDFUNCTION blendfun;
		blendfun.BlendOp = AC_SRC_OVER;
		blendfun.BlendFlags = 0;
		blendfun.SourceConstantAlpha = 70;
		blendfun.AlphaFormat = 0;

		AlphaBlend(hdc, 0, 0, sizeX, sizeY, memDC, 0, 0, sizeX, sizeY, blendfun);


		if (gameStatus == 1) {
			DrawText(hdc, _T("WIN!"), -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		}
		else {

			DrawText(hdc, _T("GAME OVER!"), -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}

		SelectObject(memDC, oldBrush);
		SelectObject(memDC, oldPen);
		SelectObject(memDC, oldBmap);
		DeleteObject(font);
		DeleteObject(pen);
		DeleteObject(bmap);
		DeleteObject(brush);
		ReleaseDC(hWnd, memDC);
		ReleaseDC(hWnd, hdc);
		DeleteDC(memDC);
		DeleteDC(hdc);

		save_config(values);

		KillTimer(hWnd, 7);

		break;
	}
	case WM_DESTROY:
		valuesMap.clear();
		openedTimers.clear();
		windowProperties.clear();
		scale.clear();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


LRESULT CALLBACK WndChildProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	case WM_CREATE:
	{
		scale.insert(make_pair(hWnd, 1.0));

		break;
	}
	case WM_TIMER:
	{

		RECT rc;
		RECT rcParent;
		GetWindowRect(GetParent(hWnd), &rcParent);
		GetWindowRect(hWnd, &rc);
		MapWindowPoints(HWND_DESKTOP, GetParent(hWnd), (LPPOINT)&rc, 2);

		const double maxScale = double(WINCHILD_SIZE) / (double)BRICK_SIZE;
		const double step = (maxScale - 1.0)/10;

		if (wParam == 1) {
			auto it = scale.find(hWnd);

			paint_raw(hWnd, 1.0);
			it->second += step;

			if (it->second >= maxScale) {
				it->second = 1.0;
				openedTimers.erase(hWnd);
				KillTimer(hWnd, 1);
				SetTimer(hWnd, 2, USER_TIMER_MINIMUM, NULL);
			}

		}
		if (wParam == 2) {
			auto it = scale.find(hWnd);
			paint_raw(hWnd, 1.0);
			it->second -= step;

			if (it->second <= 1 + step) {
				it->second = 1.0;
				openedTimers.erase(hWnd);
				KillTimer(hWnd, 2);
			}
		}

		if (wParam == 3) {
			auto it = scale.find(hWnd);
			if ((int)it->second == 1) {
				paint_raw(hWnd, 1.0);
				it->second = 0.1;
			}
			else it->second += 0.1;

			if ( (int)scale[hWnd] == 1) {

				it->second = 1.0;
				openedTimers.erase(hWnd);
				KillTimer(hWnd, 3);

			}

		}
			//MoveWindow(hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
		InvalidateRect(hWnd, NULL, TRUE);

		break;
	}
	case WM_PAINT:
	{
		//HERE
		paint_tile(hWnd, scale[hWnd]);

		break;

	}
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

LRESULT CALLBACK WndCounterProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	const int bufSize = 6;
	TCHAR buf[bufSize];

	switch (message)
	{
	case WM_PAINT:
	{
		COLORREF color = RGB(204, 192, 174);

		RECT rc;
		GetClientRect(hWnd, &rc);
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		HPEN pen = CreatePen(PS_SOLID, 1, color);
		HPEN oldPen = (HPEN)SelectObject(hdc, pen);
		HBRUSH brush = CreateSolidBrush(color);
		HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

		HFONT font = CreateFont(
			-MulDiv(14, GetDeviceCaps(hdc, LOGPIXELSY), 72), //Height
			0, //Width
			0, //Escapement
			0, //Orientation
			FW_BOLD, //Weight
			false, //Italic
			FALSE, //Underline
			0, //StrikeOut
			EASTEUROPE_CHARSET, //CharSet
			OUT_DEFAULT_PRECIS, //OutPrecision
			CLIP_DEFAULT_PRECIS, //ClipPrecision
			DEFAULT_QUALITY, //Quality
			DEFAULT_PITCH | FF_SWISS, //PitchAndFamily
			_T("Verdana") //Facename
		);

		HFONT oldFont = (HFONT)SelectObject(hdc, font);

		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkColor(hdc, color);

		RoundRect(hdc, SPACING, SPACING, 4*WINCHILD_SIZE - 2*SPACING, WINCHILD_SIZE - SPACING, 20, 20);
		_stprintf_s(buf, 6, _T("%d"), sum);
		DrawText(hdc, buf, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		DeleteObject(font);
		SelectObject(hdc, oldPen);
		DeleteObject(pen);
		SelectObject(hdc, oldBrush);
		DeleteObject(brush);
		SelectObject(hdc, oldFont);
		DeleteObject(font);
		EndPaint(hWnd, &ps);

		break;

	}
	case WM_CREATE:
	{
		break;
	}
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}


// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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


BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam)
{

	DestroyWindow(hWnd);

	return TRUE;
}