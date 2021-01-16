#pragma once

//int __cdecl strcmp(
//    _In_z_ char const* _Str1,
//    _In_z_ char const* _Str2
//);

void* RewriteFunction(const char* szRewriteModuleName, const char* szRewriteFunctionName, void* pRewriteFunctionPointer);
void PrintFunctions();