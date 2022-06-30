//====================================================================================
// 程序名称：S6-GAMECYCLE
// 时间: 2022年6月30日
// 描述: 学习游戏循环
//====================================================================================

#include <Windows.h>
#include <tchar.h>
#include <time.h>

//====================================================================================
// 宏定义
//====================================================================================
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE L"Timer Animation"


//====================================================================================
// 全局变量声名
//====================================================================================
HDC g_hdc = NULL, g_mdc = NULL;		// 设备环境句柄 DC句柄
DWORD g_tPre = 0, g_tNow = 0;		// 记录绘制图片的时间
HBITMAP g_hSpirite[5];				// 位图数组,用来存储各人物位图
int g_iNum = 0;						// 当前显示的图片编号


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
	wndClass.lpszClassName = _T("WD_NAME");							//窗口类的名字

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
			TranslateMessage(&msg);	// 将虚拟按键消息转化为字符消息
			DispatchMessage(&msg);	// 分发一个消息给窗口程序
		}
		else
		{
			g_tNow = GetTickCount();	// 获取当前系统时间
			if (g_tNow - g_tPre >= 100)
			{
				Game_Paint(hwnd);
			}
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

	wchar_t filename[20];
	//载入动画位图
	for (int i = 0; i < 5; i++)
	{
		memset(filename, 0, sizeof(filename));
		swprintf_s(filename, L"Animation%d.bmp", (i + 1));

		g_hSpirite[i] = (HBITMAP)LoadImage(NULL, filename, IMAGE_BITMAP, WINDOW_WIDTH, WINDOW_HEIGHT, LR_LOADFROMFILE);
	}

	//建立兼容DC
	g_mdc = CreateCompatibleDC(g_hdc);

	g_iNum = 0;						//初始的图像显示为0
	return TRUE;
}


//------------------------------------------------------------------------------------
// Game_Paint 绘制函数
//------------------------------------------------------------------------------------
VOID Game_Paint(HWND hwnd)
{
	//处理图号
	if (g_iNum == 5)
		g_iNum = 0;

	//根据图号来贴图
	SelectObject(g_mdc, g_hSpirite[g_iNum]);
	BitBlt(g_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_mdc, 0, 0, SRCCOPY);

	g_tPre = GetTickCount();

	g_iNum++;
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