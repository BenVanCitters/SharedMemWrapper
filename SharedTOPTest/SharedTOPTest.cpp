// SharedTOPTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "UT_SharedMem.h"
int main()
{
	UT_SharedMem *shm = new UT_SharedMem("mymemoryname");

    std::cout << "Hello World!\n"; 
	while (true)
	{
		if(shm->lock());
		{
			//Now get the pointer of the memory

			void *data = shm->getMemory();
			char* tmp = (char*)data;
			//Once you have this pointer, what you do with it depends on the type of OP you are communicating with.Finally, unlock once done read / writing the data.
			for (int i = 0; i < 2000; i++)
			{
				printf("%d,", tmp[i]);
			}
			printf("\n");
			//
		}
		shm->unlock();
	}

}
