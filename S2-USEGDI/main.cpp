#include <Windows.h>
#include <tchar.h>
#include <time.h>

//====================================================================================
// 宏定义
//====================================================================================
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE L"标题"


//====================================================================================
// 全局变量声名
//====================================================================================
HDC g_hdc;	// 设备环境句柄
HPEN g_hPen[7] = { 0 };
HBRUSH g_hBrush[7] = { 0 };
int g_iPenStyle[7] = { PS_SOLID, PS_DASH,PS_DOT,PS_DASHDOT,PS_DASHDOTDOT,
PS_NULL,PS_INSIDEFRAME };
int g_iBrushStyle[7] = { HS_VERTICAL,HS_HORIZONTAL,HS_CROSS,HS_DIAGCROSS,
HS_FDIAGONAL,HS_BDIAGONAL };


//====================================================================================
// 全局函数声名
//====================================================================================
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL Game_Init(HWND hwnd);
VOID Game_Paint(HWND hwnd);
BOOL Game_CleanUp(HWND hwnd);

//------------------------------------------------------------------------------------
// WinMain主函数
//------------------------------------------------------------------------------------
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	WNDCLASSEX wndClass = { 0 };
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;			//窗口的样式
	wndClass.lpfnWndProc = WndProc;						//窗口的消息处理函数
	wndClass.cbClsExtra = 0;							//窗口类的附加内存
	wndClass.cbWndExtra = 0;							//窗口的附加内存
	wndClass.hInstance = hInstance;
	wndClass.hIcon = (HICON)::LoadImage(NULL, _T("icon.ico"), IMAGE_ICON,
		0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);					//图标资源
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);					//箭头光标
	wndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);	//窗口类的背景画笔刷句柄(窗体发生重绘的时候使用这里指定的画笔来执行重绘)
	wndClass.lpszMenuName = NULL;									//菜单资源的名字
	wndClass.lpszClassName = _T("WD_NAME");						//窗口类的名字

	// 注册窗口
	if (!RegisterClassEx(&wndClass))
		return -1;

	// 创建窗口
	HWND hwnd = CreateWindow(_T("WD_NAME"), WINDOW_TITLE,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH,
		WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

	MoveWindow(hwnd, 200, 50, WINDOW_WIDTH, WINDOW_HEIGHT, true);			// 设置窗口初始位置
	ShowWindow(hwnd, nCmdShow);												// 显示窗口
	UpdateWindow(hwnd);														// 更新窗口

	//游戏资源的初始化
	if (!Game_Init(hwnd))
	{
		MessageBox(hwnd, L"游戏资源初始失败", L"消息窗口", 0);
		return FALSE;
	}

	//循环消息过程
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);	//将虚拟按键消息转化为字符消息
			DispatchMessage(&msg);	//分发一个消息给窗口程序
		}
	}
	// 注销
	UnregisterClass(L"WD_NAME", wndClass.hInstance);

	return 0;
}

//------------------------------------------------------------------------------------
// 窗口过程函数
//------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT paintStruct;

	switch (message)
	{
	case WM_PAINT:		//客户区重绘消息
		g_hdc = BeginPaint(hwnd, &paintStruct);
		Game_Paint(hwnd);
		EndPaint(hwnd, &paintStruct);
		ValidateRect(hwnd, NULL);
		break;
	case WM_KEYDOWN:	//键盘被摁下事件
		if (wParam == VK_ESCAPE)
			DestroyWindow(hwnd);
		break;
	case WM_DESTROY:	//窗口销毁信息
		//游戏资源清理函数
		Game_CleanUp(hwnd);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
		break;
	}
	return 0;
}

//====================================================================================
// 函数实现部分
//====================================================================================


BOOL Game_Init(HWND hwnd)
{
	g_hdc = GetDC(hwnd);
	srand((unsigned)time(NULL));	//初始化时间种子

	//随机初始笔和笔刷的样式
	for (int i = 0; i < 6; i++)
	{
		g_hPen[i] = CreatePen(g_iPenStyle[i], 1, RGB(rand() % 256, rand() % 256, rand() % 256));
		if (i == 6)
			g_hBrush[i] = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
		else
			g_hBrush[i] = CreateHatchBrush(g_iBrushStyle[i], RGB(rand() % 256, rand() % 256, rand() % 256));
	}

	Game_Paint(hwnd);
	ReleaseDC(hwnd, g_hdc);
	return TRUE;
}

VOID Game_Paint(HWND hwnd)
{
	int y = 0;

	for (int i = 0; i < 6; i++)
	{
		y = (1 + i) * 70;
		SelectObject(g_hdc, g_hPen[i]);		//选择笔
		MoveToEx(g_hdc, 30, y, NULL);
		LineTo(g_hdc, 100, y);
	}

	int x1 = 120;
	int x2 = 190;

	for (int i = 0; i < 6; i++)
	{
		SelectObject(g_hdc, g_hBrush[i]);
		Rectangle(g_hdc, x1, 70, x2, y);
		x1 += 90;
		x2 += 90;
	}
}

BOOL Game_CleanUp(HWND hwnd)
{
	for (int i = 0; i < 6; i++)
	{
		DeleteObject(g_hPen[i]);
		DeleteObject(g_hBrush[i]);
	}

	return TRUE;
}