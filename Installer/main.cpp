#include <iostream>
#include <vector>
#include <filesystem>
#include <windows.h>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <conio.h>
#include "resource.h"
#include "LnkClass.h"

int WriteResorce(int type, const char* name, std::filesystem::path targetDirectory,const char* exportName) {
	auto bin = FindResourceA(nullptr, MAKEINTRESOURCEA(type), name);
	if (!bin) {
		std::cout << name <<"リソースがありませんでした。\n";
		return 1;
	}
	auto ptr = LoadResource(nullptr, bin);
	if (!ptr) {
		std::cout << name <<"exeリソースの読み込みに失敗しました。\n";
		return 1;
	}
	auto size = SizeofResource(nullptr, bin);
	std::ofstream w(targetDirectory / exportName, std::ios::out | std::ios::binary | std::ios::trunc);
	w.write((char*)ptr, size);
	w.close();
	std::cout << exportName <<"をコピーしました。\n";
	return 0;
}

int main() {
	std::cout << "インストールを開始するには何かキーを押してください。\n";
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
	if (std::filesystem::create_directory(targetDirectory)) {
		std::cout << "LINECTRLフォルダを作成しました。\n";
		if (WriteResorce(IDR_EXE1, "EXE", targetDirectory, "LINECTRL.exe"))
			return 0;
		if (WriteResorce(IDR_DLL1, "DLL", targetDirectory, "LINEdll.dll"))
			return 0;
		if (WriteResorce(IDR_EXE2, "EXE", targetDirectory, "uninstall.exe"))
			return 0;
		makeLNK(FILE_ATTRIBUTE_SYSTEM, targetDirectory / "LINECTRL.exe", targetDirectory / "LINE.lnk", targetDirectory);
		makeLNK(FILE_ATTRIBUTE_SYSTEM, targetDirectory / "uninstall.exe", (root/startMenu).parent_path()/"LINECTRLunst.lnk", targetDirectory);
		std::cout << "ショートカットファイルを作成しました。";
		if (std::ifstream(root / desktop).is_open()) {
			std::filesystem::rename(root / desktop, targetDirectory / "LINE.lnk.back");
			std::filesystem::copy(targetDirectory / "LINE.lnk", root / desktop, std::filesystem::copy_options::overwrite_existing);
		}
		if (std::ifstream((root / startMenu)).is_open()) {
			std::filesystem::rename(root / startMenu, targetDirectory / "LINE.lnk.back");
			std::filesystem::copy(targetDirectory / "LINE.lnk", root / startMenu, std::filesystem::copy_options::overwrite_existing);
		}
		if (std::ifstream((root / startMenu2)).is_open()) {
			std::filesystem::rename(root / startMenu2, targetDirectory / "LINE.lnk.back");
			std::filesystem::copy(targetDirectory / "LINE.lnk", root / startMenu2, std::filesystem::copy_options::overwrite_existing);
		}
		if (std::ifstream((root / taskBar)).is_open()) {
			std::filesystem::rename(root / taskBar, targetDirectory / "LINE.lnk.back");
			std::filesystem::copy(targetDirectory / "LINE.lnk", root / taskBar, std::filesystem::copy_options::overwrite_existing);
		}
		std::cout << "オリジナルのショートカットファイルのバックアップを作成しました。\n";
		std::filesystem::remove(targetDirectory / "LINE.lnk");
		std::cout << "完了しました。終了するにはキーを押してください。\n";
	}else
		std::cout << "すでにインストール済みもしくはエラーです。インストール処理を中断します。\n";
	
	_getch();
}