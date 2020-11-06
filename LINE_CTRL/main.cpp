#include <Windows.h>
#include <tchar.h>

bool ctFlag = false;
bool enFlag = false;
bool isLine = false;
const TCHAR p[] = TEXT("Qt5QWindowIcon");

TCHAR className[1024];
VOID CALLBACK LineTimer(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);
VOID CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime) {
	HWND hwnd = GetForegroundWindow();
	GetClassName(hwnd, className, 1024);

	if (!_tcscmp(p, className)) {
		isLine = true;
		KillTimer(hWnd, 1);
		SetTimer(hWnd, 2, 10, (TIMERPROC)LineTimer);
	}
}

VOID CALLBACK LineTimer(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime) {
	HWND hwnd = GetForegroundWindow();
	GetClassName(hwnd, className, 1024);

	if (_tcscmp(p, className)) {
		isLine = false;
		KillTimer(hWnd, 2);
		SetTimer(hWnd, 1, 100, (TIMERPROC)TimerProc);
	}
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000 && !enFlag) {
		ctFlag = true;
	}
	if (GetAsyncKeyState(VK_RETURN) & 0x8000 && !ctFlag) {
		enFlag = true;
	}
	if (ctFlag && GetAsyncKeyState(VK_RETURN) & 0x8000) {
		INPUT inp[1];
		inp[0].type = INPUT_KEYBOARD;
		inp[0].ki.wVk = VK_MENU;
		inp[0].ki.wScan = (short)MapVirtualKey(VK_MENU, 0);
		inp[0].ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
		inp[0].ki.dwExtraInfo = 0;
		inp[0].ki.time = 0;
		SendInput(1, inp, sizeof(INPUT));
		inp[0].type = INPUT_KEYBOARD;
		inp[0].ki.wVk = VK_RETURN;
		inp[0].ki.wScan = (short)MapVirtualKey(VK_RETURN, 0);
		inp[0].ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
		inp[0].ki.dwExtraInfo = 0;
		inp[0].ki.time = 0;
		SendInput(1, inp, sizeof(INPUT));
		inp[0].type = INPUT_KEYBOARD;
		inp[0].ki.wVk = VK_MENU;
		inp[0].ki.wScan = (short)MapVirtualKey(VK_MENU, 0);
		inp[0].ki.dwFlags = KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;
		inp[0].ki.dwExtraInfo = 0;
		inp[0].ki.time = 0;
		SendInput(1, inp, sizeof(INPUT));
		inp[0].type = INPUT_KEYBOARD;
		inp[0].ki.wVk = VK_RETURN;
		inp[0].ki.wScan = (short)MapVirtualKey(VK_RETURN, 0);
		inp[0].ki.dwFlags = KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;
		inp[0].ki.dwExtraInfo = 0;
		inp[0].ki.time = 0;
		SendInput(1, inp, sizeof(INPUT));
	}
	if (!(GetAsyncKeyState(VK_RETURN) & 0x8000)) {
		enFlag = false;
	}
	if (!(GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
		ctFlag = false;
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_CREATE:
		SetTimer(hwnd, 1, 100, (TIMERPROC)TimerProc);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, int nCmdShow) {
	HWND hwnd;
	MSG msg;
	WNDCLASS winc;

	winc.style = CS_HREDRAW | CS_VREDRAW;
	winc.lpfnWndProc = WndProc;
	winc.cbClsExtra = winc.cbWndExtra = 0;
	winc.hInstance = hInstance;
	winc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winc.hCursor = LoadCursor(NULL, IDC_ARROW);
	winc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	winc.lpszMenuName = NULL;
	winc.lpszClassName = TEXT("LINE_CTRL");

	if (!RegisterClass(&winc)) return 0;
	HANDLE hMSP;

	hMSP = CreateMutex(NULL, TRUE, L"LINE-CTRL");
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		MessageBox(NULL, L"ëΩèdãNìÆ"
			, L"LINE-CTRL", MB_OK);
		if (hMSP) {
			ReleaseMutex(hMSP);
			CloseHandle(hMSP);
		}
		return FALSE;
	}
	hwnd = CreateWindow(
		TEXT("LINE_CTRL"), TEXT("LINE_CTRL"),
		WS_OVERLAPPEDWINDOW,
		0, 0, 0, 0, NULL, NULL,
		hInstance, NULL
	);

	if (hwnd == NULL) return 1;

	while (GetMessage(&msg, NULL, 0, 0)) DispatchMessage(&msg);
	return msg.wParam;
}