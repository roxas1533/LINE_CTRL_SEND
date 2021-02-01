#include "LnkClass.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <exception>

std::filesystem::path getRelativePath(std::filesystem::path p, std::filesystem::path n) {
	return std::filesystem::relative(p, n);
}
bool ReadProcessMemory(SIZE_T& point, LPCVOID base, LPVOID lpBuffer, SIZE_T nSize) {
	point += nSize;
	return ReadProcessMemory(GetCurrentProcess(), base, lpBuffer, nSize, 0);
}

void makeLNK(DWORD fileAttribute, std::filesystem::path path, std::filesystem::path LnkPath, std::filesystem::path workingDirectroy) {
	std::wcout.imbue(std::locale(""));
	DWORD linkflag = HasLinkTargetIDList | HasLinkInfo | IsUnicode | HasRelativePath;
	if (workingDirectroy != "")
		linkflag |= HasWorkingDir;
	WIN32_FILE_ATTRIBUTE_DATA fileData;
	if (!std::filesystem::path(LnkPath).is_absolute()) {
		char dir[MAX_PATH];
		GetCurrentDirectoryA(MAX_PATH, dir);
		LnkPath = std::wstring(std::filesystem::current_path().wstring() + L"\\" + LnkPath.wstring());
	}

	if (!GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &fileData)) {
		std::cout << "ƒGƒ‰[>>" << GetLastError();
		return;
	}

	LnkClass newLnk;
	newLnk.lnkHeader.headerSize = 0x0000004C;
	HRESULT r;
	r = CLSIDFromString((LPCOLESTR)L"{00021401-0000-0000-C000-000000000046}", &(newLnk.lnkHeader.LinkCLSID));
	newLnk.lnkHeader.LinkFlags = linkflag;
	newLnk.lnkHeader.FileAttributes = fileAttribute;
	newLnk.lnkHeader.CreationTime = fileData.ftCreationTime;
	newLnk.lnkHeader.AccessTime = fileData.ftLastAccessTime;
	newLnk.lnkHeader.WriteTime = fileData.ftLastWriteTime;
	newLnk.lnkHeader.FileSize = fileData.nFileSizeLow;
	newLnk.lnkHeader.IconIndex = 0;
	newLnk.lnkHeader.ShowCommand = 1;
	newLnk.lnkHeader.HotKey = 0;
	newLnk.lnkHeader.Reserved1 = 0;
	newLnk.lnkHeader.Reserved2 = 0;
	newLnk.lnkHeader.Reserved3 = 0;
	std::ofstream writeLnk;
	writeLnk.open(LnkPath, std::ios::out | std::ios::binary | std::ios::trunc);
	writeLnk.write((char*)&(newLnk.lnkHeader), sizeof(LNK_HEADER));
	if (linkflag & HasLinkTargetIDList) {
		std::filesystem::path pathParse = path;
		for (;;) {
			GetFileAttributesExA(pathParse.string().c_str(), GetFileExInfoStandard, &fileData);
			std::string aa = pathParse.filename().string() + '\0';

			FileEntryItem* FEI = new FileEntryItem((BYTE)(pathParse.has_extension() ? 0x32 : 0x31), fileData.nFileSizeLow, fileData, (WORD)fileData.dwFileAttributes, aa);

			pathParse = pathParse.parent_path();
			newLnk.linkTargetIdList.add(FEI);

			if (pathParse.root_path() == pathParse) {
				VoluteShellItem* vls = new VoluteShellItem(0x2F, "C:\\");
				newLnk.linkTargetIdList.add(vls);
				break;
			}
		}
		newLnk.linkTargetIdList.add(new RootFolderShellItem());
		newLnk.linkTargetIdList.Write(writeLnk);
		if (linkflag & HasLinkInfo) {
			newLnk.lnkInfo = new LinkInfo(path);
			newLnk.lnkInfo->Write(writeLnk);
		}
		if (linkflag & HasRelativePath) {
			newLnk.relativePath = new StringData(getRelativePath(std::filesystem::path(path), std::filesystem::path(LnkPath)).u16string());
			newLnk.relativePath->Write(writeLnk);
		}
		if (linkflag & HasWorkingDir) {
			newLnk.workingDir = new StringData(workingDirectroy.u16string());
			newLnk.workingDir->Write(writeLnk);
		}
	}
	DWORD terminalBlockEx = 0;
	writeLnk.write((char*)&(terminalBlockEx), sizeof(DWORD));
	writeLnk.close();
}

std::string loadLNKPath(std::filesystem::path path) {
	LnkClass lnk;
	std::ifstream ifs("C:\\Users\\roxas1533\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\LINE.lnk", std::ios::in | std::ios::binary);
	ifs.read((char*)&(lnk.lnkHeader), sizeof(lnk.lnkHeader));
	if (lnk.lnkHeader.LinkFlags & HasLinkTargetIDList) {
		ifs.read((char*)&lnk.linkTargetIdList.Size, sizeof(lnk.linkTargetIdList.Size));
		BYTE* IDLIST = new BYTE[lnk.linkTargetIdList.Size];
		ifs.read((char*)IDLIST, lnk.linkTargetIdList.Size);
	}
	DWORD LinkInfoSize;
	ifs.read((char*)&LinkInfoSize, sizeof(LinkInfoSize));
	DWORD LinkInfoHeaderSize;
	ifs.read((char*)&LinkInfoHeaderSize, sizeof(LinkInfoHeaderSize));

	DWORD LinkInfoFlags;
	ifs.read((char*)&LinkInfoFlags, sizeof(LinkInfoFlags));
	bool isVolumeID = false;
	bool isLocalBasePath = false;
	bool isLocalBasePathUnicode = false;
	bool isCommonNetworkRelativeLink = false;

	if ((LinkInfoFlags & 1)) {
		isVolumeID = true;
		isLocalBasePath = true;
		if (LinkInfoHeaderSize > 0x24)
			isLocalBasePathUnicode = true;
	}
	if (((LinkInfoFlags >> 1) & 1)) {
		isCommonNetworkRelativeLink = true;
	}
	DWORD VolumeIDOffset;
	ifs.read((char*)&VolumeIDOffset, sizeof(VolumeIDOffset));
	if (isVolumeID && VolumeIDOffset == 0)
		throw std::exception();
	if (!isVolumeID && VolumeIDOffset > 0)
		throw std::exception();

	DWORD LocalBasePathOffset;
	ifs.read((char*)&LocalBasePathOffset, sizeof(LocalBasePathOffset));
	if (isLocalBasePath && LocalBasePathOffset == 0)
		throw std::exception();
	if (!isLocalBasePath && LocalBasePathOffset > 0)
		throw std::exception();


	DWORD CommonNetworkRelativeLinkOffset;
	ifs.read((char*)&CommonNetworkRelativeLinkOffset, sizeof(CommonNetworkRelativeLinkOffset));
	if (isCommonNetworkRelativeLink && CommonNetworkRelativeLinkOffset == 0)
		throw std::exception();
	if (!isCommonNetworkRelativeLink && CommonNetworkRelativeLinkOffset > 0)
		throw std::exception();

	DWORD CommonPathSuffixOffset;
	ifs.read((char*)&CommonPathSuffixOffset, sizeof(CommonPathSuffixOffset));
	if (LinkInfoHeaderSize > 0x24) {
		DWORD LocalBasePathOffsetUnicode;
		ifs.read((char*)&LocalBasePathOffsetUnicode, sizeof(LocalBasePathOffsetUnicode));
		if (isLocalBasePathUnicode && LocalBasePathOffsetUnicode == 0)
			throw std::exception();
		if (!isLocalBasePathUnicode && LocalBasePathOffsetUnicode > 0)
			throw std::exception();
		DWORD CommonPathSuffixOffsetUnicode;
		ifs.read((char*)&CommonPathSuffixOffsetUnicode, sizeof(CommonPathSuffixOffsetUnicode));
	}
	if (isVolumeID) {
		DWORD VolumeIDSize;
		ifs.read((char*)&VolumeIDSize, sizeof(VolumeIDSize));

		DWORD DriveType;
		ifs.read((char*)&DriveType, sizeof(DriveType));

		DWORD DriveSerialNumber;
		ifs.read((char*)&DriveSerialNumber, sizeof(DriveSerialNumber));

		DWORD VolumeLabelOffset;
		ifs.read((char*)&VolumeLabelOffset, sizeof(VolumeLabelOffset));

		if (VolumeIDOffset == 0x14) {
			DWORD VolumeLabelOffsetUnicode;
			ifs.read((char*)&VolumeLabelOffsetUnicode, sizeof(VolumeLabelOffsetUnicode));
		}
		char temp;
		std::string data;
		for (;;) {
			ifs.read((char*)&temp, sizeof(temp));
			data.push_back(temp);
			if (temp == '\0')
				break;
		}
	}
	if (isLocalBasePath) {
		char temp;
		std::string LocalBasePath;
		for (;;) {
			ifs.read((char*)&temp, sizeof(temp));
			LocalBasePath.push_back(temp);
			if (temp == '\0')
				break;
		}
		return LocalBasePath;
	}

	if (isCommonNetworkRelativeLink) {
		throw std::exception();
	}
	char temp;
	std::string CommonPathSuffix;
	for (;;) {
		ifs.read((char*)&temp, sizeof(temp));
		CommonPathSuffix.push_back(temp);
		if (temp == '\0')
			break;
	}
	if (LinkInfoHeaderSize > 0x24) {
		if (isLocalBasePathUnicode) {
			throw std::exception();
		}
		throw std::exception();
	}
	return nullptr;
}

LnkClass::LnkClass() :linkTargetIdList(LinkTargetIDList())
{
	lnkHeader = {};
}
