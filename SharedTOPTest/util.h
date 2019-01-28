#pragma once
#ifndef BVC_LPC_UTIL
#define BVC_LPC_UTIL

#include <windows.h>

inline LPCWSTR getLPCWSTRFromCharStar(const char* c)
{
	//	wchar_t* wtext = new wchar_t[20];
	//	//mbstowcs(wtext, name, strlen(name) + 1);//Plus null
	//	mbstowcs_s(wtext,name,)
	//	LPWSTR ptr = wtext;
	//	delete wtext;

	int strLength = strlen(c)+1;
	size_t numCharsConverted;
	wchar_t* wtext = new wchar_t[strLength];

	//delete[] wtext;
	//mbstowcs(wtext, c, strLength + 1);//Plus null
	
	mbstowcs_s(&numCharsConverted, wtext, strLength+1, c, 99999);
	LPWSTR ptr = wtext;
	return ptr;
}
#endif