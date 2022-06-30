//====================================================================================
// 程序名称：S7-SNOW
// 时间: 2022年6月30日
// 描述: 学习粒子效果
//====================================================================================

//====================================================================================
// 头文件部分
//====================================================================================
#include <Windows.h>
#include <tchar.h>
#include <time.h>

#pragma comment(lib, "Msimg32.lib")

//====================================================================================
// 宏定义
//====================================================================================
#define WINDOW_WIDTH 800			// 窗口宽度
#define WINDOW_HEIGHT 600			// 窗口高度
#define WINDOW_TITLE L"S7-SNOW"		// 窗口标题
#define PARTICLE_NUMBER 200			// 粒子数量

//====================================================================================
// 全局结构体定义
//====================================================================================
struct SNOW
{
	int x;		// 雪花的X坐标
	int y;		// 雪花的Y坐标
	BOOL exist;	// 雪花是否存在
};

//====================================================================================
// 全局变量声名
//====================================================================================
HDC g_hdc = NULL, g_mdc = NULL, g_bufdc = NULL;		// 全局环境句柄
HBITMAP g_hBackgrand, g_hSnow;						// 图片句柄
DWORD g_tPre = 0, g_tNow = 0;						// 绘图时间记录
RECT g_rect;										// RECR结构体，用于存储内部窗口区域的坐标
SNOW SnowFlowers[PARTICLE_NUMBER];					// 雪花粒子数组
int g_SnowNum = 0;								// 用于计数的变量

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
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);	//窗口类的背景画笔刷句柄(窗体发生重绘的时候使用这里指定的画笔来执行重绘)
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
	srand((unsigned)time(NULL));

	HBITMAP bmp;

	g_hdc = GetDC(hwnd);					// 设备DC
	g_mdc = CreateCompatibleDC(g_hdc);		// 兼容DC
	g_bufdc = CreateCompatibleDC(g_hdc);	// 缓存DC
	bmp = CreateCompatibleBitmap(g_hdc, WINDOW_WIDTH, WINDOW_HEIGHT);

	SelectObject(g_mdc, bmp);				//将空位图放在g_mdc中

	//载入位图资源
	g_hBackgrand = (HBITMAP)LoadImage(NULL, L"backgrand.bmp", IMAGE_BITMAP, 800, 600, LR_LOADFROMFILE);
	g_hSnow = (HBITMAP)LoadImage(NULL, L"snow.bmp", IMAGE_BITMAP, 8, 8, LR_LOADFROMFILE);

	GetClientRect(hwnd, &g_rect);	//取得内部区域的大小

	Game_Paint(hwnd);
	return TRUE;
}


//------------------------------------------------------------------------------------
// Game_Paint 绘制函数
//------------------------------------------------------------------------------------
VOID Game_Paint(HWND hwnd)
{
	//贴上背景图(这里是黑色的背景 所以不需要图)
	SelectObject(g_bufdc, g_hBackgrand);
	BitBlt(g_mdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_bufdc, 0, 0, SRCCOPY);

	//创建粒子
	if (g_SnowNum < PARTICLE_NUMBER)
	{
		SnowFlowers[g_SnowNum].x = rand() % g_rect.right;	// 设置为水平方向上的任意位置
		SnowFlowers[g_SnowNum].y = 0;						// 将Y坐标都设置为0
		SnowFlowers[g_SnowNum].exist = true;
		g_SnowNum++;
	}

	for (int i = 0; i < PARTICLE_NUMBER; i++)
	{
		if (SnowFlowers[i].exist)
		{
			SelectObject(g_bufdc, g_hSnow);

			//将粒子透明贴图到兼容DC中
			TransparentBlt(g_mdc, SnowFlowers[i].x, SnowFlowers[i].y, 8, 8, g_bufdc, 0, 0, 8, 8, RGB(0, 0, 0));

			//让粒子进行左右偏移操作
			if (rand() % 2 == 0)
				SnowFlowers[i].x += rand() % 3;
			else
				SnowFlowers[i].x -= rand() % 3;

			//让粒子做匀速运动
			SnowFlowers[i].y += 20;
			if (SnowFlowers[i].y > g_rect.bottom)
			{
				SnowFlowers[i].x = rand() % g_rect.right;
				SnowFlowers[i].y = 0;
			}
		}
	}

	//将兼容DC贴到设备DC中
	BitBlt(g_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_mdc, 0, 0, SRCCOPY);

	g_tPre = GetTickCount();
}

//------------------------------------------------------------------------------------
// Game_CleanUp 资源释放函数
//------------------------------------------------------------------------------------
BOOL Game_CleanUp(HWND hwnd)
{
	DeleteObject(g_hdc);
	DeleteDC(g_mdc);
	DeleteDC(g_bufdc);
	return TRUE;
}