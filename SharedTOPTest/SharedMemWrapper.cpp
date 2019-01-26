#include "SharedMemWrapper.h"

#include <iostream>
#include "UT_SharedMem.h"
#include "TOP_SharedMemHeader.h"


SharedMemWrapper::SharedMemWrapper()
{
	std::cout << "Hello World!\n";
	UT_SharedMem *shm = new UT_SharedMem("mymemoryname");
	while (true)
	{
		bool lock_success = shm->lock();
		if (lock_success)
		{			
			TOP_SharedMemHeader* tmp = (TOP_SharedMemHeader*)shm->getMemory();
			printPixelFormat(tmp);
			printDataFormat(tmp);
			printDataType(tmp);
			printData(tmp);
		}
		shm->unlock();
	}
	delete shm;
}

void SharedMemWrapper::printDataType(TOP_SharedMemHeader* tmp)
{
	int typ = tmp->dataType;
	switch (typ)
	{
	case GL_UNSIGNED_BYTE:
		printf("GL_UNSIGNED_BYTE\n"); break;
	case GL_UNSIGNED_SHORT:
		printf("GL_UNSIGNED_SHORT\n"); break;
	case GL_FLOAT:
		printf("GL_FLOAT\n"); break;
	case GL_HALF_FLOAT_ARB:
		printf("GL_HALF_FLOAT_ARB\n"); break;
	default:
		break;
	}

}
void SharedMemWrapper::printDataFormat(TOP_SharedMemHeader* tmp)
{
	switch (tmp->dataFormat)
	{
	case GL_RED:
		printf("GL_RED\n"); break;
	case GL_RG:
		printf("GL_RG\n"); break;
	case GL_RGB:
		printf("GL_RGB\n"); break;
	case GL_RGBA:
		printf("GL_RGBA\n"); break;
	case GL_LUMINANCE_ALPHA:
		printf("GL_LUMINANCE_ALPHA\n"); break;
	case GL_ALPHA:
		printf("GL_ALPHA\n"); break;
	case GL_LUMINANCE:
		printf("GL_LUMINANCE\n"); break;
	case GL_BGR_EXT:
		printf("GL_BGR_EXT\n"); break;
	case GL_BGRA_EXT:
		printf("GL_BGRA_EXT\n"); break;
	case GL_ABGR_EXT:
		printf("GL_ABGR_EXT\n"); break;
	default:
		break;
	}
}

void SharedMemWrapper::printPixelFormat(TOP_SharedMemHeader* tmp)
{
	switch (tmp->pixelFormat)
	{
	case GL_RGBA8: printf("GL_RGBA8\n"); break;
	case GL_ALPHA8: printf("GL_ALPHA8\n"); break;
	case GL_ALPHA16: printf("GL_ALPHA16\n"); break;
	case GL_RGB10_A2: printf("GL_RGB10_A2\n"); break;
	case GL_RGBA16: printf("GL_RGBA16\n"); break;
	case GL_RGBA16F_ARB : printf("GL_RGBA16F_ARB\n"); break;
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
		break;
	}
}

void SharedMemWrapper::printData(TOP_SharedMemHeader* tmp)
{
	int total_pixels = tmp->width * tmp->height*4;
	unsigned char* t = (unsigned char*)tmp->getImage();
	for (int i = 0; i < total_pixels; i++)
	{
		printf("%03.0d,", t[i]);
		
	}
	printf("\n");
}

//void oldMeth(UT_SharedMem *shm)
//{
//
//	void *data = shm->getMemory();
//	unsigned char* tmp = (unsigned char*)data;
//	//Once you have this pointer, what you do with it depends on the type of OP you are communicating with.Finally, unlock once done read / writing the data.
//	for (int i = 0; i < 200; i++)
//	{
//		printf("%03.0d,", tmp[i]);
//	}
//	printf("\n");
//}

SharedMemWrapper::~SharedMemWrapper()
{
}
