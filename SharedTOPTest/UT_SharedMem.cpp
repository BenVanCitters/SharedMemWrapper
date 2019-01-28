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
 * Filename: UT_SharedMem.C
 */

#ifdef WIN32
#else
   #include <string.h>
   #define _strdup(s) strdup(s) 
#endif 
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include "UT_SharedMem.h"

bool
UT_SharedMem::open(const ShmString &name,  unsigned int size, bool supportInfo)
{
	mySize = size;
	myMemory = 0;
	myMapping = 0;
	mySharedMemInfo = NULL;
    memset(myNamePostFix, 0, sizeof(myNamePostFix));

    myShortName = name;

	mySupportInfo = supportInfo;

    createName();

	ShmString m;
	m = myName;
#ifdef WIN32
	m += L"Mutex";
#else
	m += "Mutex";
#endif

    myMutex = new UT_Mutex(m);

    if (size > 0)
		myAmOwner = true;
    else
		myAmOwner = false;

    if (supportInfo)
    {
		if (!createInfo())
		{
			return false;
		}
    }
    else
    {
		mySharedMemInfo = NULL;
    }

    if (size > 0)
    {
		if (!createSharedMem())
		{
		    myErrorState = UT_SHM_ERR_ALREADY_EXIST;
		    return false;
		}
    }
    else
    {
		if (!openSharedMem())
		{
		    myErrorState = UT_SHM_ERR_DOESNT_EXIST;
		    return false;
		}
    }
    myErrorState = UT_SHM_ERR_NONE;
	return true;
}

UT_SharedMem::UT_SharedMem(const ShmString &name)
{
	open(name);
}

UT_SharedMem::UT_SharedMem(const ShmString &name, unsigned int size)
{
	open(name, size);
}

UT_SharedMem::UT_SharedMem(const ShmString &name, unsigned int size, bool supportInfo)
{
	open(name, size, supportInfo);
}

UT_SharedMem::~UT_SharedMem()
{
    detach();
    delete mySharedMemInfo;
    delete myMutex;
}

bool
UT_SharedMem::checkInfo()
{
	if (mySupportInfo)
	{
		// If we are looking for an info and can't find it,
		// then release the segment also
		if (!createInfo())
		{
			detach();
			myErrorState = UT_SHM_ERR_INFO_DOESNT_EXIST;
			return false;
		}
	}

    if (mySharedMemInfo && mySharedMemInfo->getErrorState() == UT_SHM_ERR_NONE && !myAmOwner)
    {
		mySharedMemInfo->lock();
		UT_SharedMemInfo *info = (UT_SharedMemInfo*)mySharedMemInfo->getMemory();

		if (info->version > 1)
		{
			if (info->detach)
			{
				mySharedMemInfo->unlock();
				detach();
				myErrorState = UT_SHM_ERR_INFO_DOESNT_EXIST;
				return false;
			}
		}

		ShmString pn;
		pn = info->namePostFix;

		if (pn != myNamePostFix)
		{
			memcpy(myNamePostFix, pn.data(), UT_SHM_MAX_POST_FIX_SIZE * sizeof(ShmChar));
		    detachInternal();
		}
		mySharedMemInfo->unlock();

    }
	return true;
}

void
UT_SharedMem::resize(unsigned int s)
{

    // This can't be called by someone that didn't create it in the first place
    // Also you can't resize it if you arn't using the info feature
    // Finally, don't set the size to 0, just delete this object if you want to clean it
    if (mySize > 0 && mySharedMemInfo && myAmOwner)
    {
		mySharedMemInfo->lock();
		UT_SharedMemInfo *info = (UT_SharedMemInfo*)mySharedMemInfo->getMemory();
		if (info && info->supported)
		{
		    detachInternal();
		    mySize = s;
		    // Keep trying until we find a name that works
		    do 
		    {
				randomizePostFix();
				createName();
		    } while(!createSharedMem());
		    memcpy(info->namePostFix, myNamePostFix, UT_SHM_MAX_POST_FIX_SIZE * sizeof(ShmChar));
		}
		else // Otherwise, just try and detach and resize, if it fails give up
		{
		    detachInternal();
		    mySize = s;
		    if (!createSharedMem())
		    {
				myErrorState = UT_SHM_ERR_ALREADY_EXIST;
		    }

		}
		mySharedMemInfo->unlock();
    }
}

void
UT_SharedMem::randomizePostFix()
{
    for (int i = 0; i < UT_SHM_MAX_POST_FIX_SIZE - 1; i++)
    {
		int r = rand() % 26;
		char ch = 'a' + r;
		myNamePostFix[i] = ch;
    }
}

void
UT_SharedMem::createName()
{
#ifdef WIN32
	myName = L"TouchSHM";
#else
	myName = "TouchSHM";
#endif

	myName += myShortName;
	myName += myNamePostFix;
}

bool
UT_SharedMem::createSharedMem()
{
    if (myMapping)
		return true;

#ifdef WIN32 
    myMapping = CreateFileMappingW(INVALID_HANDLE_VALUE, 
								  NULL,
								  PAGE_READWRITE,
								  0,
								  mySize,
								  myName.data());

    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
		detach();
		return false;
    }
#else
    assert(false);
#endif 

    if (myMapping)
		return true;
    else
		return false;
}

bool
UT_SharedMem::openSharedMem()
{
    if (myMapping)
		return true;
    createName();
#ifdef WIN32 
    myMapping = OpenFileMappingW( FILE_MAP_ALL_ACCESS, FALSE, myName.data());
#else
    assert(false);
#endif 

    if (!myMapping)
		return false;


   return true;
}

bool
UT_SharedMem::detachInternal()
{
    if (myMemory)
    {
#ifdef WIN32 
		UnmapViewOfFile(myMemory);
#else
        assert(false);
#endif 
		myMemory = 0;
    }
    if (myMapping)
    {
#ifdef WIN32 
		CloseHandle(myMapping);
#else
        assert(false);
#endif 
		myMapping = 0;
    }


    // Try to open the file again, if it works then someone else is still holding onto the file
    if (openSharedMem())
    {
#ifdef WIN32
		CloseHandle(myMapping);
#else
        assert(false);
#endif 
		myMapping = 0;
		return false;
    }
    
		    
    return true;
}


bool
UT_SharedMem::detach()
{
    if (mySharedMemInfo)
    {
		if (mySharedMemInfo->getErrorState() == UT_SHM_ERR_NONE)
		{
			mySharedMemInfo->lock();
			UT_SharedMemInfo *info = (UT_SharedMemInfo*)mySharedMemInfo->getMemory();
			if (info && myAmOwner)
			{
				info->detach = true;
			}
			mySharedMemInfo->unlock();
		}
		delete mySharedMemInfo;
		mySharedMemInfo = NULL;
	}
	memset(myNamePostFix, 0, sizeof(myNamePostFix));
	return detachInternal();
}

bool
UT_SharedMem::createInfo()
{
	if (!mySupportInfo)
		return true;
	if (mySharedMemInfo)
	{
		return mySharedMemInfo->getErrorState() == UT_SHM_ERR_NONE;
	}

	srand(time(NULL));
	ShmString infoName;
	infoName += myName;
	infoName += UT_SHM_INFO_DECORATION;

	mySharedMemInfo = new UT_SharedMem(infoName, 
									   myAmOwner ? sizeof(UT_SharedMemInfo) : 0, false);

	if (myAmOwner)
	{
		if (mySharedMemInfo->getErrorState() != UT_SHM_ERR_NONE)
		{
			myErrorState = UT_SHM_ERR_INFO_ALREADY_EXIST;
			return false;
		}
		mySharedMemInfo->lock();
		UT_SharedMemInfo *info = (UT_SharedMemInfo*)mySharedMemInfo->getMemory();
		if (!info)
		{
			myErrorState = UT_SHM_ERR_UNABLE_TO_MAP;
			mySharedMemInfo->unlock();
			return false;
		}
		info->magicNumber = UT_SHM_INFO_MAGIC_NUMBER;
		info->version = 2;
		info->supported = false;
		info->detach = false;
		memset(info->namePostFix, 0, UT_SHM_MAX_POST_FIX_SIZE);
		mySharedMemInfo->unlock();
	}
	else
	{
		if (mySharedMemInfo->getErrorState() != UT_SHM_ERR_NONE)
		{
			myErrorState = UT_SHM_ERR_INFO_DOESNT_EXIST;
			return false;
		}
		mySharedMemInfo->lock();
		UT_SharedMemInfo *info = (UT_SharedMemInfo*)mySharedMemInfo->getMemory();
		if (!info)
		{
			myErrorState = UT_SHM_ERR_UNABLE_TO_MAP;
			mySharedMemInfo->unlock();
			return false;
		}
		if (info->magicNumber != UT_SHM_INFO_MAGIC_NUMBER)
		{
			myErrorState = UT_SHM_ERR_INFO_DOESNT_EXIST;
			mySharedMemInfo->unlock();
			return false;
		}
		// Let the other process know that we support the info
		info->supported = true;
		mySharedMemInfo->unlock();
	}

	return true;
}

void *
UT_SharedMem::getMemory()
{
	if (!checkInfo())
	{
		return NULL;
	}

    if( myMemory == 0 )
    {
		if ((myAmOwner && createSharedMem()) || (!myAmOwner && openSharedMem()))
		{
#ifdef WIN32 
		    myMemory = MapViewOfFile(myMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
#else
		    assert(false);
		    myMemory = NULL;
#endif 
		    if (!myMemory)
				myErrorState = UT_SHM_ERR_UNABLE_TO_MAP;
		}
    }
    if (myMemory)
    {
		myErrorState = UT_SHM_ERR_NONE;
    }
    return myMemory;
}

bool
UT_SharedMem::lock()
{
    return myMutex->lock(5000);
}

bool
UT_SharedMem::unlock()
{
    return myMutex->unlock();
}
