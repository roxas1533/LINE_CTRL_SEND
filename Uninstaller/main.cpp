#include <windows.h>
#include <iostream>
#include <conio.h>
#include <filesystem>
#include <fstream>

int main(int argc, char* argv[])
{
	STARTUPINFOA si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	si.cb = sizeof(si);

	if (argc == 1)
	{

		std::cout << "アンインストールを開始するには何かキーを押してください。\n";
		_getch();
		wchar_t c[] = L"C:\\Users\\%username%";
		std::wstring exePath;
		exePath.resize(ExpandEnvironmentStringsW(c, nullptr, 0));
		if (ExpandEnvironmentStringsW(c, &exePath.front(), exePath.size()) == NULL) {
			MessageBoxA(NULL, "環境変数をパスに変換できませんでした。", "LINEERROR", MB_OK);
			return 0;
		}
		std::filesystem::path rootPath = exePath;
		std::string root = rootPath.string();
		root.erase(--root.end());
		rootPath = root;
		std::filesystem::path desktop = "Desktop\\LINE.lnk";
		std::filesystem::path startMenu = "AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\LINE.lnk";
		std::filesystem::path startMenu2 = "AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\LINE\\LINE.lnk";
		std::filesystem::path taskBar = "AppData\\Roaming\\Microsoft\\Internet Explorer\\Quick Launch\\User Pinned\\TaskBar\\LINE.lnk";

		std::filesystem::path targetDirectory = root / std::filesystem::path("AppData\\Roaming\\LINECTRL");

		if (!std::ifstream(targetDirectory / "LINE.lnk.back")) {
			std::cout << "LINE.lnk.backが存在しません。ファイルをスキップします。\n";
		}
		else {
			if (std::ifstream(root / desktop).is_open()) {
				std::filesystem::copy(targetDirectory / "LINE.lnk.back", root / desktop, std::filesystem::copy_options::overwrite_existing);
			}
			if (std::ifstream((root / startMenu)).is_open()) {
				std::filesystem::copy(targetDirectory / "LINE.lnk.back", root / startMenu, std::filesystem::copy_options::overwrite_existing);
			}
			if (std::ifstream((root / startMenu2)).is_open()) {
				std::filesystem::copy(targetDirectory / "LINE.lnk.back", root / startMenu2, std::filesystem::copy_options::overwrite_existing);
			}
			if (std::ifstream((root / taskBar)).is_open()) {
				std::filesystem::copy(targetDirectory / "LINE.lnk.back", root / taskBar, std::filesystem::copy_options::overwrite_existing);
			}
			std::filesystem::remove((rootPath / startMenu).parent_path() / "LINECTRLunst.lnk");
		}


		std::filesystem::path orig = argv[0];
		orig.replace_filename("2.exe");
		std::string deletePath = (std::filesystem::temp_directory_path() / "un103.exe").string() + " " + orig.string() + " " + targetDirectory.string();
		std::cout << deletePath;
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(sa);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;

		std::filesystem::copy(argv[0], std::filesystem::temp_directory_path() / "un103.exe", std::filesystem::copy_options::overwrite_existing);
		MoveFileA(argv[0], "2.exe");
		HANDLE h = CreateFileA((std::filesystem::temp_directory_path() / "un103.exe").string().c_str(), 0, FILE_SHARE_READ, &sa,
			OPEN_EXISTING, FILE_FLAG_DELETE_ON_CLOSE, NULL);
		CreateProcessA(NULL, (LPSTR)deletePath.c_str(), NULL, NULL,
			TRUE, 0, NULL, std::filesystem::temp_directory_path().string().c_str(), &si, &pi);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		CloseHandle(h);
	}
	else if (argc == 3)
	{
		bool flag = false;
		while (!flag)
		{
			try {
				std::filesystem::remove_all(argv[2]);
				flag = true;
			}
			catch (std::filesystem::filesystem_error e) {
				std::cout << e.what() << "\n";
			}
		}

		std::cout << "アンインストールが完了しました\n";
		_getch();
		CreateProcessA(NULL, (LPSTR)"notepad", NULL, NULL, TRUE,
			DEBUG_ONLY_THIS_PROCESS, NULL, NULL, &si, &pi);
	}
}