//====================================================================================
// �������ƣ�S3-TEXTOUTPUT
// ʱ��: 2022��6��29��
// ����: ѧϰGDI������Ļ���
//====================================================================================

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

//------------------------------------------------------------------------------------
// Game_Init ��ʼ������
//------------------------------------------------------------------------------------
BOOL Game_Init(HWND hwnd)
{
	g_hdc = GetDC(hwnd);
	srand((unsigned)time(NULL));	//��ʼ��ʱ������


	Game_Paint(hwnd);
	ReleaseDC(hwnd, g_hdc);
	return TRUE;
}


//------------------------------------------------------------------------------------
// Game_Paint ���ƺ���
//------------------------------------------------------------------------------------
VOID Game_Paint(HWND hwnd)
{
	// HFONT CreateFontW(
	//	 [in] int     cHeight,			//������߼��߶�
	//	 [in] int     cWidth,			//������߼����
	//	 [in] int     cEscapement,		//������ʾ�ĽǶ�
	//	 [in] int     cOrientation,		//����ĽǶ�
	//	 [in] int     cWeight,			//����İ���
	//	 [in] DWORD   bItalic,			//�Ƿ�Ϊб��
	//	 [in] DWORD   bUnderline,		//�Ƿ���»���
	//	 [in] DWORD   bStrikeOut,		//�Ƿ��ɾ����
	//	 [in] DWORD   iCharSet,			//������ַ���
	//	 [in] DWORD   iOutPrecision,	//����ľ���
	//	 [in] DWORD   iClipPrecision,	//�ü��ľ���
	//	 [in] DWORD   iQuality,			//���������
	//	 [in] DWORD   iPitchAndFamily,	//��������ַ���
	//	 [in] LPCWSTR pszFaceName		//��������
	// );
	//
	HFONT hFont = CreateFont(30, 0, 0, 0, 0, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, L"΢���ź�");

	SelectObject(g_hdc, hFont);
	SetBkMode(g_hdc, TRANSPARENT);

	wchar_t text1[] = L"����һ������ ��Ҫ����ʾ�ڽ�����";
	wchar_t text2[] = L"��Ҳ��һ������ ��Ҫ����ʾ�ڽ�����";
	wchar_t text3[] = L"�⻹��һ������ ��Ҫ����ʾ�ڽ�����";

	//��һ������
	SetTextColor(g_hdc, RGB(50, 255, 50));
	TextOut(g_hdc, 10, 20, text1, wcslen(text1));
	//�ڶ�������
	SetTextColor(g_hdc, RGB(50, 50, 255));
	TextOut(g_hdc, 10, 100, text2, wcslen(text1));
	//����������
	SetTextColor(g_hdc, RGB(255, 150, 50));
	TextOut(g_hdc, 10, 180, text3, wcslen(text1));
}

//------------------------------------------------------------------------------------
// Game_CleanUp ��Դ�ͷź���
//------------------------------------------------------------------------------------
BOOL Game_CleanUp(HWND hwnd)
{
	DeleteObject(g_hdc);
	return TRUE;
}