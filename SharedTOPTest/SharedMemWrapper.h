#pragma once
class TOP_SharedMemHeader;

class SharedMemWrapper
{
public:
	SharedMemWrapper();
	~SharedMemWrapper();
	void printDataType(TOP_SharedMemHeader* tmp);
	void printPixelFormat(TOP_SharedMemHeader* tmp);
	void printData(TOP_SharedMemHeader* tmp);
	void printDataFormat(TOP_SharedMemHeader* tmp);
	TOP_SharedMemHeader* memMask;
};

