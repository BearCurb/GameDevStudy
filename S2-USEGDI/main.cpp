#include <Windows.h>
#include <tchar.h>
#include <time.h>

//====================================================================================
// �궨��
//====================================================================================
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE L"����"


//====================================================================================
// ȫ�ֱ�������
//====================================================================================
HDC g_hdc;	// �豸�������
HPEN g_hPen[7] = { 0 };
HBRUSH g_hBrush[7] = { 0 };
int g_iPenStyle[7] = { PS_SOLID, PS_DASH,PS_DOT,PS_DASHDOT,PS_DASHDOTDOT,
PS_NULL,PS_INSIDEFRAME };
int g_iBrushStyle[7] = { HS_VERTICAL,HS_HORIZONTAL,HS_CROSS,HS_DIAGCROSS,
HS_FDIAGONAL,HS_BDIAGONAL };


//====================================================================================
// ȫ�ֺ�������
//====================================================================================
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL Game_Init(HWND hwnd);
VOID Game_Paint(HWND hwnd);
BOOL Game_CleanUp(HWND hwnd);

//------------------------------------------------------------------------------------
// WinMain������
//------------------------------------------------------------------------------------
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	WNDCLASSEX wndClass = { 0 };
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;			//���ڵ���ʽ
	wndClass.lpfnWndProc = WndProc;						//���ڵ���Ϣ������
	wndClass.cbClsExtra = 0;							//������ĸ����ڴ�
	wndClass.cbWndExtra = 0;							//���ڵĸ����ڴ�
	wndClass.hInstance = hInstance;
	wndClass.hIcon = (HICON)::LoadImage(NULL, _T("icon.ico"), IMAGE_ICON,
		0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);					//ͼ����Դ
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);					//��ͷ���
	wndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);	//������ı�������ˢ���(���巢���ػ��ʱ��ʹ������ָ���Ļ�����ִ���ػ�)
	wndClass.lpszMenuName = NULL;									//�˵���Դ������
	wndClass.lpszClassName = _T("WD_NAME");						//�����������

	// ע�ᴰ��
	if (!RegisterClassEx(&wndClass))
		return -1;

	// ��������
	HWND hwnd = CreateWindow(_T("WD_NAME"), WINDOW_TITLE,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH,
		WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

	MoveWindow(hwnd, 200, 50, WINDOW_WIDTH, WINDOW_HEIGHT, true);			// ���ô��ڳ�ʼλ��
	ShowWindow(hwnd, nCmdShow);												// ��ʾ����
	UpdateWindow(hwnd);														// ���´���

	//��Ϸ��Դ�ĳ�ʼ��
	if (!Game_Init(hwnd))
	{
		MessageBox(hwnd, L"��Ϸ��Դ��ʼʧ��", L"��Ϣ����", 0);
		return FALSE;
	}

	//ѭ����Ϣ����
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);	//�����ⰴ����Ϣת��Ϊ�ַ���Ϣ
			DispatchMessage(&msg);	//�ַ�һ����Ϣ�����ڳ���
		}
	}
	// ע��
	UnregisterClass(L"WD_NAME", wndClass.hInstance);

	return 0;
}

//------------------------------------------------------------------------------------
// ���ڹ��̺���
//------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT paintStruct;

	switch (message)
	{
	case WM_PAINT:		//�ͻ����ػ���Ϣ
		g_hdc = BeginPaint(hwnd, &paintStruct);
		Game_Paint(hwnd);
		EndPaint(hwnd, &paintStruct);
		ValidateRect(hwnd, NULL);
		break;
	case WM_KEYDOWN:	//���̱������¼�
		if (wParam == VK_ESCAPE)
			DestroyWindow(hwnd);
		break;
	case WM_DESTROY:	//����������Ϣ
		//��Ϸ��Դ������
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
// ����ʵ�ֲ���
//====================================================================================


BOOL Game_Init(HWND hwnd)
{
	g_hdc = GetDC(hwnd);
	srand((unsigned)time(NULL));	//��ʼ��ʱ������

	//�����ʼ�ʺͱ�ˢ����ʽ
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
		SelectObject(g_hdc, g_hPen[i]);		//ѡ���
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