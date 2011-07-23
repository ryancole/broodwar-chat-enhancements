#pragma once

struct structAddChat {
	unsigned long pointerToUsername;
	unsigned long toggleByte1;
	unsigned long toggleByte2;
	unsigned long pointerToMessage;
	char username[24];
	char message[255];
};

void HandleRecvWhisper();
void HandleOutgoingTextSub();
void AddChatSub(structAddChat *data);
void SendChatSub();
void EnterChannelEventSub();
void CreateMessageBox();