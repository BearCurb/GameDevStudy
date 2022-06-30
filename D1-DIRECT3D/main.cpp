//====================================================================================
// 程序名称：D1-DIRECT3D
// 时间: 2022年6月30日
// 描述: 学习direct3D通用框架
//====================================================================================

//=============================【头文件包含部分】=====================================
// 描述:包含程序的头文件
//====================================================================================
#include <d3d9.h>

//=============================【库文件包含部分】=====================================
// 描述:包含程序依赖的库文件
//====================================================================================
#pragma comment(lib,"winmm.lib") 
#pragma comment(lib,"d3d9.lib") 
#pragma comment(lib,"d3dx9.lib")


//===============================【宏定义部分】=======================================
// 描述:定义一些辅助宏
//====================================================================================
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE L"Direct3D"
#define SAFE_RELEASE(p) {if(p){p->Release();(p) = NULL;}}

//=============================【全局变量声明部分】====================================
// 描述:声名所需要的全局变量
//====================================================================================
IDirect3DDevice9* g_pd3dDevice;

//============================【全局函数声名部分】====================================
// 描述:包含程序的头文件
//====================================================================================
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
HRESULT Direct3D_Init(HWND hwnd);		//Direct3D初始化
HRESULT	Object_Init(HWND hwnd);			//需要绘制的物体初始化
VOID Direct3D_Render(HWND hwnd);		//Driect3D渲染代码
VOID Direct3D_CleanUp();				//清理COM和其他资源

//===============================【WinMain函数】======================================
// 描述:包含程序的头文件
//====================================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	//窗口设计
	WNDCLASSEX wndClass = { 0 };
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = (HICON)::LoadImage(NULL, L"icon.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = L"MY_WINDOW";

	//注册窗口
	if (!RegisterClassEx(&wndClass))
		return -1;

	//创建窗口
	HWND hwnd = CreateWindow(L"MY_WINDOW", WINDOW_TITLE, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
		CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

	//D3D资源的初始化
	if (S_OK == Direct3D_Init(hwnd))
	{
		MessageBox(hwnd, L"OK", L"Message", 0);
	}
	else
	{
		MessageBox(hwnd, L"ERROE", L"Message", 0);
	}

	//移动显示与跟新
	MoveWindow(hwnd, 250, 80, WINDOW_WIDTH, WINDOW_HEIGHT, true);
	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	//等待音乐


	//消息循环
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Direct3D_Render(hwnd);
		}
	}

	//窗口类的注销
	UnregisterClass(L"MY_WINDOW", wndClass.hInstance);

	return 0;
}

//===============================【WndProc函数】======================================
// 描述:窗口过程函数WndPeoc，对窗口消息进行处理
//====================================================================================
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		Direct3D_Render(hwnd);
		ValidateRect(hwnd, NULL);
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		Direct3D_CleanUp();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
		break;
	}

	return 0;
}

//============================【Direct3D_Init函数】===================================
// 描述:Direct3D初始化函数,进行Direct3D的初始化
//====================================================================================
HRESULT Direct3D_Init(HWND hwnd)
{
	//Direct3D版本协商
	LPDIRECT3D9 pD3D = NULL;
	if (NULL == (pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;

	//根据设备硬件信息选择 硬件顶点计算或软件顶点计算
	D3DCAPS9 caps;
	int vp = 0;
	if (FAILED(pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps)))
	{
		return E_FAIL;
	}
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth = WINDOW_WIDTH;
	d3dpp.BackBufferHeight = WINDOW_HEIGHT;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hwnd;
	d3dpp.Windowed = true;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.Flags = 0;
	d3dpp.FullScreen_RefreshRateInHz = 0;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	//创建Direct3D设备接口
	if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
		hwnd, vp, &d3dpp, &g_pd3dDevice)))
		return E_FAIL;

	SAFE_RELEASE(pD3D);

	//调用一次对象资源的初始化
	if (!(S_OK == Object_Init(hwnd))) return E_FAIL;
	return S_OK;
}

//============================【Object_Init()函数】===================================
// 描述:渲染资源初始化函数，对需要渲染的资源物体进行初始化
//====================================================================================
HRESULT Object_Init(HWND hwnd)
{
	return S_OK;
}


//============================【Direct3d_Render()函数】=================================
// 描述:具体的渲染方法
//====================================================================================
void Direct3D_Render(HWND hwnd)
{
}


//============================【Direct3D_CleanUp()函数】===================================
// 描述:资源清理函数，在此函数中进行程序推出前的资源清理工作
//====================================================================================
void Direct3D_CleanUp()
{

}


