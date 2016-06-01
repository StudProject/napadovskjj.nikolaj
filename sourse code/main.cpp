#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <stdio.h>
#include <math.h>
#include <Windowsx.h>
#include <ctime>
#include "Model.h"

/**
@mainpage Simulation of a bullet flight.
@authors Napadovskiy Nikolai.
@version 2.0

@full The application simulates bullet flight path, depending on the given initial parameters: the initial speed and altitude, the flight angle and wind speed.
After drawing the trajectory displays the distance traversed by a shell, and the average time of flight.
*/

/**
@class Main
*/

HBITMAP hBitmap = NULL;
// Global variables
#define IDC_EDIT1 10000
#define IDC_BUTTON1 10001
#define IDC_EDIT2 10003
#define IDC_EDIT3 10004
#define IDC_BUTTON2 10002



HWND Button, edit1, edit2, edit3, Button1; 

/// The class name of the main window
static TCHAR szWindowClass[] = _T("win32app");

/// The string that appears in the application's title bar.
static TCHAR szTitle[] = _T("Симулятор полета пули. Нападовский Николай группа 5441");

HINSTANCE hInst;

/// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void Physics(ULONGLONG dT);
void Start();
bool pause = false;
int A = 0;
int B = 0;
int qqq = 0;
int ggg = 0;

/**
@function WinMain
@brief The main function.
@full The main function is the entry point into the program and is responsible for drawing window.
@param hInstance Handle to the current instance of the window
@param hPrevInstance Handle to the previous instance of the window
@param lpCmdLine A pointer to the command line
@param nCmdShow Shows window state
@return Exit the application code
*/
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
	
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL,
            _T("Call to RegisterClassEx failed!"),
            _T("Win32 Guided Tour"),
            NULL);

        return 1;
    }

    hInst = hInstance;

	/**
	@function CreateWindow
	@brief The function that creates the window.
	@param szWindowClass The class name
	@param szTitle Window Name
	@param WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_CLIPCHILDREN | WS_POPUP Display Modes window
	@param CW_USEDEFAULT Window position on the x and y axis
	@param 1110/600 The width and height of the window
	@param NULL Handle to the parent window and menu handle
	@param hInstance Handle to application instance
	@param NULL Parameters transmitted from the WndProc
	*/
    HWND hWnd = CreateWindow(
        szWindowClass,
        szTitle,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_CLIPCHILDREN,
        CW_USEDEFAULT, CW_USEDEFAULT,
        1022, 600,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hWnd)
    {
        MessageBox(NULL,
            _T("Call to CreateWindow failed!"),
            _T("Win32 Guided Tour"), 
            NULL);

        return 1;
    }
	/// Creating edits for user input parameters (initial speed and altitude, the flight angle and wind speed) and buttons.
	 edit1 = CreateWindowEx(WS_EX_CLIENTEDGE, _T("edit"), _T(""), WS_BORDER|  WS_VISIBLE| WS_CHILD, //создаем три поля ввода
        100, 10, 100, 30, hWnd, (HMENU)IDC_EDIT1, hInst, NULL);
	 edit2 = CreateWindowEx(WS_EX_CLIENTEDGE, _T("edit"), _T(""), WS_BORDER|  WS_VISIBLE| WS_CHILD, //создаем три поля ввода
        75, 50, 100, 30, hWnd, (HMENU)IDC_EDIT2, hInst, NULL);
	 edit3 = CreateWindowEx(WS_EX_CLIENTEDGE, _T("edit"), _T(""), WS_BORDER|  WS_VISIBLE| WS_CHILD, //создаем три поля ввода
        150, 90, 100, 30, hWnd, (HMENU)IDC_EDIT3, hInst, NULL);
	 Button = CreateWindowEx(0, _T("button"), _T("Начать моделирование"), WS_BORDER|  WS_VISIBLE| WS_CHILD, 
        10, 130, 185, 30, hWnd, (HMENU)IDC_BUTTON1, hInst, NULL);
	 Button1 = CreateWindowEx(0, _T("button"), _T("Пауза/Продолжить"), WS_BORDER|  WS_VISIBLE| WS_CHILD, 
        210, 130, 160, 30, hWnd, (HMENU)IDC_BUTTON2, hInst, NULL);
    
	 /**
	 @function ShowWindow
	 @brief Function to display the window.
	 @param hWnd Window Handle
	 @param nCmdShow Window Display Mode
	 */
    ShowWindow(hWnd,
        nCmdShow);
	/**
	@function UpdateWindow
	@brief Function to update the window.
	@param hWnd Window Handle
	*/
    UpdateWindow(hWnd);

    /// Main message loop:
    MSG msg;

	ULONGLONG prevTime = -1, curTime, deltaT;
	/// Call Start function
	Start();
/// Enter the infinite message loop
while(TRUE)
{
    /// Check to see if any messages are waiting in the queue
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        /// Translate the message and dispatch it to WindowProc()
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    /// If the message is WM_QUIT, exit the while loop
    if(msg.message == WM_QUIT)
        break;

	/// Removing the number of milliseconds that have elapsed since the system was launched
	curTime = GetTickCount64();
	/// Determination of the time change
	if (prevTime == -1)
		deltaT = 0;
	else deltaT = curTime - prevTime;
	prevTime = curTime;
	if (!pause) 
		{ 
			/// Calling a function is responsible for the application physics
			Physics(deltaT);
		}
    
	/// Calling a function that initiates a redraw window
	InvalidateRect(hWnd,NULL,1);
}

    return (int) msg.wParam;
}

/**
@function Start
@brief Start function.
@full The function responsible for initializing variables when the user presses the "Start".
*/
void Start() 
{
	ch.dist = 0;/// Distance
	ch.x = 30;  /// Coordinate of the bullet on the x-axis
	ch.y = 529; /// Coordinate of the bullet on the y-axis
	ch.vx = 0;/// Bullet speed along the x axis
	ch.vy = 0;/// Bullet speed along the y axis
	ch.pos = 0;//Переменная для разделения блоков дистанций
	ch.p = 1;//Промежуточная переменная-счетчик
	
}

/**
@function Physics
@brief The function responsible for the application physics.
@full Function responsible for allowing applications and physics calculate projectile velocity along the axes of abscissas and ordinates at any given time, and its coordinates on these axes.
@full The function code is as follows:
@code
void Physics(ULONGLONG dT)
{
	if(ggg == 1)
	{
		if(ch.y <= 530)
		{

			if(qqq == 0)
			{
				ch.vy = ch.vx0*(-0.75);
				qqq = 1;
			}

			if ((ch.alpha >= 0) && (ch.alpha <= 90))
			{ 
				ch.x += (ch.vx0-ch.wind)*((double)dT / 1000)*cos(ch.alpha*PI/180);
				
				ch.vy += ((double)dT/1000)*G;
				
				ch.y += ch.vy*((double)dT / 1000)*sin(ch.alpha*PI/180);
				
				if(ch.y >= 530) {
					ch.end_time = clock();
					ch.search_time = ch.end_time - ch.start_time;
				}
			}
		}
	}
}
@endcode
*/
void Physics(ULONGLONG dT)
{	
	if(ggg == 1)//Ожидание нажатия клавиши для старта моделирования
	{
			if(ch.y <= 530)
		{
				
			if(qqq == 0) 
			{
				ch.vy = ch.vx0*(-0.75);
				qqq = 1;
			}
			
			if ((ch.alpha >= 0) && (ch.alpha <= 90))
			{ //полет при углах от 0 до 180 градусов 
					
			  /// Bullet in the x-coordinate at each time point
					ch.x += (ch.vx0-ch.wind)*((double)dT / 1000)*cos(ch.alpha*PI/180); 
					/// The speed of the bullet on the y axis
					ch.vy += ((double)dT/1000)*G;
					/// Bullet in the y-coordinate at each time point
					ch.y += ch.vy*((double)dT / 1000)*sin(ch.alpha*PI/180);
					/// The calculation of flight time
					if(ch.y >= 530) {
						ch.end_time = clock();
						ch.search_time = ch.end_time - ch.start_time;
					}
			}	
			
		}
	}
}

/**
@function WndProc
@brief Function producing processing messages for the window.
@full This function handles the many messages that the application receives from the operating system. For example, in the application dialog box, there is a button "Start", on pressing which the operating system sends a message to the application that the button was pressed. WndProc function is responsible for responding to this event. In this application, the appropriate response to this event will be the beginning of the start of the simulation.
@param hWnd Window handle
@param message Message ID
@param wParam The first parameter of message
@param lParam Second parameter of message
*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
    HDC hdc;
	HBRUSH hBrush = 0;
	HPEN hPen, hOldPen;
    int width,height;
	HDC backbuffDC;
	HBITMAP backbuffer;
	int savedDC;
	/// Titles before entry fields
	TCHAR vx[] = _T("Скорость="); //Заголовки перед полями ввода
	TCHAR vx2[] = _T("м/с");
	TCHAR vy[] = _T("Угол=");
	TCHAR vy2[] = _T("Град.");
	TCHAR windage[] = _T("Скорость ветра=");
	TCHAR windage2[] = _T("м/с (макс. 10 м/с)");
	TCHAR gorizont[] = _T("Линия горизонта");
	char speed[100];//скорость
	char angale[100];//угол
	char windage1[100];//скорость ветра
	int sped = 0;//промежуточная переменная скорости
	int wind = 0;//промежуточная переменная скорости ветра
	int angl = 0;//промежуточная переменная угла
	int restartGame = 0	;//переменная для рестарта игры
    switch (message)
    {
		/// Report on the creation of the window
	case WM_CREATE:
		/// Download the 24-bit .bmp image as a background
		hBitmap = (HBITMAP)LoadImage(hInst, L"1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE); 
		break;
		 
		
		/// Posted by menu command or control element
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1:
			switch(HIWORD(wParam))
			{
				/// Message to press the "Start" button
			case BN_CLICKED:
				/// Getting the text from the input fields
				GetWindowTextA(edit1,speed,99);
				GetWindowTextA(edit2,angale,99);
				GetWindowTextA(edit3,windage1,99);
							
				/// Translation of rows of data in numbers and write them in variables
				sscanf(speed,"%d",&sped);
				ch.vx0 = sped; 
				sscanf(angale,"%d",&angl);
				ch.alpha = angl;
				sscanf(windage1,"%d",&wind);
				ch.wind = wind;
				ggg = 1;

				ch.start_time = clock();

				break;
			}
			break;
		}

		switch (LOWORD(wParam))//запрос на повтор
		{
		case IDC_BUTTON2:
			switch(HIWORD(wParam))
			{
				/// A key message "Pause"
			case BN_CLICKED:
				{
					pause = !(pause); 
					break;
				}
				
			}
		
		}
		

		/// Posted on drawing elements within the window
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		RECT rect;
		HDC hdcMem; 
		HGDIOBJ oldBitmap;
		BITMAP bitmap;

		hdcMem = CreateCompatibleDC(hdc); 
		oldBitmap = SelectObject(hdcMem, hBitmap);

		GetObject(hBitmap, sizeof(bitmap), &bitmap); 
		BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY); 
	
		/// Bullet drawing
		Ellipse(hdc,ch.x-CHAR_R,ch.y-CHAR_R,ch.x+CHAR_R,ch.y+CHAR_R);
		
		/// Printing text
		WCHAR s[100];
		WCHAR w[100];
		WCHAR q[100];

		TextOut(hdc,
            10, 17,
            vx, _tcslen(vx));
		TextOut(hdc,
            220, 17,
            vx2, _tcslen(vx2));
		TextOut(hdc,
            10, 57,
            vy, _tcslen(vy));
		TextOut(hdc,
            200, 57,
            vy2, _tcslen(vy2));
		TextOut(hdc,
            10, 97,
            windage, _tcslen(windage));
		TextOut(hdc,
            270, 97,
            windage2, _tcslen(windage2));
		TextOut(hdc,
            650, 535,
            gorizont, _tcslen(gorizont));
				
		MoveToEx(hdc, 30, 530, NULL); //сделать текущими координаты x1, y1
		LineTo(hdc,980,530);
		MoveToEx(hdc, 980, 530, NULL); //сделать текущими координаты x1, y1
		LineTo(hdc,960,520);
		MoveToEx(hdc, 980, 530, NULL); //сделать текущими координаты x1, y1
		LineTo(hdc,960,540);


		MoveToEx(hdc, 30, 530, NULL); //сделать текущими координаты x1, y1
		LineTo(hdc,30,200);	
		MoveToEx(hdc, 30, 200, NULL); //сделать текущими координаты x1, y1
		LineTo(hdc,40,220);	
		MoveToEx(hdc, 30, 200, NULL); //сделать текущими координаты x1, y1
		LineTo(hdc,20,220);	

		if(ch.y >= 530 && ch.x != 30)	{
				
			MoveToEx(hdc, ch.x, ch.y, NULL);
			LineTo(hdc,ch.x,ch.y-40);
			MoveToEx(hdc,ch.x,ch.y-40,NULL);
			LineTo(hdc,ch.x-10,ch.y-40);
			MoveToEx(hdc,ch.x,ch.y-40,NULL);
			LineTo(hdc,ch.x+10,ch.y-40);

			ch.vx1 = ch.vx0 - ch.wind;
			ch.dist = ((ch.vx1*ch.vx1)*(2*sin(ch.alpha*PI/180)*cos(ch.alpha*PI/180)))/G;

			wsprintf(s,_T("%dм"),(int)ch.dist); /// Distance
			TextOut(hdc,ch.x-12,ch.y-60,s,wcslen(s));
			wsprintf(q,_T("%dс"),(int)(ch.search_time)/1000); /// Time of a flight
			TextOut(hdc,ch.x-12,ch.y-80,q,wcslen(q));

			if(ch.x > 1000)
			{
				
				if(ch.pos < 300){
			wsprintf(s,_T("Дистанция %d = %d"),(int)ch.p,(int)ch.dist); /// Distance
			TextOut(hdc,470,20+ch.pos,s,wcslen(s));
			
			wsprintf(s,_T("Время %d = %d"),(int)ch.p,(int)(ch.search_time)/1000); /// Time of a flight
			TextOut(hdc,630,20+ch.pos,s,wcslen(s));
					
				}
			}

			/// Time of a flight
					sped = 0;//обнуляем все переменные
					angl = 0;
					wind = 0;
					ch.dist = 0;
					ch.x = 30;  
					ch.y = 530;  
					ch.vx = 0;
					ch.vy = 0;
					ch.vx0 = 0;
					ch.alpha = 0;
					ch.wind = 0;
					ch.start_time = 0;
					ch.end_time = 0;
					ch.search_time = 0;
					ch.pos = ch.pos + 20;
					ch.p++;
					ggg = 0;
					qqq = 0;

		}
		
			
		/// End drawing
        EndPaint(hWnd, &ps);
        break;
		
		/// Post Uninstall window background
	case WM_ERASEBKGND:
		return 1;

		/// The message about the destruction of the window
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }

    return 0;
}