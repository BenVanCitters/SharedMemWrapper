#include "SharedMemWrapper.h"

#include <iostream>
#include "UT_SharedMem.h"
#include "TOP_SharedMemHeader.h"
#include <chrono>

SharedMemWrapper::SharedMemWrapper() : SharedMemWrapper("no_name", NULL) {}

SharedMemWrapper::SharedMemWrapper(const char* name, SharedMemDelegate* delgt) : myName(name), sharedMemDelegate(delgt)
{}

//init the loop - make a sharedmem with a name...

bool SharedMemWrapper::initLoop()
{
	return initLoop(myName);
}
bool SharedMemWrapper::initLoop(const char* SharedMemName)
{
	if (shm)
	{
		shm->unlock(); //unlock?? fuck it!  might as well do this before deleting it forever!
		shm->detach();
		delete shm;
		shm = NULL;
	}
	//make a wchar out of a normal char star
	size_t returnValue; // The number of characters converted.
	size_t strln = strlen(SharedMemName);
	const size_t sizeInWords = strln+1; // The size of the wcstr buffer in words
	wchar_t *wc_name = new wchar_t[sizeInWords]; // we make this on the heap - but we don't delete...
	
	errno_t err = mbstowcs_s(&returnValue, wc_name, sizeInWords, SharedMemName, strln);

	std::cout << "Creating a UT_SharedMem!\n";
	shm = new UT_SharedMem(wc_name);
	delete[] wc_name;
	//return false on error
	return !hadError(shm);
}

//returns false on failure
bool SharedMemWrapper::processMem()
{
	auto start = std::chrono::high_resolution_clock::now();
	if (shm == NULL)
	{
		printf("shared mem isn't initialized - call initLoop first \n");
		return false;
	}
	if (hadError(shm))
	{
		return false;
	}
	bool lock_success = shm->lock();

	if (lock_success)
	{
		TOP_SharedMemHeader* tmp = (TOP_SharedMemHeader*)shm->getMemory();
		//not sure what can cause this but it definitely happens a lot
		if (tmp == NULL)
		{
			printf("getMem returned null - terminating loop.\n");
			shm->unlock();
			return false;
		}
		//bail if there was an error - haderror will print the error
		if (hadError(shm))
		{
			shm->unlock();
			return false;
		}
		if (sharedMemDelegate != NULL)
		{
			int dataSize = tmp->width * tmp->height * getBytesPerPixel(tmp);
			sharedMemDelegate->sharedMemDataCallback((unsigned char*)tmp->getImage(), dataSize, tmp->width, tmp->height);
		}
		//do stuff
		//printPixelFormat(tmp);
		//printDataFormat(tmp);
		//printDataType(tmp);
		//printData(tmp);
	}
	else
	{
		printf("There was a problem getting a lock on the shared memory block... try re-naming the shared mem out\n");
		return false;
	}
	shm->unlock();
	auto finish = std::chrono::high_resolution_clock::now();
	long long nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count();
	float millis = nanos / 1000000.f;
	//std::cout << "frame processing took: " << millis << "ms                       \r";
	//if we made it this far awesome!
	return true;
}

//cleans up the loop and returns the results of the unlock op
bool SharedMemWrapper::terminateLoop()
{
	bool unlock_success = shm->unlock();
	delete shm;
	shm = NULL;
	return unlock_success;
}

//returns true iff there was an error state in the ut shared mem object
bool SharedMemWrapper::hadError(UT_SharedMem* shm)
{
	UT_SharedMemError err = shm->getErrorState();
	bool hadErr = (err != UT_SHM_ERR_NONE);
	if (hadErr)
	{
		printError(err);
		std::cout << "bad news ahead!!" << std::endl;
	}
	return hadErr;
}

void SharedMemWrapper::printError(UT_SharedMemError err)
{
	switch (err)
	{
	case UT_SHM_ERR_NONE: std::cout << "err: UT_SHM_ERR_NONE" << std::endl;  break;
	case UT_SHM_ERR_ALREADY_EXIST: std::cout << "err: UT_SHM_ERR_ALREADY_EXIST" << std::endl;  break;
	case UT_SHM_ERR_DOESNT_EXIST: std::cout << "err: UT_SHM_ERR_DOESNT_EXIST" << std::endl;  break;
	case UT_SHM_ERR_INFO_ALREADY_EXIST: std::cout << "err: UT_SHM_ERR_INFO_ALREADY_EXIST" << std::endl;  break;
	case UT_SHM_ERR_INFO_DOESNT_EXIST: std::cout << "err: UT_SHM_ERR_INFO_DOESNT_EXIST" << std::endl;  break;
	case UT_SHM_ERR_UNABLE_TO_MAP: std::cout << "err: UT_SHM_ERR_UNABLE_TO_MAP" << std::endl;  break;
	default:
		printf("Unknown err: %#x\n", err);
		break;
	}
}

void SharedMemWrapper::printDataType(TOP_SharedMemHeader* tmp)
{
	printf("dataType: ");
	int typ = tmp->dataType;
	switch (typ)
	{
	case GL_UNSIGNED_BYTE: printf("GL_UNSIGNED_BYTE\n"); break;
	case GL_UNSIGNED_SHORT: printf("GL_UNSIGNED_SHORT\n"); break;
	case GL_FLOAT: printf("GL_FLOAT\n"); break;
	case GL_HALF_FLOAT_ARB: printf("GL_HALF_FLOAT_ARB\n"); break;
	default:
		printf("Unknown: %#x - %d\n", tmp->dataType, tmp->dataType);
		break;
	}
}
void SharedMemWrapper::printDataFormat(TOP_SharedMemHeader* tmp)
{
	printf("dataformat: ");
	switch (tmp->dataFormat)
	{
	case GL_RED: printf("GL_RED\n"); break;
	case GL_RG: printf("GL_RG\n"); break;
	case GL_RGB: printf("GL_RGB\n"); break;
	case GL_RGBA: printf("GL_RGBA\n"); break;
	case GL_LUMINANCE_ALPHA: printf("GL_LUMINANCE_ALPHA\n"); break;
	case GL_ALPHA: printf("GL_ALPHA\n"); break;
	case GL_LUMINANCE: printf("GL_LUMINANCE\n"); break;
	case GL_BGR_EXT: printf("GL_BGR_EXT\n"); break;
	case GL_BGRA_EXT: printf("GL_BGRA_EXT\n"); break;
	case GL_ABGR_EXT: printf("GL_ABGR_EXT\n"); break;
	default:
		printf("Unknown: %#x - %d\n", tmp->dataFormat, tmp->dataFormat);
		break;
	}
}

void SharedMemWrapper::printPixelFormat(TOP_SharedMemHeader* tmp)
{
	printf("pixelFormat: ");
	switch (tmp->pixelFormat)
	{
	case GL_RGBA8: printf("GL_RGBA8\n"); break;
	case GL_ALPHA8: printf("GL_ALPHA8\n"); break;
	case GL_ALPHA16: printf("GL_ALPHA16\n"); break;
	case GL_RGB10_A2: printf("GL_RGB10_A2\n"); break;
	case GL_RGBA16: printf("GL_RGBA16\n"); break;
	case GL_RGBA16F_ARB: printf("GL_RGBA16F_ARB\n"); break;
	case GL_RGBA32F_ARB: printf("GL_RGBA32F_ARB\n"); break;
	case GL_ALPHA16F_ARB: printf("GL_ALPHA16F_ARB\n"); break;
	case GL_ALPHA32F_ARB: printf("GL_ALPHA32F_ARB\n"); break;
	case GL_RGB32F_ARB: printf("GL_RGB32F_ARB\n"); break;
	case GL_R11F_G11F_B10F_EXT: printf("GL_R11F_G11F_B10F_EXT\n"); break;
	case GL_R8: printf("GL_R8\n"); break;
	case GL_R16: printf("GL_R16\n"); break;
	case GL_R16F: printf("GL_R16F\n"); break;
	case GL_R32F: printf("GL_R32F\n"); break;
	case GL_RG8: printf("GL_RG8\n"); break;
	case GL_RG16: printf("GL_RG16\n"); break;
	case GL_RG16F: printf("GL_RG16F\n"); break;
	case GL_RG32F: printf("GL_RG32F\n"); break;
	default:
		printf("Unknown: %#x - %d\n", tmp->pixelFormat, tmp->pixelFormat);
		break;
	}
}
int SharedMemWrapper::getChannelCount(TOP_SharedMemHeader* tmp)
{
	int result = -1;
	switch (tmp->pixelFormat)
	{
	case GL_RGBA8: result = 4; break;
	case GL_ALPHA8: result = 1; break;
	case GL_ALPHA16: result = 1; break;
	case GL_RGB10_A2: result = 4; break;
	case GL_RGBA16: result = 4; break;
	case GL_RGBA16F_ARB: result = 4; break;
	case GL_RGBA32F_ARB: 4; break;
	case GL_ALPHA16F_ARB: result = 1; break;
	case GL_ALPHA32F_ARB: result = 1; break;
	case GL_RGB32F_ARB: 3; break;
	case GL_R11F_G11F_B10F_EXT: result = 3; break;
	case GL_R8: result = 1; break;
	case GL_R16: result = 1; break;
	case GL_R16F: result = 1; break;
	case GL_R32F: result = 1; break;
	case GL_RG8: result = 2; break;
	case GL_RG16: result = 2; break;
	case GL_RG16F: result = 2; break;
	case GL_RG32F: result = 2; break;
	default:
		result = 4;
		printf("Unknown: %#x - %d\n", tmp->pixelFormat, tmp->pixelFormat);
		break;
	}
	return result;
}

int SharedMemWrapper::getBytesPerPixel(TOP_SharedMemHeader* tmp)
{
	int result = -1;
	switch (tmp->pixelFormat)
	{
	case GL_RGBA8: result = 4; break;
	case GL_ALPHA8: result = 1; break;
	case GL_ALPHA16: result = 2; break;
	case GL_RGB10_A2: result = 2; break;
	case GL_RGBA16: result = 2; break;
	case GL_RGBA16F_ARB: result = 8; break;
	case GL_RGBA32F_ARB: 16; break;
	case GL_ALPHA16F_ARB: result = 2; break;
	case GL_ALPHA32F_ARB: result = 4; break;
	case GL_RGB32F_ARB: 4; break;
	case GL_R11F_G11F_B10F_EXT: result = 4; break;
	case GL_R8: result = 1; break;
	case GL_R16: result = 2; break;
	case GL_R16F: result = 2; break;
	case GL_R32F: result = 4; break;
	case GL_RG8: result = 4; break;
	case GL_RG16: result = 4; break;
	case GL_RG16F: result = 4; break;
	case GL_RG32F: result = 8; break;
	default:
		result = 16;
		printf("Unknown: %#x - %d\n", tmp->pixelFormat, tmp->pixelFormat);
		break;
	}
	return result;
}
void SharedMemWrapper::printData(TOP_SharedMemHeader* tmp)
{
	int bytesPerPix = getBytesPerPixel(tmp);
	int total_pixels = tmp->width * tmp->height * bytesPerPix;
	unsigned char* t = (unsigned char*)tmp->getImage();
	const char* s = "%#.2x,";
	for (int i = 0; i < total_pixels; i++)
	{
		//std::cout << std::hex << std::setw(4) << t[i] << ", ";
		//printf("%03.0d,", t[i]);
		//printf("%#.2x,", t[i]);
		printf(s, t[i]);
	}
	printf("\n");
}


SharedMemWrapper::~SharedMemWrapper()
{
}
