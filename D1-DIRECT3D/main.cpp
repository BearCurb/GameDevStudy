//====================================================================================
// �������ƣ�D1-DIRECT3D
// ʱ��: 2022��6��30��
// ����: ѧϰdirect3Dͨ�ÿ��
//====================================================================================

//=============================��ͷ�ļ��������֡�=====================================
// ����:���������ͷ�ļ�
//====================================================================================
#include <d3d9.h>

//=============================�����ļ��������֡�=====================================
// ����:�������������Ŀ��ļ�
//====================================================================================
#pragma comment(lib,"winmm.lib") 
#pragma comment(lib,"d3d9.lib") 
#pragma comment(lib,"d3dx9.lib")


//===============================���궨�岿�֡�=======================================
// ����:����һЩ������
//====================================================================================
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE L"Direct3D"
#define SAFE_RELEASE(p) {if(p){p->Release();(p) = NULL;}}

//=============================��ȫ�ֱ����������֡�====================================
// ����:��������Ҫ��ȫ�ֱ���
//====================================================================================
IDirect3DDevice9* g_pd3dDevice;

//============================��ȫ�ֺ����������֡�====================================
// ����:���������ͷ�ļ�
//====================================================================================
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
HRESULT Direct3D_Init(HWND hwnd);		//Direct3D��ʼ��
HRESULT	Object_Init(HWND hwnd);			//��Ҫ���Ƶ������ʼ��
VOID Direct3D_Render(HWND hwnd);		//Driect3D��Ⱦ����
VOID Direct3D_CleanUp();				//����COM��������Դ

//===============================��WinMain������======================================
// ����:���������ͷ�ļ�
//====================================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	//�������
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

	//ע�ᴰ��
	if (!RegisterClassEx(&wndClass))
		return -1;

	//��������
	HWND hwnd = CreateWindow(L"MY_WINDOW", WINDOW_TITLE, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
		CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

	//D3D��Դ�ĳ�ʼ��
	if (S_OK == Direct3D_Init(hwnd))
	{
		MessageBox(hwnd, L"OK", L"Message", 0);
	}
	else
	{
		MessageBox(hwnd, L"ERROE", L"Message", 0);
	}

	//�ƶ���ʾ�����
	MoveWindow(hwnd, 250, 80, WINDOW_WIDTH, WINDOW_HEIGHT, true);
	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	//�ȴ�����


	//��Ϣѭ��
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

	//�������ע��
	UnregisterClass(L"MY_WINDOW", wndClass.hInstance);

	return 0;
}

//===============================��WndProc������======================================
// ����:���ڹ��̺���WndPeoc���Դ�����Ϣ���д���
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

//============================��Direct3D_Init������===================================
// ����:Direct3D��ʼ������,����Direct3D�ĳ�ʼ��
//====================================================================================
HRESULT Direct3D_Init(HWND hwnd)
{
	//Direct3D�汾Э��
	LPDIRECT3D9 pD3D = NULL;
	if (NULL == (pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;

	//�����豸Ӳ����Ϣѡ�� Ӳ��������������������
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

	//����Direct3D�豸�ӿ�
	if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
		hwnd, vp, &d3dpp, &g_pd3dDevice)))
		return E_FAIL;

	SAFE_RELEASE(pD3D);

	//����һ�ζ�����Դ�ĳ�ʼ��
	if (!(S_OK == Object_Init(hwnd))) return E_FAIL;
	return S_OK;
}

//============================��Object_Init()������===================================
// ����:��Ⱦ��Դ��ʼ������������Ҫ��Ⱦ����Դ������г�ʼ��
//====================================================================================
HRESULT Object_Init(HWND hwnd)
{
	return S_OK;
}


//============================��Direct3d_Render()������=================================
// ����:�������Ⱦ����
//====================================================================================
void Direct3D_Render(HWND hwnd)
{
}


//============================��Direct3D_CleanUp()������===================================
// ����:��Դ���������ڴ˺����н��г����Ƴ�ǰ����Դ������
//====================================================================================
void Direct3D_CleanUp()
{

}


