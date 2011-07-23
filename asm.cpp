#include <windows.h>
#include <stdio.h>
#include "asm.h"
#include "global.h"

void __declspec(naked) HandleRecvWhisper() {
	static const unsigned long originalFunction = 0x1901B480;
	char *replyto;
	replyto = lastwhisperfrom;

	__asm {
		pushad
		mov edx, dword ptr [edi + 8]
		cmp edx, 00000001h
		jnz done

		mov ebx, dword ptr [edi]
		push ebx
		push replyto
		call strcpy
		add esp, 08h

done:
		call originalFunction
		popad
		retn
	}
}

void __declspec(naked) HandleOutgoingTextSub() {
	//static unsigned long originalFunction = 0x19023850;
	__asm {
		pushad
		push eax
		call handleoutgoingtextptr
		add esp, 04h
		popad
		mov eax, 01h
		retn
	}
}

void __declspec(naked) AddChatSub(structAddChat *data) {
	static unsigned long originalFunction = 0x1901B480;

	__asm {
		pushad
		mov edi, eax
		call originalFunction
		popad
		retn
	}
}

void __declspec(naked) SendChatSub() {
	static unsigned long originalFunction = 0x19023850;
	static unsigned long addrlen = (unsigned long)(replytext + strlen(replytext));
	static unsigned long textlen = strlen(replytext)+1;
	
	if(replytext[0] == '/') {
		__asm {
			pushad
			mov eax, replytext
			mov ecx, 0h
			mov edx, textlen
			mov ebx, eax
			call originalFunction
			popad
			retn
		}
	} else {
		__asm {
			pushad
			mov eax, replytext
			mov ecx, addrlen
			mov ebx, eax
			call originalFunction
			popad
			retn
		}
	}
}

void __declspec(naked) EnterChannelEventSub() {
	static const unsigned long jmptoaddr = 0x1901DF4D;
	static const unsigned long fixcode = 0x1903DA90;
	__asm {
		pushad
		call enterchanneleventptr
		popad
		add esp, 04h
		push fixcode
		jmp jmptoaddr
	}
}

void __declspec(naked) CreateMessageBox() {
	static const unsigned long originalFunction = 0x19019610;
	__asm {
		pushad
		push messageboxtext
		call originalFunction
		popad
		retn
	}
}