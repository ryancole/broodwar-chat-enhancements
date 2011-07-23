#include <windows.h>
#include "patching.h"
#include "asm.h"
#include "queue.h"

int PatchAddress(int type, unsigned long AddressToPatch, unsigned long AddressToUse, unsigned long PadSize) {
    PadSize = PadSize == NULL ? 0 : PadSize;
	unsigned char CallOperand;

	switch(type) {
		case 1:
			CallOperand = 0xE9;
			break;

		case 2:
			CallOperand = 0xE8;
			break;

		default:
			CallOperand = 0xE9;
			break;
	}
    unsigned long DataBuffer   = (AddressToUse - (AddressToPatch + 5));
    unsigned long OldProtect   = 0;

    VirtualProtect((LPVOID)AddressToPatch, 5 + PadSize, PAGE_EXECUTE_READWRITE, &OldProtect);
    *(unsigned char*)AddressToPatch = CallOperand;
    *(unsigned long*)(AddressToPatch + 1) = DataBuffer;

    for (unsigned long x = PadSize; x > 0; x--) {
        *(unsigned char*)(AddressToPatch+5+x) = 0x90;
    }
    VirtualProtect((LPVOID)AddressToPatch, 5 + PadSize, OldProtect, &OldProtect);

    return 1;
}

unsigned long PatcherThread(void) {
	while(GetModuleHandle("battle.snp") == NULL) {
		Sleep(1000);
	}

	PatchAddress(2, 0x19017F1A, (DWORD)&HandleRecvWhisper, NULL);
	PatchAddress(2, 0x1901D8DB, (DWORD)&HandleOutgoingTextSub, NULL);
	PatchAddress(2, 0x1901DF48, (DWORD)&EnterChannelEventSub, NULL);
	queueManip.queueThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)QueueThread, NULL, NULL, NULL);
	return 0;
}