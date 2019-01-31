// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "SharedMemWrapper.h"


int main()
{
	SharedMemWrapper smw;
	while (true)
	{
		bool running = smw.initLoop("TOPShm");
		while (running)
		{
			running = smw.processMem();
		}
		smw.terminateLoop();
	}
}
