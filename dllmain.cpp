// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include "pch.h"
#pragma comment(linker, "/SECTION:shared,rws")
#pragma data_seg("shared")
HHOOK hMyHook;
HWND parentWnd = 0;
int loadCount = 0;
#pragma data_seg()
#pragma comment(lib,"Comctl32.lib")

extern "C" __declspec(dllexport) void setWind(HWND hwnd);
LONG_PTR DefStaticProc;
std::vector<INPUT> returnInput;
std::vector<INPUT> sendInput;
const long maskScan = ((long)pow(2, 8) - 1) << 16;
LRESULT CALLBACK StaticProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_KEYDOWN:
		if (wp == VK_RETURN && ((lp & maskScan) >> 16)) {
			if (GetKeyState(VK_CONTROL) & 0x8000) {
				SendInput(sendInput.size(), sendInput.data(), sizeof(INPUT));
				return 0;
			}
			if (SendInput(returnInput.size(), returnInput.data(), sizeof(INPUT)))
				return 0;
		}
		return CallWindowProc((WNDPROC)DefStaticProc, hwnd, msg, wp, lp);
	default:
		break;
	}
	return CallWindowProc((WNDPROC)DefStaticProc, hwnd, msg, wp, lp);
}
void HookMessage() {
	DefStaticProc = GetWindowLongPtr(parentWnd, GWLP_WNDPROC);
	SetWindowLongPtr(parentWnd, GWLP_WNDPROC, (__int3264)(LONG_PTR)StaticProc);
}
void setWind(HWND hwnd) {
	parentWnd = hwnd;
}
extern "C" __declspec(dllexport) int getLoadCount() {
	return loadCount;
}
extern "C" HWND WINAPI HookCreateWindowExW(
	_In_ DWORD dwExStyle,
	_In_opt_ LPCWSTR lpClassName,
	_In_opt_ LPCWSTR lpWindowName,
	_In_ DWORD dwStyle,
	_In_ int X,
	_In_ int Y,
	_In_ int nWidth,
	_In_ int nHeight,
	_In_opt_ HWND hWndParent,
	_In_opt_ HMENU hMenu,
	_In_opt_ HINSTANCE hInstance,
	_In_opt_ LPVOID lpParam) {
	HWND nextWnd = CreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
	SetWindowLongPtr(nextWnd, GWLP_WNDPROC, (__int3264)(LONG_PTR)StaticProc);
	return nextWnd;
}


void pushKey(std::vector<INPUT>& in, WORD key, int iskeyUp) {
	INPUT temp;
	temp.type = INPUT_KEYBOARD;
	temp.ki.wVk = key;
	temp.ki.wScan = MapVirtualKey(0, 0);
	temp.ki.dwFlags = iskeyUp;
	in.push_back(temp);
}

LRESULT CALLBACK KeyProc(int nCode, WPARAM wParam, LPARAM lParam) {
	std::cout << wParam << "\n";
	return CallNextHookEx(0, nCode, wParam, lParam);
}


BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: {
		//AllocConsole();
		//FILE* fp;
		//freopen_s(&fp, "CONOUT$", "w", stdout);
		HookMessage();
		pushKey(returnInput, VK_SHIFT,0);
		pushKey(returnInput, VK_RETURN,0);
		pushKey(returnInput, VK_RETURN, KEYEVENTF_KEYUP);
		pushKey(returnInput, VK_SHIFT, KEYEVENTF_KEYUP);
		pushKey(sendInput, VK_CONTROL, KEYEVENTF_KEYUP);
		pushKey(sendInput, VK_RETURN,0);
		pushKey(sendInput, VK_RETURN, KEYEVENTF_KEYUP);
		loadCount++;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}