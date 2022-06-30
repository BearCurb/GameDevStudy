//====================================================================================
// �������ƣ�S7-SNOW
// ʱ��: 2022��6��30��
// ����: ѧϰ����Ч��
//====================================================================================

//====================================================================================
// ͷ�ļ�����
//====================================================================================
#include <Windows.h>
#include <tchar.h>
#include <time.h>

#pragma comment(lib, "Msimg32.lib")

//====================================================================================
// �궨��
//====================================================================================
#define WINDOW_WIDTH 800			// ���ڿ��
#define WINDOW_HEIGHT 600			// ���ڸ߶�
#define WINDOW_TITLE L"S7-SNOW"		// ���ڱ���
#define PARTICLE_NUMBER 200			// ��������

//====================================================================================
// ȫ�ֽṹ�嶨��
//====================================================================================
struct SNOW
{
	int x;		// ѩ����X����
	int y;		// ѩ����Y����
	BOOL exist;	// ѩ���Ƿ����
};

//====================================================================================
// ȫ�ֱ�������
//====================================================================================
HDC g_hdc = NULL, g_mdc = NULL, g_bufdc = NULL;		// ȫ�ֻ������
HBITMAP g_hBackgrand, g_hSnow;						// ͼƬ���
DWORD g_tPre = 0, g_tNow = 0;						// ��ͼʱ���¼
RECT g_rect;										// RECR�ṹ�壬���ڴ洢�ڲ��������������
SNOW SnowFlowers[PARTICLE_NUMBER];					// ѩ����������
int g_SnowNum = 0;								// ���ڼ����ı���

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
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);	//������ı�������ˢ���(���巢���ػ��ʱ��ʹ������ָ���Ļ�����ִ���ػ�)
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
	srand((unsigned)time(NULL));

	HBITMAP bmp;

	g_hdc = GetDC(hwnd);					// �豸DC
	g_mdc = CreateCompatibleDC(g_hdc);		// ����DC
	g_bufdc = CreateCompatibleDC(g_hdc);	// ����DC
	bmp = CreateCompatibleBitmap(g_hdc, WINDOW_WIDTH, WINDOW_HEIGHT);

	SelectObject(g_mdc, bmp);				//����λͼ����g_mdc��

	//����λͼ��Դ
	g_hBackgrand = (HBITMAP)LoadImage(NULL, L"backgrand.bmp", IMAGE_BITMAP, 800, 600, LR_LOADFROMFILE);
	g_hSnow = (HBITMAP)LoadImage(NULL, L"snow.bmp", IMAGE_BITMAP, 8, 8, LR_LOADFROMFILE);

	GetClientRect(hwnd, &g_rect);	//ȡ���ڲ�����Ĵ�С

	Game_Paint(hwnd);
	return TRUE;
}


//------------------------------------------------------------------------------------
// Game_Paint ���ƺ���
//------------------------------------------------------------------------------------
VOID Game_Paint(HWND hwnd)
{
	//���ϱ���ͼ(�����Ǻ�ɫ�ı��� ���Բ���Ҫͼ)
	SelectObject(g_bufdc, g_hBackgrand);
	BitBlt(g_mdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_bufdc, 0, 0, SRCCOPY);

	//��������
	if (g_SnowNum < PARTICLE_NUMBER)
	{
		SnowFlowers[g_SnowNum].x = rand() % g_rect.right;	// ����Ϊˮƽ�����ϵ�����λ��
		SnowFlowers[g_SnowNum].y = 0;						// ��Y���궼����Ϊ0
		SnowFlowers[g_SnowNum].exist = true;
		g_SnowNum++;
	}

	for (int i = 0; i < PARTICLE_NUMBER; i++)
	{
		if (SnowFlowers[i].exist)
		{
			SelectObject(g_bufdc, g_hSnow);

			//������͸����ͼ������DC��
			TransparentBlt(g_mdc, SnowFlowers[i].x, SnowFlowers[i].y, 8, 8, g_bufdc, 0, 0, 8, 8, RGB(0, 0, 0));

			//�����ӽ�������ƫ�Ʋ���
			if (rand() % 2 == 0)
				SnowFlowers[i].x += rand() % 3;
			else
				SnowFlowers[i].x -= rand() % 3;

			//�������������˶�
			SnowFlowers[i].y += 20;
			if (SnowFlowers[i].y > g_rect.bottom)
			{
				SnowFlowers[i].x = rand() % g_rect.right;
				SnowFlowers[i].y = 0;
			}
		}
	}

	//������DC�����豸DC��
	BitBlt(g_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_mdc, 0, 0, SRCCOPY);

	g_tPre = GetTickCount();
}

//------------------------------------------------------------------------------------
// Game_CleanUp ��Դ�ͷź���
//------------------------------------------------------------------------------------
BOOL Game_CleanUp(HWND hwnd)
{
	DeleteObject(g_hdc);
	DeleteDC(g_mdc);
	DeleteDC(g_bufdc);
	return TRUE;
}