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
		std::cout << name <<"���\�[�X������܂���ł����B\n";
		return 1;
	}
	auto ptr = LoadResource(nullptr, bin);
	if (!ptr) {
		std::cout << name <<"exe���\�[�X�̓ǂݍ��݂Ɏ��s���܂����B\n";
		return 1;
	}
	auto size = SizeofResource(nullptr, bin);
	std::ofstream w(targetDirectory / exportName, std::ios::out | std::ios::binary | std::ios::trunc);
	w.write((char*)ptr, size);
	w.close();
	std::cout << exportName <<"���R�s�[���܂����B\n";
	return 0;
}

int main() {
	std::cout << "�C���X�g�[�����J�n����ɂ͉����L�[�������Ă��������B\n";
	_getch();
	wchar_t c[] = L"C:\\Users\\%username%";
	std::wstring exePath;
	exePath.resize(ExpandEnvironmentStringsW(c, nullptr, 0));
	if (ExpandEnvironmentStringsW(c, &exePath.front(), exePath.size()) == NULL) {
		MessageBoxA(NULL, "���ϐ����p�X�ɕϊ��ł��܂���ł����B", "LINEERROR", MB_OK);
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
		std::cout << "LINECTRL�t�H���_���쐬���܂����B\n";
		if (WriteResorce(IDR_EXE1, "EXE", targetDirectory, "LINECTRL.exe"))
			return 0;
		if (WriteResorce(IDR_DLL1, "DLL", targetDirectory, "LINEdll.dll"))
			return 0;
		if (WriteResorce(IDR_EXE2, "EXE", targetDirectory, "uninstall.exe"))
			return 0;
		makeLNK(FILE_ATTRIBUTE_SYSTEM, targetDirectory / "LINECTRL.exe", targetDirectory / "LINE.lnk", targetDirectory);
		makeLNK(FILE_ATTRIBUTE_SYSTEM, targetDirectory / "uninstall.exe", (root/startMenu).parent_path()/"LINECTRLunst.lnk", targetDirectory);
		std::cout << "�V���[�g�J�b�g�t�@�C�����쐬���܂����B";
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
		std::cout << "�I���W�i���̃V���[�g�J�b�g�t�@�C���̃o�b�N�A�b�v���쐬���܂����B\n";
		std::filesystem::remove(targetDirectory / "LINE.lnk");
		std::cout << "�������܂����B�I������ɂ̓L�[�������Ă��������B\n";
	}else
		std::cout << "���łɃC���X�g�[���ς݂������̓G���[�ł��B�C���X�g�[�������𒆒f���܂��B\n";
	
	_getch();
}