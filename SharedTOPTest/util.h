#pragma once
#ifndef BVC_LPC_UTIL
#define BVC_LPC_UTIL

#include <windows.h>

LPCWSTR getLPCWSTRFromCharStar(const char* c)
{
	//	wchar_t* wtext = new wchar_t[20];
	//	//mbstowcs(wtext, name, strlen(name) + 1);//Plus null
	//	mbstowcs_s(wtext,name,)
	//	LPWSTR ptr = wtext;
	//	delete wtext;

	int strLength = strlen(c);
	wchar_t* wtext = new wchar_t[strLength];
	//mbstowcs(wtext, c, strLength + 1);//Plus null
	size_t x = 0;
	mbstowcs_s(&x, wtext, strLength+1, c, 99999);
	LPWSTR ptr = wtext;
	return ptr;
}
#endif