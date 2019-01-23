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
	
	}

}
