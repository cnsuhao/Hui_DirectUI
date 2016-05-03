// KDUI32.cpp : ���� DLL Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "KDUI32.h"
#include <math.h>
#include <Windowsx.h>

#pragma comment(lib, "msimg32.lib")

#ifdef _MANAGED
#pragma managed(push, off)
#endif

//////
GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR gdiplusToken;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		GdiplusShutdown(gdiplusToken);
		break;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

