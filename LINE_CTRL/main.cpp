#include <Windows.h>
#include <tchar.h>

HANDLE hMSP;
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow
){
	bool ctFlag=false;
	bool enFlag = false;
	hMSP = CreateMutex(NULL, TRUE,L"LINE-CTRL");
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		MessageBox(NULL, L"ëΩèdãNìÆ"
			, L"LINE-CTRL", MB_OK);
		ReleaseMutex(hMSP);
		CloseHandle(hMSP);
		return FALSE;
	}
	while (1) {
		TCHAR className[1024];
		HWND hwnd = GetForegroundWindow();
		GetClassName(hwnd, className, 1024);
		TCHAR p[] = TEXT("Qt5QWindowIcon");
		if (!_tcscmp(p, className)) {
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000&&!enFlag) {
				ctFlag = true;
			}
			if (GetAsyncKeyState(VK_RETURN) & 0x8000 && !ctFlag) {
				enFlag = true;
			}
			if (ctFlag&&GetAsyncKeyState(VK_RETURN) & 0x8000) {
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
				inp[0].ki.dwFlags = KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP;
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
		Sleep(1);
	}
	ReleaseMutex(hMSP);
	CloseHandle(hMSP);

	return 0;
}