#include <windows.h>
#include "patching.h"
#include "asm.h"
#include "global.h"

// brood war chat enhancements by warz (starcrap) 2004

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved) {
    switch (dwReason) {
		case DLL_PROCESS_ATTACH: {
				LARGE_INTEGER Counter, Frequency;
				QueryPerformanceCounter(&Counter);
				QueryPerformanceFrequency(&Frequency);

				startTime = Counter.QuadPart / Frequency.QuadPart;
				strcpy(lastwhisperfrom, "Anonymous");
				strcpy(lastwhisperto, "Anonymous");
				CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)PatcherThread, NULL, 0, NULL);
				break;
		}

		case DLL_PROCESS_DETACH: {
				break;
		}
    }

	return true;
}