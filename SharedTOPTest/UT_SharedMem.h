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
  * COMMENTS:
  *		Refer to the wiki article on Using Shared Memory in TouchDesigner
  *		for info on this class.
  *
  */

#ifndef __UT_SharedMem__
#define __UT_SharedMem__

#include <string>


#include <winsock2.h>

typedef HANDLE ShmId;
typedef std::wstring ShmString;
typedef WCHAR ShmChar;
#define UT_SHM_INFO_DECORATION L"4jhd783h"

#define UT_SHM_INFO_MAGIC_NUMBER 0x56ed34ba


#define UT_SHM_MAX_POST_FIX_SIZE 32

typedef enum
{
	UT_SHM_ERR_NONE = 0,
	UT_SHM_ERR_ALREADY_EXIST,
	UT_SHM_ERR_DOESNT_EXIST,
	UT_SHM_ERR_INFO_ALREADY_EXIST,
	UT_SHM_ERR_INFO_DOESNT_EXIST,
	UT_SHM_ERR_UNABLE_TO_MAP,
} UT_SharedMemError;

// This is an internal class used by UT_SharedMem to handle
// resizing and closing of a memory segment
class UT_SharedMemInfo
{
public:
	int magicNumber;
	int version;
	bool supported;
	ShmChar namePostFix[UT_SHM_MAX_POST_FIX_SIZE];

	// version 2
	bool	detach;
};

#include "UT_Mutex.h"

// This is needed so that if people include this file from elsewhere
// it'll compile correctly
#ifndef UT_DLLEXP
#define UT_DLLEXP 
#endif 

class UT_DLLEXP UT_SharedMem
{
public:

	// Use this if you are the SENDER
	// size is in bytes
	UT_SharedMem(const ShmString &name, unsigned int size);

	// Use this one if tyou are the RECEIVER, you won't know the size of the
	// memory segment
	UT_SharedMem(const ShmString &name);


	~UT_SharedMem();

	// Get the size of the shared memory, only the SENDER will know the size
	// the RECEIVER will always get a value of 0
	unsigned int
		getSize() const
	{
		return mySize;
	}

	void		resize(unsigned int size);

	bool		lock();
	bool		unlock();


	//
	// detach will disassociate the shared memory segment from this process.
	// It returns true if the associated memory mapping has been freed.
	// It returns false if the memory mapping is still around after detaching,
	// this usually means someone else still has the mapping open.
	//
	bool		detach();

	//
	//  getMemory will return a pointer to the shared memory segment.
	//
	void*		getMemory();

	const ShmString&
		getName()
	{
		return myShortName;
	}

	UT_SharedMemError
		getErrorState()
	{
		return myErrorState;
	}


private:

	// Internal constructor, not for external use
	UT_SharedMem(const ShmString &name, unsigned int size, bool supportInfo);

	bool		open(const ShmString &name, unsigned int size = 0, bool supportInfo = true);

	bool		detachInternal();
	bool		checkInfo();
	void		randomizePostFix();
	void		createName();

	bool		createSharedMem();
	bool		openSharedMem();

	bool		createInfo();

	ShmString	myShortName;
	ShmString	myName;

	ShmString   tmpPNHolder;//object scope var used in checkinfo() as an optimization to avoid constant re-construction of this string object
	ShmChar		myNamePostFix[UT_SHM_MAX_POST_FIX_SIZE];
	unsigned int mySize;
	void*		myMemory;

	ShmId		 	myMapping;
	UT_Mutex*		myMutex;
	UT_SharedMem*	mySharedMemInfo;
	UT_SharedMemError	myErrorState;

	bool				myAmOwner;
	bool				mySupportInfo;

};

#endif /* __UT_SharedMem__ */
