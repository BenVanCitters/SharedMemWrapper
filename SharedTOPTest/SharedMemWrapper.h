#pragma once

#include "UT_SharedMem.h"
class TOP_SharedMemHeader;

class SharedMemWrapper
{
public:
	SharedMemWrapper();
	~SharedMemWrapper();

private:
	bool hadError(UT_SharedMem * shm);
	void printError(UT_SharedMemError err);	
	void printDataType(TOP_SharedMemHeader* tmp);
	void printPixelFormat(TOP_SharedMemHeader* tmp);
	void printData(TOP_SharedMemHeader* tmp);
	void printDataFormat(TOP_SharedMemHeader* tmp);
	TOP_SharedMemHeader* memMask;
};

