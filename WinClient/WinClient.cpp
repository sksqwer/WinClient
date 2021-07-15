// WinClient.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "framework.h"
#include "WinClient.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

//socket client
WSADATA	wsadata;
SOCKET	s;
SOCKADDR_IN addr = { 0 };
TCHAR msg[200], str[200];
char buffer[200];
int msgLen;

//chatting 
TCHAR _ID[20] = _T("");
char ID[20] = ("");
std::list<TCHAR*> chat_history;

//function
int Win_Client_close();
int Win_Client_init(HWND hWnd);
void SendMessageToServer();

//chatting
void deletelist();


// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINCLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINCLIENT));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINCLIENT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINCLIENT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//static
	static int msgcount = 0;
	static bool id_flag = true;
	static int pos_y = 0;

    switch (message)
    {
	case WM_CREATE:
	{

		return Win_Client_init(hWnd);
	}
	break;
	case WM_ASYNC:
	{
		switch ((lParam))
		{
		case FD_READ:
			msgLen = recv(s, buffer, 100, 0);
			buffer[msgLen + 1] = NULL;
#ifdef _UNICODE
			msgLen = MultiByteToWideChar(CP_ACP, 0, buffer, strlen(buffer), NULL, NULL);
			MultiByteToWideChar(CP_ACP, 0, buffer, strlen(buffer), msg, msgLen);
			msg[msgLen] = NULL;
			TCHAR *newmsg = new TCHAR[200];
			_stprintf_s(newmsg, 200, _T("%s"), msg);

			chat_history.push_back(newmsg);
#else
			strcpy(msg, buffer);
#endif _UNICODE
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		}
	}
	break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_CHAR:
	{
		if (id_flag)
		{
			if (wParam == VK_RETURN)
			{
				id_flag = false;
				msgcount = 0;
				msgLen = WideCharToMultiByte(CP_ACP, 0, _ID, -1, NULL, 0, NULL, NULL);
				WideCharToMultiByte(CP_ACP, 0, _ID, -1, ID, msgLen, NULL, NULL);
				return 0;
			}
			else if (wParam == VK_BACK)
			{
				if(msgcount != 0)
					msgcount--;
				_ID[msgcount] = NULL;
			}
			else
			{
				if (msgcount < 19)
				{
					_ID[msgcount++] = wParam;
					_ID[msgcount] = NULL;
					InvalidateRect(hWnd, NULL, true);
				}

			}
		}
		else
		{
			if (wParam == VK_RETURN)
			{
				if (s == INVALID_SOCKET) return 0;

#ifdef _UNICODE		
				TCHAR temp[200];
				_stprintf_s(temp, 200, _T("%s"), str);
				_stprintf_s(str, 200, _T("%s : %s"), _ID, temp);

				msgLen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
				WideCharToMultiByte(CP_ACP, 0, str, -1, buffer, msgLen, NULL, NULL);
				buffer[msgLen] = NULL;
#else
				strcpy(msg, buffer);
#endif _UNICODE
				SendMessageToServer();
				msgcount = 0;
				str[msgcount] = NULL;

				return 0;

			}
			else if (wParam == VK_BACK)
			{
				if (msgcount != 0)
					msgcount--;
				str[msgcount] = NULL;
			}
			else
			{
				if (msgcount < 199)
				{
					str[msgcount++] = wParam;
					str[msgcount] = NULL;
				}
			}
		}

		InvalidateRect(hWnd, NULL, true);

	}
	break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...

			if (id_flag)
			{
				TextOut(hdc, 0, 0, _T("ID를 입력하세요 : "), _tcslen(_T("ID를 입력하세요 : ")));
				TextOut(hdc, _tcslen(_T("ID를 입력하세요 : "))* 11 + 1, 0, _ID, _tcslen(_ID));
				return 0;
			}
			pos_y = 0;
			for (auto itr = chat_history.begin(); itr != chat_history.end(); itr++)
			{
				TextOut(hdc, 0, pos_y, *itr, (int)_tcslen(*itr));
				pos_y += 20;
			}

			TCHAR temp[200];
			_stprintf_s(temp, 200, _T("%s : %s"), _ID, str);
			TextOut(hdc, 0, pos_y, temp, (int)_tcslen(temp));



            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		Win_Client_close();
		deletelist();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

int Win_Client_close()
{
	closesocket(s);
	WSACleanup();
	return 1;
}

int Win_Client_init(HWND hWnd)
{

	WSAStartup(MAKEWORD(2, 2), &wsadata);
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET)
	{
		MessageBox(NULL, _T("socket failed"), _T("Error"), MB_OK);
		return 0;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = 8080;
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");


	if (connect(s, (LPSOCKADDR)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		MessageBox(NULL, _T("connecting failed"), _T("Error"), MB_OK);
		return 0;
	}

	WSAAsyncSelect(s, hWnd, WM_ASYNC, FD_READ);

	return 0;
}

void SendMessageToServer()
{
	send(s, buffer, strlen(buffer) + 1, 0);
}

void deletelist()
{
	for (auto itr = chat_history.begin(); itr != chat_history.end(); itr++)
		delete *itr;
}
