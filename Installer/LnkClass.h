#pragma once
#include "lnkStruct.h"
#include "combaseapi.h"
#include <string>
#include <iostream>
#include <filesystem>
class LnkClass {
public:
	LnkClass();
	LNK_HEADER lnkHeader;
	LinkTargetIDList linkTargetIdList;
	LinkInfo* lnkInfo;
	StringData* nameString;
	StringData* relativePath;
	StringData* workingDir;
	StringData* commandLineArguments;
	StringData* iconLocation;
};

void makeLNK(DWORD fileAttribute, std::filesystem::path path, std::filesystem::path LnkPath, std::filesystem::path workingDirectory="");
std::string loadLNKPath(std::filesystem::path path);