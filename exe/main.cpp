#include <iostream>
#include <Windows.h>
#include <sstream>
#include "getProcess.h"
#include <string>
#include<fstream>
#pragma comment(lib, "winmm.lib")
bool isDebug = true;
void debugError(const char* out) {
	if (isDebug)
		std::cout << out << "\n";
}
void debugError(std::string out) {
	debugError(out.c_str());
}
typedef void(*GetHWND)(HWND hwnd, HINSTANCE proid);
int main(int argc, char* argv[]) {
	if(argc!=1)
		isDebug = true;
	STARTUPINFOA  tStartupInfo = { 0 };
	PROCESS_INFORMATION tProcessInfomation = { 0 };
	char c[] = "C:\\Users\\%username%\\AppData\\Local\\LINE\\bin\\current\\LINE.exe";
	std::string exePath;
	exePath.resize(ExpandEnvironmentStringsA(c, nullptr, 0));
	if (ExpandEnvironmentStringsA(c, &exePath.front(), exePath.size()) == NULL) {
		MessageBoxA(NULL, "環境変数をパスに変換できませんでした。", "LINEERROR", MB_OK);
		return 0;
	}
	debugError(std::string("環境変数パス:") + std::string(exePath));
	std::ifstream ifs((std::string(exePath)));
	if (!ifs) {
		debugError("exeファイル存在確認失敗");
		MessageBoxA(NULL, "LINE.exeが存在しません", "LINEERROR", MB_OK);
		return 0;
	}
	else
		debugError("exeファイル存在確認OK");

	if (getProcess()) {
		char* charPath = new char[exePath.size() + 1]; // メモリ確保
		std::char_traits<char>::copy(charPath, exePath.c_str(), exePath.size() + 1);
		CreateProcessA(0, charPath, NULL, NULL, FALSE, 0, NULL, NULL, &tStartupInfo, &tProcessInfomation);
		return 0;
	}
#if _DEBUG  
	char dllPath[] = "C:\\Users\\roxas1533\\source\\repos\\LINEDLL\\Debug\\LINEdll.dll";
#else
	char dllPath[] = "LINEdll.dll";
#endif
	ifs = std::ifstream((std::string(dllPath)));
	if (!ifs) {
		debugError("dllファイル存在確認失敗");
		MessageBoxA(NULL, "LINE.dllが存在しません", "LINEERROR", MB_OK);
		return 0;
	}
	else
		debugError("dllファイル存在確認OK");

	exePath.resize(exePath.size() - 2);
	exePath += " run -t 100\0";
	char* charPath = new char[exePath.size() + 1]; // メモリ確保
	std::char_traits<char>::copy(charPath, exePath.c_str(), exePath.size() + 1);
	bool isSuccess = CreateProcessA(0, charPath
		, NULL
		, NULL
		, FALSE
		, 0
		, NULL
		, NULL
		, &tStartupInfo
		, &tProcessInfomation
	);
	if (isSuccess) {
		debugError("プロセス開始成功");
		debugError(std::string("プロセス:") + std::to_string((INT_PTR)tProcessInfomation.hProcess));
		debugError(std::string("PID:") + std::to_string((INT_PTR)tProcessInfomation.dwProcessId));

		HINSTANCE hModule = LoadLibraryA(dllPath);
		if (hModule) {
			GetHWND setHwnd = (GetHWND)GetProcAddress(hModule, "setWind");
			HWND LINEHwnd = GetWindowHandle(tProcessInfomation.dwProcessId);
			while (!LINEHwnd)
				LINEHwnd = GetWindowHandle(tProcessInfomation.dwProcessId);
			while (LINEHwnd)
				LINEHwnd= GetWindowHandle(tProcessInfomation.dwProcessId);
			while (!LINEHwnd)
				LINEHwnd = GetWindowHandle(tProcessInfomation.dwProcessId);
			std::stringstream stream;
			stream << std::hex << LINEHwnd;
			debugError(std::string("ウィンドウハンドル取得:")+stream.str());
			setHwnd(LINEHwnd, hModule);
		}

	}
	else {
		debugError(std::string("起動に失敗しました。エラーコード:") + std::to_string(GetLastError()));
		MessageBoxA(NULL, (std::string("起動に失敗しました。エラーコード:") + std::to_string(GetLastError())).c_str(), "LINEERROR", MB_OK);
	}
	LPSTR libPath = dllPath;
	DWORD pathSize;
	pathSize = strlen(libPath) + 1;
	LPSTR remoteLibPath = (LPSTR)VirtualAllocEx(tProcessInfomation.hProcess, NULL, pathSize, MEM_COMMIT, PAGE_READWRITE);
	if (!remoteLibPath) {
		debugError(std::string("VirtualAllocEx失敗。エラーコード:") + std::to_string(GetLastError()));
		MessageBoxA(NULL, (std::string("対象プロセスのメモリ確保に失敗しました。エラーコード:") + std::to_string(GetLastError())).c_str(), "LINEERROR", MB_OK);
	}
	else {
		std::ostringstream address;
		address << (void*)remoteLibPath;
		debugError(std::string("VirtualAllocEx成功...開始アドレス:") + address.str());
		bool isWrite = WriteProcessMemory(tProcessInfomation.hProcess, remoteLibPath, libPath, pathSize, NULL);
		if (isWrite) {
			debugError("WriteProcessMemory成功");
			HANDLE newHandle = CreateRemoteThread(tProcessInfomation.hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, remoteLibPath, 0, NULL);
			if (newHandle) {
				debugError(std::string("CreateRemoteThread成功...ハンドル:") + std::to_string((long long)newHandle));
			}
			else {
				debugError(std::string("CreateRemoteThread失敗。エラーコード:") + std::to_string(GetLastError()));
				MessageBoxA(NULL, (std::string("dllを注入できませんでした。エラーコード:") + std::to_string(GetLastError())).c_str(), "LINEERROR", MB_OK);
			}
		}
		else {
			debugError(std::string("WriteProcessMemory失敗。エラーコード:") + std::to_string(GetLastError()));
			MessageBoxA(NULL, (std::string("対象プロセスのメモリ書き込みに失敗しました。エラーコード:") + std::to_string(GetLastError())).c_str(), "LINEERROR", MB_OK);
		}
	}
	if (isDebug)
		getchar();
	//ResumeThread(tProcessInfomation.hThread);
	::CloseHandle(tProcessInfomation.hProcess);
	::CloseHandle(tProcessInfomation.hThread);

	return 0;
}