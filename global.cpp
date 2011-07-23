#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include "global.h"
#include "asm.h"
#include "queue.h"

char *lastwhisperfrom = new char[64];
char *lastwhisperto = new char[64];
char *replytext = new char[264];
char *messageboxtext = new char[264];
char *currentchannel = (char*)0x1903D900;
__int64 startTime;

funcptr_1 *handleoutgoingtextptr = HandleOutgoingText;
funcptr_2 *enterchanneleventptr = EnterChannelEvent;

void EnterChannelEvent() {
	AddMessage("-> You are using BWEnhancements.");
}

bool __cdecl HandleOutgoingText(unsigned long textaddr) {
	char *text = (char*)textaddr;

	if(!strncmp(text, "/ver", 4) || !strncmp(text, "/version", 8)) {
		AddMessage("-> BWEnhancements - Copyright (C) 2004 BroodwarThreat - A StarCrap production (www.rafm.org of warz).");
		return true;
	} else if(!strncmp(text, "/r ", 3) || !strncmp(text, "/reply ", 7)) {
		QuickReply(text);
		return true;
	} else if(!strncmp(text, "/w ", 3) || !strncmp(text, "/m ", 3) || !strncmp(text, "/msg ", 5) || !strncmp(text, "/whisper ", 9)) {
		SetWhisperBackTarget(text);
		return false;
	} else if(!strncmp(text, "/wb ", 4)) {
		SendWhisperBack(text+4);
		return true;
	} else if(!strncmp(text, "/sgn ", 5)) {
		if(strlen(text) > 5) {
			SpoofLobbyName(text+5);
			AddMessage("-> Spoofing game lobby name to %s.", text+5);
		} else {
			AddMessage("-> Spoof game lobby name to what?");
		}
		return true;
	} else if(!strncmp(text, "/scn ", 5)) {
		if(strlen(text) > 5) {
			SpoofChannelName(text+5);
			AddMessage("-> Spoofing local channel name to %s.", text+5);
		} else {
			AddMessage("-> Spoof local channel name to what?");
		}
		return true;
	} else if(!strncmp(text, "/rejoin", 7)) {
		char *rejointo = new char[strlen(currentchannel)+1];
		strcpy(rejointo, currentchannel);
		SendChat("/join the void");
		QueueChat("/join %s", rejointo);
		delete [] rejointo;
		return true;
	} else if(!strncmp(text, "/cq", 3) || !strncmp(text, "/clearqueue", 11)) {
		queueManip.ClearQueue();
		AddMessage("-> The message queue has been cleared.");
		return true;
	} else if(!strncmp(text, "/uptime", 7)) {
		PrintUptimes();
		return true;
	} else if(!strncmp(text, "/msgbox ", 8)) {
		if(strlen(text) > 8) {
			strcpy(messageboxtext, text+8);
			CreateMessageBox();
		}
		return true;
	}

	QueueChat("%s", text);
	return true;
}

void AddChat(const char *username, const char *message) {
	structAddChat *chantext = new structAddChat;

	chantext->toggleByte1 = 0x00;
	chantext->toggleByte2 = 0x03;
	strcpy(chantext->username, username);
	strcpy(chantext->message, message);
	chantext->pointerToUsername = (unsigned long)chantext->username;
	chantext->pointerToMessage = (unsigned long)chantext->message;
	AddChatSub(chantext);
}

void AddMessage(const char *format, ...) {
	int len = 0;
	char *buffer;

	va_list args;
	va_start(args, format);
	len = _vscprintf(format, args) + 1;
	buffer = new char[len];
	vsprintf(buffer, format, args);

	structAddChat *chantext = new structAddChat;

	chantext->toggleByte1 = 0x00;
	chantext->toggleByte2 = 0x05;
	strcpy(chantext->username, "none");
	strcpy(chantext->message, buffer);
	chantext->pointerToUsername = (unsigned long)chantext->username;
	chantext->pointerToMessage = (unsigned long)chantext->message;
	AddChatSub(chantext);

	delete chantext;
	delete [] buffer;
}

void SendChat(const char *buffer) {
	strcpy(replytext, buffer);
	SendChatSub();
}

void QueueChat(const char *format, ...) {
	int len = 0;
	char *buffer;

	va_list args;
	va_start(args, format);
	len = _vscprintf(format, args) + 1;
	buffer = new char[len];
	vsprintf(buffer, format, args);
	queueManip.AddQueue(buffer, 1);
	delete [] buffer;
}

void QuickReply(char *text) {
	if(!strncmp(lastwhisperfrom, "Anonymous", 9)) {
		AddMessage("-> Nobody has whispered you yet.");
		return;
	}
	
	if(!strncmp(text, "/r ", 3)) {
		QueueChat("/w %s %s", lastwhisperfrom, text+3);
	} else if(!strncmp(text, "/reply ", 7)) {
		QueueChat("/w %s %s", lastwhisperfrom, text+7);
	}
	return;
}

void SetWhisperBackTarget(char *buffer) {
	char *ptr;
	char *text = new char[strlen(buffer)+1];
	strcpy(text, buffer);

	if(!strncmp(text, "/w ", 3) || !strncmp(text, "/m ", 3)) {
		ptr = strchr(text+3, ' ');
		if(ptr) *ptr = '\0';
		else {
			if(strlen(text) <= 3) {
				delete [] text;
				return;
			}
		}
		strcpy(lastwhisperto, text+3);
	} else if(!strncmp(text, "/msg ", 5)) {
		ptr = strchr(text+5, ' ');
		if(ptr) *ptr = '\0';
		else {
			if(strlen(text) <= 5) {
				delete [] text;
				return;
			}
		}
		strcpy(lastwhisperto, text+5);
	} else if(!strncmp(text, "/whisper ", 9)) {
		ptr = strchr(text+9, ' ');
		if(ptr) *ptr = '\0';
		else {
			if(strlen(text) <= 9) {
				delete [] text;
				return;
			}
		}
		strcpy(lastwhisperto, text+9);
	}
	delete [] text;
	return;
}

void SendWhisperBack(char *text) {
	if(!strncmp(lastwhisperto, "Anonymous", 8)) {
		AddMessage("-> You have not whispered anybody yet.");
		return;
	}
	QueueChat("/w %s %s", lastwhisperto, text);
	return;
}

void SpoofLobbyName(char *text) {
	char *address = (char*)0x1903C2A8;
	strcpy(address, text);
}

void SpoofChannelName(char *text) {
	char *address = (char*)0x1903C538;
	strcpy(address, text);
}

void PrintUptimes() {
	unsigned long sec  = 0;
	unsigned long min  = 0;
	unsigned long hour = 0;
	unsigned long day  = 0;
	LARGE_INTEGER lpPerformanceCount, lpFrequency;
	memset(&lpPerformanceCount, 0, sizeof(lpPerformanceCount));
	memset(&lpFrequency, 0, sizeof(lpFrequency));

		if(QueryPerformanceCounter(&lpPerformanceCount))
			QueryPerformanceFrequency(&lpFrequency);
		__int64 uptime = lpPerformanceCount.QuadPart / lpFrequency.QuadPart;
		sec = static_cast<unsigned long>(uptime);
		if(sec >  60) {
			min = sec / 60;
			sec %= 60;
		}
		if(min >  60) {
			hour = min / 60;
			min %= 60;
		}
		if(hour >  24) {
			day = hour / 24;
			hour %= 24;
		}
		AddMessage("-> System uptime: %u days, %u hours, %u minutes, %u seconds.", day, hour, min, sec);

		memset(&lpPerformanceCount, 0, sizeof(lpPerformanceCount));
		memset(&lpFrequency, 0, sizeof(lpFrequency));
		sec = min = hour = day = 0;

		if(QueryPerformanceCounter(&lpPerformanceCount))
			QueryPerformanceFrequency(&lpFrequency);
		__int64 connectionUptime = lpPerformanceCount.QuadPart / lpFrequency.QuadPart;
		connectionUptime -= startTime;
		sec = static_cast<unsigned long>(connectionUptime);
		if(sec >  60) {
			min = sec / 60;
			sec %= 60;
		}
		if(min >  60) {
			hour = min / 60;
			min %= 60;
		}
		if(hour >  24) {
			day = hour / 24;
			hour %= 24;
		}
		if(startTime == (unsigned long)-1)
			AddMessage("-> There was an error in retrieving the connection uptime.");
		else
			AddMessage("-> Time since injection: %d days, %d hours, %d minutes, %d seconds.", day, hour, min, sec);
}