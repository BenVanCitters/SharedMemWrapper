/* Shared Use License: This file is owned by Derivative Inc. (Derivative) and
 * can only be used, and/or modified for use, in conjunction with
 * Derivative's TouchDesigner software, and only if you are a licensee who has
 * accepted Derivative's TouchDesigner license or assignment agreement (which
 * also govern the use of this file).  You may share a modified version of this
 * file with another authorized licensee of Derivative's TouchDesigner software.
 * Otherwise, no redistribution or sharing of this file, with or without
 * modification, is permitted.
 */

 /*
  * Filename: UT_Mutex.C
  */

#include <assert.h>

#include "UT_Mutex.h"

  // We don't include the leakwatch here
  // since this file is used by users

UT_Mutex::UT_Mutex(const MtxString &name)
{
	myMutex = CreateMutexW(NULL, FALSE, name.data());


}


UT_Mutex::~UT_Mutex()
{
	CloseHandle(myMutex);
}

bool
UT_Mutex::lock(int timeout)
{
	DWORD result = WaitForSingleObject(myMutex, timeout);
	if (result != WAIT_OBJECT_0)
		return false;
	else
		return true;
}

bool
UT_Mutex::unlock()
{
	return ReleaseMutex(myMutex);
}
