#pragma once
struct SharedMemDelegate
{
	virtual void sharedMemDataCallback(unsigned char* data, int size, unsigned int width, unsigned int height) = 0;
};