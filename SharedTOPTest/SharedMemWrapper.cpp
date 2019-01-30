#include "SharedMemWrapper.h"

#include <iostream>
#include "UT_SharedMem.h"
#include "TOP_SharedMemHeader.h"


SharedMemWrapper::SharedMemWrapper()
{
	bool trying_to_get_shared_tex = true;
	while (trying_to_get_shared_tex)
	{
		std::cout << "Creating a UT_SharedMem!\n";

		bool looping = true;
		UT_SharedMem* shm = new UT_SharedMem(L"TOPShm");

		if(hadError(shm))
		{ looping = false; }

		while (looping)
		{
			bool lock_success = shm->lock();

			if (lock_success)
			{
				TOP_SharedMemHeader* tmp = (TOP_SharedMemHeader*)shm->getMemory();
				if (!hadError(shm))
				{
					printPixelFormat(tmp);
					printDataFormat(tmp);
					printDataType(tmp);
					printData(tmp);
				}
				else
				{					
					looping = false;
				}
			}
			else
			{
				looping = false;
				printf("There was a problem getting a lock...\n");
			}
			shm->unlock();
		}
		delete shm;
	}
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

void SharedMemWrapper::printData(TOP_SharedMemHeader* tmp)
{
	int total_pixels = tmp->width * tmp->height * 4;
	unsigned char* t = (unsigned char*)tmp->getImage();
	for (int i = 0; i < total_pixels; i++)
	{
		//printf("%03.0d,", t[i]);
		printf("%#x,", t[i]);


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
