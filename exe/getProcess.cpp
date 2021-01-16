#include "getProcess.h"

#include <windows.h>
#include <psapi.h>
#include <iostream>
#include <string>
#include <algorithm>

int getProcess() {
    DWORD allProc[1024];
    DWORD cbNeeded;
    int nProc;
    int i;

    // PID一覧を取得
    if (!EnumProcesses(allProc, sizeof(allProc), &cbNeeded)) {
        return 1;
    }

    nProc = cbNeeded / sizeof(DWORD);

    for (i = 0; i < nProc; i++) {
        wchar_t procName[MAX_PATH] = TEXT("<unknown>");

        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
            PROCESS_VM_READ,
            FALSE, allProc[i]);

        if (NULL != hProcess) {
            HMODULE hMod;
            DWORD cbNeeded;

            if (EnumProcessModules(hProcess, &hMod, sizeof(hMod),
                &cbNeeded)) {
                GetModuleBaseName(hProcess, hMod, procName,
                    sizeof(procName) / sizeof(TCHAR));
            }
            // プロセス名とPIDを表示
            if (std::wstring(procName) == L"LINE.exe") {
                CloseHandle(hProcess);
                return 1;
            }

            CloseHandle(hProcess);

        }

    }

    return 0;
}

HWND GetWindowHandle(	// 戻り値: 成功 望みのHWND / 失敗 NULL
    const DWORD TargetID)	// プロセスID
{
    HWND hWnd = GetTopWindow(NULL);
    do {
        if (GetWindowLong(hWnd, GWL_HWNDPARENT) != 0 || !IsWindowVisible(hWnd))
            continue;
        DWORD ProcessID;
        GetWindowThreadProcessId(hWnd, &ProcessID);
        if (TargetID == ProcessID) {
            return hWnd;
        }
    } while ((hWnd = GetNextWindow(hWnd, GW_HWNDNEXT)) != NULL);

    return NULL;
}