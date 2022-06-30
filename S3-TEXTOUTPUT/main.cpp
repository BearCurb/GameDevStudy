//====================================================================================
// 程序名称：S3-TEXTOUTPUT
// 时间: 2022年6月29日
// 描述: 学习GDI中字体的绘制
//====================================================================================

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
	srand((unsigned)time(NULL));	//初始化时间种子


	Game_Paint(hwnd);
	ReleaseDC(hwnd, g_hdc);
	return TRUE;
}


//------------------------------------------------------------------------------------
// Game_Paint 绘制函数
//------------------------------------------------------------------------------------
VOID Game_Paint(HWND hwnd)
{
	// HFONT CreateFontW(
	//	 [in] int     cHeight,			//字体的逻辑高度
	//	 [in] int     cWidth,			//字体的逻辑宽度
	//	 [in] int     cEscapement,		//字体显示的角度
	//	 [in] int     cOrientation,		//字体的角度
	//	 [in] int     cWeight,			//字体的磅数
	//	 [in] DWORD   bItalic,			//是否为斜体
	//	 [in] DWORD   bUnderline,		//是否带下划线
	//	 [in] DWORD   bStrikeOut,		//是否带删除线
	//	 [in] DWORD   iCharSet,			//所需的字符集
	//	 [in] DWORD   iOutPrecision,	//输出的精度
	//	 [in] DWORD   iClipPrecision,	//裁剪的精度
	//	 [in] DWORD   iQuality,			//输出的质量
	//	 [in] DWORD   iPitchAndFamily,	//字体间距的字符集
	//	 [in] LPCWSTR pszFaceName		//字体名称
	// );
	//
	HFONT hFont = CreateFont(30, 0, 0, 0, 0, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, L"微软雅黑");

	SelectObject(g_hdc, hFont);
	SetBkMode(g_hdc, TRANSPARENT);

	wchar_t text1[] = L"这是一段文字 需要被显示在界面上";
	wchar_t text2[] = L"这也是一段文字 需要被显示在界面上";
	wchar_t text3[] = L"这还是一段文字 需要被显示在界面上";

	//第一段文字
	SetTextColor(g_hdc, RGB(50, 255, 50));
	TextOut(g_hdc, 10, 20, text1, wcslen(text1));
	//第二段文字
	SetTextColor(g_hdc, RGB(50, 50, 255));
	TextOut(g_hdc, 10, 100, text2, wcslen(text1));
	//第三段文字
	SetTextColor(g_hdc, RGB(255, 150, 50));
	TextOut(g_hdc, 10, 180, text3, wcslen(text1));
}

//------------------------------------------------------------------------------------
// Game_CleanUp 资源释放函数
//------------------------------------------------------------------------------------
BOOL Game_CleanUp(HWND hwnd)
{
	DeleteObject(g_hdc);
	return TRUE;
}