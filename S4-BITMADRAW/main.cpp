//====================================================================================
// 程序名称：S4-BITMADRAW
// 时间: 2022年6月29日
// 描述: 学习GDI中位图的绘制
//====================================================================================

#include <Windows.h>
#include <tchar.h>
#include <time.h>

#pragma comment(lib, "Msimg32.lib")

//====================================================================================
// 宏定义
//====================================================================================
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE L"绘制位图-遮蔽罩和透明"


//====================================================================================
// 全局变量声名
//====================================================================================
HDC g_hdc = NULL, g_mdc = NULL;		// 设备环境句柄
HBITMAP g_hBackgrand, g_hSpirite1, g_hSpirite2;		//图片句柄

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

//------------------------------------------------------------------------------------
// Game_Init 初始化函数
//------------------------------------------------------------------------------------
BOOL Game_Init(HWND hwnd)
{
	g_hdc = GetDC(hwnd);

	//加载位图
	g_hBackgrand = (HBITMAP)LoadImage(NULL, L"backgrand.bmp", IMAGE_BITMAP, WINDOW_WIDTH,
		WINDOW_HEIGHT, LR_LOADFROMFILE);
	g_hSpirite1 = (HBITMAP)LoadImage(NULL, L"Mario.bmp", IMAGE_BITMAP, 128, 64, LR_LOADFROMFILE);
	g_hSpirite2 = (HBITMAP)LoadImage(NULL, L"Mario2.bmp", IMAGE_BITMAP, 64, 64, LR_LOADFROMFILE);

	//建立兼容DC
	g_mdc = CreateCompatibleDC(g_hdc);

	Game_Paint(hwnd);
	ReleaseDC(hwnd, g_hdc);
	return TRUE;
}


//------------------------------------------------------------------------------------
// Game_Paint 绘制函数
//------------------------------------------------------------------------------------
VOID Game_Paint(HWND hwnd)
{
	//贴上背景图
	SelectObject(g_mdc, g_hBackgrand);
	BitBlt(g_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_mdc, 0, 0, SRCCOPY);

	//透明遮盖法
	SelectObject(g_mdc, g_hSpirite1);
	BitBlt(g_hdc, 100, 420, 64, 64, g_mdc, 64, 0, SRCAND);
	BitBlt(g_hdc, 100, 420, 64, 64, g_mdc, 0, 0, SRCPAINT);

	//透明色彩
	SelectObject(g_mdc, g_hSpirite2);
	TransparentBlt(g_hdc, 200, 420, 64, 64, g_mdc, 0, 0, 64, 64, RGB(0, 0, 0));
}

//------------------------------------------------------------------------------------
// Game_CleanUp 资源释放函数
//------------------------------------------------------------------------------------
BOOL Game_CleanUp(HWND hwnd)
{
	DeleteObject(g_hdc);
	DeleteDC(g_mdc);
	return TRUE;
}