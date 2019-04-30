#include "ATTAC.h"
#include "TWBFind.h"
#include <time.h>

long Thread_ID;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}


long ATTAC_Time()
{
	time_t data;

	time(&data);

	return data;
}

long ATTAC_XOR(unsigned long a, unsigned long b)
{
	return (long) a ^ b;
}