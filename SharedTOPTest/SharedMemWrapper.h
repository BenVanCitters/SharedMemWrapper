#pragma once

#include "SharedMemDelegate.h"
#include "UT_SharedMem.h"
class TOP_SharedMemHeader;

class SharedMemWrapper
{
public:
	SharedMemWrapper();
	SharedMemWrapper(const char* name, SharedMemDelegate* delgt);
	~SharedMemWrapper();

	bool initLoop();
	bool initLoop(const char* SharedMemName);
	bool processMem();
	bool terminateLoop();
	inline void setName(const char* name) { myName = name; }
	SharedMemDelegate* sharedMemDelegate;
private:
	bool hadError(UT_SharedMem * shm);
	void printError(UT_SharedMemError err);
	void printDataType(TOP_SharedMemHeader* tmp);
	void printPixelFormat(TOP_SharedMemHeader* tmp);
	int getChannelCount(TOP_SharedMemHeader * tmp);
	int getBytesPerPixel(TOP_SharedMemHeader * tmp);
	void printData(TOP_SharedMemHeader* tmp);
	void printDataFormat(TOP_SharedMemHeader* tmp);
	UT_SharedMem* shm;
	const char* myName;
};