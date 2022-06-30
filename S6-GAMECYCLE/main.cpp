//====================================================================================
// �������ƣ�S6-GAMECYCLE
// ʱ��: 2022��6��30��
// ����: ѧϰ��Ϸѭ��
//====================================================================================

#include <Windows.h>
#include <tchar.h>
#include <time.h>

//====================================================================================
// �궨��
//====================================================================================
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE L"Timer Animation"


//====================================================================================
// ȫ�ֱ�������
//====================================================================================
HDC g_hdc = NULL, g_mdc = NULL;		// �豸������� DC���
DWORD g_tPre = 0, g_tNow = 0;		// ��¼����ͼƬ��ʱ��
HBITMAP g_hSpirite[5];				// λͼ����,�����洢������λͼ
int g_iNum = 0;						// ��ǰ��ʾ��ͼƬ���


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
	wndClass.lpszClassName = _T("WD_NAME");							//�����������

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
			TranslateMessage(&msg);	// �����ⰴ����Ϣת��Ϊ�ַ���Ϣ
			DispatchMessage(&msg);	// �ַ�һ����Ϣ�����ڳ���
		}
		else
		{
			g_tNow = GetTickCount();	// ��ȡ��ǰϵͳʱ��
			if (g_tNow - g_tPre >= 100)
			{
				Game_Paint(hwnd);
			}
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

//------------------------------------------------------------------------------------
// Game_Init ��ʼ������
//------------------------------------------------------------------------------------
BOOL Game_Init(HWND hwnd)
{
	g_hdc = GetDC(hwnd);

	wchar_t filename[20];
	//���붯��λͼ
	for (int i = 0; i < 5; i++)
	{
		memset(filename, 0, sizeof(filename));
		swprintf_s(filename, L"Animation%d.bmp", (i + 1));

		g_hSpirite[i] = (HBITMAP)LoadImage(NULL, filename, IMAGE_BITMAP, WINDOW_WIDTH, WINDOW_HEIGHT, LR_LOADFROMFILE);
	}

	//��������DC
	g_mdc = CreateCompatibleDC(g_hdc);

	g_iNum = 0;						//��ʼ��ͼ����ʾΪ0
	return TRUE;
}


//------------------------------------------------------------------------------------
// Game_Paint ���ƺ���
//------------------------------------------------------------------------------------
VOID Game_Paint(HWND hwnd)
{
	//����ͼ��
	if (g_iNum == 5)
		g_iNum = 0;

	//����ͼ������ͼ
	SelectObject(g_mdc, g_hSpirite[g_iNum]);
	BitBlt(g_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_mdc, 0, 0, SRCCOPY);

	g_tPre = GetTickCount();

	g_iNum++;
}

//------------------------------------------------------------------------------------
// Game_CleanUp ��Դ�ͷź���
//------------------------------------------------------------------------------------
BOOL Game_CleanUp(HWND hwnd)
{
	DeleteObject(g_hdc);
	DeleteDC(g_mdc);
	return TRUE;
}