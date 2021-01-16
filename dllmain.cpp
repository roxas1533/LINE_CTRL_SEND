// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include "pch.h"
#pragma comment(linker, "/SECTION:shared,rws")
#pragma data_seg("shared")
HHOOK hMyHook;
HWND parentWnd = 0;
#pragma data_seg()

extern "C" __declspec(dllexport) void setWind(HWND hwnd);
LONG_PTR DefStaticProc;
INPUT in;

std::string to_binString(long val)
{
	if (!val)
		return std::string("0");
	std::string str;
	for (int i = 0; i < 8 * sizeof(val); i++) {
		if ((val & 1) == 0)  // val は偶数か？
			str.insert(str.begin(), '0');  //  偶数の場合
		else
			str.insert(str.begin(), '1');  //  奇数の場合
		val >>= 1;
	}
	return str;
}
LRESULT CALLBACK StaticProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_KEYDOWN:
		switch (wp)
		{
		case VK_RETURN:
			std::cout << "VK_RETURN>>";
			break;
		case VK_SHIFT:
			std::cout << "VK_SHIFT >>";
			break;
		default:
			std::cout << wp << ">>";
			break;
		}
		std::cout << to_binString(lp) << "\n";
		if (wp == VK_RETURN && ((lp >> 29) & 1) == 0) {
			long temp = pow(2, 15);
			temp |= (1 << 29);
			if (SendInput(1, &in, sizeof(INPUT)))
				//SendMessage(hwnd, WM_KEYDOWN, VK_RETURN, (LPARAM)(1 << 29));
				return 0;
		}
		return CallWindowProc((WNDPROC)DefStaticProc, hwnd, msg, wp, lp);
	case WM_KILLFOCUS:
		if (wp && (HWND)wp != parentWnd) {
			SetWindowLongPtr((HWND)wp, GWLP_WNDPROC, (__int3264)(LONG_PTR)StaticProc);
		}
		return CallWindowProc((WNDPROC)DefStaticProc, hwnd, msg, wp, lp);
	default:
		break;
	}
	return CallWindowProc((WNDPROC)DefStaticProc, hwnd, msg, wp, lp);
}
LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam) {
	std::cout << 1111 << "\n";
	return CallNextHookEx(hMyHook, nCode, wParam, lParam);
}
void HookMessage() {
	DefStaticProc = GetWindowLongPtr(parentWnd, GWLP_WNDPROC);
	SetWindowLongPtr(parentWnd, GWLP_WNDPROC, (__int3264)(LONG_PTR)StaticProc);
}
void setWind(HWND hwnd) {
	parentWnd = hwnd;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		AllocConsole();
		FILE* fp;
		freopen_s(&fp, "CONOUT$", "w", stdout);
		PrintFunctions();
		HookMessage();
		in.type = INPUT_KEYBOARD;
		in.ki.wVk = VK_SHIFT;
		in.ki.wScan = MapVirtualKey(VK_SHIFT, 0);
		in.ki.dwExtraInfo = 20;
		in.ki.dwFlags = KEYEVENTF_EXTENDEDKEY;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}